#include "AudioSystem.h"

// FMOD is not integrated yet on macOS. Provide a no-op implementation so the
// rest of the game can compile and link.

Sample::Sample() :
  name(""),
  loop(false),
  paused(false),
  sample(nullptr),
  channel(nullptr)
{
}

Sample::~Sample() = default;

void Sample::SetVolume(float /*volume*/) {}
void Sample::SetLoop(bool doLoop) { loop = doLoop; }
void Sample::SetPaused(bool doPause) { paused = doPause; }

AudioSystem::AudioSystem(void)
{
  system = nullptr;
  bPlay = false;
}

AudioSystem::~AudioSystem(void) = default;

bool AudioSystem::Init() { return true; }

void AudioSystem::Update() {}

Sample* AudioSystem::Load(std::string /*filename*/, float /*volume*/)
{
  return nullptr;
}

bool AudioSystem::Load(std::string /*filename*/, std::string /*name*/, float /*volume*/)
{
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

bool AudioSystem::Play(std::string /*name*/, bool /*doLoop*/) { return true; }
bool AudioSystem::Play(Sample* /*sample*/, bool /*doLoop*/) { return true; }
bool AudioSystem::PlayMusic(std::string name, bool doLoop) { return Play(name, doLoop); }
bool AudioSystem::PlayMusic(Sample* sample, bool doLoop) { return Play(sample, doLoop); }

void AudioSystem::PauseMusic(std::string /*name*/) {}
void AudioSystem::UnpauseMusic(std::string /*name*/) {}
void AudioSystem::Stop(std::string /*name*/) {}
void AudioSystem::Stop(Sample* /*sample*/) {}
void AudioSystem::StopAll() {}
void AudioSystem::StopAllExcept(std::string /*name*/) {}
void AudioSystem::Delete(std::string /*name*/) {}

bool AudioSystem::IsPlaying(std::string /*name*/) { return false; }
bool AudioSystem::IsPlaying(Sample* /*sample*/) { return false; }

bool AudioSystem::SampleExists(std::string /*name*/) { return false; }

Sample* AudioSystem::FindSample(std::string /*name*/)
{
  return nullptr;
}
