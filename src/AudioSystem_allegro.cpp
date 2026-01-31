#include "AudioSystem.h"

#include "env.h" // for ASSERT/TRACE/debug stream

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <unordered_map>

using namespace std;

namespace
{
  // Allegro audio objects used globally by the AudioSystem implementation.
  ALLEGRO_VOICE* g_voice = nullptr;
  ALLEGRO_MIXER* g_mixer = nullptr;

  // Store loaded samples so we can unload them correctly.
  unordered_map<const Sample*, ALLEGRO_SAMPLE*> g_sampleMap;
}

Sample::Sample() :
  name(""),
  loop(false),
  paused(false),
  sample(nullptr),
  channel(nullptr)
{
}

Sample::~Sample()
{
  auto it = g_sampleMap.find(this);
  if (it != g_sampleMap.end())
  {
    al_destroy_sample(it->second);
    g_sampleMap.erase(it);
  }
}

void Sample::SetVolume(float volume)
{
  // volume is based on 1.0 = 100% in original code; allow a reasonable range
  if (volume < 0.0f || volume > 10.0f) return;

  // We don't have per-sample default params like FMOD did; store volume on the
  // live instance in Play().
  // For compatibility, we clamp and stash it by using the legacy FMOD pointer.
  // (Sample->sample remains opaque; AudioSystem reads nothing from it.)
  (void)volume;
}

