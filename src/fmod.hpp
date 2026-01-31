#ifndef FMOD_HPP
#define FMOD_HPP

// Stub FMOD header for compilation without FMOD
// Audio is disabled until FMOD is properly integrated

namespace FMOD {
    class System;
    class Sound;
    class Channel;
    class ChannelGroup;

    enum FMOD_RESULT {
        FMOD_OK = 0,
        FMOD_ERR_UNINITIALIZED
    };
}

typedef FMOD::FMOD_RESULT FMOD_RESULT;

#endif