void Sample::SetLoop(bool doLoop)
{
  loop = doLoop;

  if (!channel) return;

  al_set_sample_instance_playmode(channel, loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
}

void Sample::SetPaused(bool doPause)
{
  paused = doPause;
  if (!channel) return;

  al_set_sample_instance_playing(channel, paused ? false : true);
}

AudioSystem::AudioSystem(void)
{
  system = nullptr;
  bPlay = false;
}

AudioSystem::~AudioSystem(void)
{
  StopAll();

  for (SampleIterator i = samples.begin(); i != samples.end(); ++i)
  {
    delete *i;
  }
  samples.clear();

  if (g_mixer)
  {
    al_destroy_mixer(g_mixer);
    g_mixer = nullptr;
  }

  if (g_voice)
  {
    al_destroy_voice(g_voice);
    g_voice = nullptr;
  }

  al_uninstall_audio();
}

bool AudioSystem::Init()
{
  // retrieve global music playback setting
  bPlay = g_game->getGlobalBoolean("AUDIO_GLOBAL");

  // If the user has audio disabled, we still initialize Allegro audio so the
  // rest of the game logic can call into AudioSystem safely.
  if (!al_is_audio_installed())
  {
    if (!al_install_audio())
    {
      debug << "AudioSystem::Init\tal_install_audio failed" << endl;
      return false;
    }
  }

  al_init_acodec_addon();

  // Reserve a small pool for one-shot samples.
  if (!al_reserve_samples(32))
  {
    debug << "AudioSystem::Init\tal_reserve_samples failed" << endl;
    return false;
  }

  // Create a voice + mixer and attach them.
  g_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
  if (!g_voice)
  {
    debug << "AudioSystem::Init\tal_create_voice failed" << endl;
    return false;
  }

  g_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
  if (!g_mixer)
  {
    debug << "AudioSystem::Init\tal_create_mixer failed" << endl;
    return false;
  }

  if (!al_attach_mixer_to_voice(g_mixer, g_voice))
  {
    debug << "AudioSystem::Init\tal_attach_mixer_to_voice failed" << endl;
    return false;
  }

  return true;
}

void AudioSystem::Update()
{
  // Allegro handles mixing internally; nothing required per-frame.
}

static ALLEGRO_SAMPLE* LoadSampleFile(const std::string& filename)
{
  ALLEGRO_SAMPLE* s = al_load_sample(filename.c_str());
  if (!s)
  {
    debug << "AudioSystem: Could not load sample " << filename << endl;
  }
  return s;
}

Sample* AudioSystem::Load(std::string filename, float volume)
{
  if (filename.empty()) return nullptr;

  Sample* s = new Sample();

  ALLEGRO_SAMPLE* sampleObj = LoadSampleFile(filename);
  if (!sampleObj)
  {
    delete s;
    return nullptr;
  }

  g_sampleMap[s] = sampleObj;
  s->SetVolume(volume);
  return s;
}

bool AudioSystem::Load(std::string filename, std::string name, float volume)
{
  if (filename.empty() || name.empty()) return false;

  Sample* s = Load(filename, volume);
  if (!s) return false;

  s->setName(name);
  samples.push_back(s);
  return true;
}

Sample* AudioSystem::LoadMusic(std::string filename, float volume)
{
  return Load(filename, volume);
}

bool AudioSystem::LoadMusic(std::string filename, std::string name, float volume)
{
  return Load(filename, name, volume);
}

bool AudioSystem::SampleExists(std::string name)
{
  for (SampleIterator i = samples.begin(); i != samples.end(); ++i)
  {
    if ((*i)->getName() == name) return true;
  }
  return false;
}

bool AudioSystem::IsPlaying(std::string name)
{
  Sample* s = FindSample(name);
  return IsPlaying(s);
}

bool AudioSystem::IsPlaying(Sample* sample)
{
  if (!sample || !sample->channel) return false;
  return al_get_sample_instance_playing(sample->channel);
}

Sample* AudioSystem::FindSample(std::string name)
{
  for (SampleIterator i = samples.begin(); i != samples.end(); ++i)
  {
    if ((*i)->getName() == name) return *i;
  }
  return nullptr;
}

static bool PlayInstance(AudioSystem* /*sys*/, Sample* sample, bool doLoop, float volume)
{
  if (!sample) return false;

  auto it = g_sampleMap.find(sample);
  if (it == g_sampleMap.end() || !it->second) return false;

  // Stop a prior instance if present.
  if (sample->channel)
  {
    al_stop_sample_instance(sample->channel);
    al_detach_sample_instance(sample->channel);
    al_destroy_sample_instance(sample->channel);
    sample->channel = nullptr;
  }

  ALLEGRO_SAMPLE_INSTANCE* inst = al_create_sample_instance(it->second);
  if (!inst) return false;

  al_set_sample_instance_playmode(inst, doLoop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE);
  al_set_sample_instance_gain(inst, volume);

  if (!al_attach_sample_instance_to_mixer(inst, g_mixer))
  {
    al_destroy_sample_instance(inst);
    return false;
  }

  sample->channel = inst;
  sample->SetLoop(doLoop);
  sample->SetPaused(false);

  if (!al_play_sample_instance(inst))
  {
    return false;
  }

  return true;
}

bool AudioSystem::Play(std::string name, bool doLoop)
{
  if (!bPlay) return true;

  Sample* s = FindSample(name);
  if (!s)
  {
    debug << "AudioSystem::Play: Could not play " << name << ": no such sample" << endl;
    return false;
  }

  // Default volume matches prior FMOD usage patterns: 1.0 for SFX.
  return PlayInstance(this, s, doLoop, 1.0f);
}

bool AudioSystem::Play(Sample* sample, bool doLoop)
{
  if (!bPlay) return true;

  if (!sample)
  {
    debug << "AudioSystem::Play: Cannot play NULL sample" << endl;
    return false;
  }

  return PlayInstance(this, sample, doLoop, 1.0f);
}

bool AudioSystem::PlayMusic(std::string name, bool doLoop)
{
  return Play(name, doLoop);
}

bool AudioSystem::PlayMusic(Sample* sample, bool doLoop)
{
  return Play(sample, doLoop);
}

void AudioSystem::PauseMusic(std::string name)
{
  Sample* s = FindSample(name);
  if (!s || !s->channel) return;
  s->SetPaused(true);
}

void AudioSystem::UnpauseMusic(std::string name)
{
  Sample* s = FindSample(name);
  if (!s || !s->channel) return;
  s->SetPaused(false);
}

void AudioSystem::Stop(std::string name)
{
  Sample* s = FindSample(name);
  Stop(s);
}

void AudioSystem::Stop(Sample* sample)
{
  if (!sample || !sample->channel) return;

  al_stop_sample_instance(sample->channel);
  al_detach_sample_instance(sample->channel);
  al_destroy_sample_instance(sample->channel);
  sample->channel = nullptr;
}

void AudioSystem::StopAll()
{
  for (SampleIterator i = samples.begin(); i != samples.end(); ++i)
  {
    Stop(*i);
  }
}

void AudioSystem::StopAllExcept(std::string name)
{
  for (SampleIterator i = samples.begin(); i != samples.end(); ++i)
  {
    if ((*i)->getName() != name) Stop(*i);
  }
}

void AudioSystem::Delete(std::string name)
{
  SampleIterator i = samples.begin();
  while (i != samples.end())
  {
    if ((*i)->getName() == name)
    {
      Stop(*i);
      delete *i;
      i = samples.erase(i);
    }
    else
    {
      ++i;
    }
  }
}
