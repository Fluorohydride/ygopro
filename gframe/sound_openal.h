#ifndef YGOPEN_SOUND_OPENAL_H
#define YGOPEN_SOUND_OPENAL_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>

namespace YGOpen {

/* Modified from minetest: src/client/sound.h, src/client/sound_openal.cpp
 * https://github.com/minetest/minetest
 * Licensed under GNU LGPLv2.1
 */

struct OpenALSoundBuffer
{
    ALenum format;
    ALsizei frequency;
    ALuint id;
    std::vector<char> buffer;
};

class OpenALSingleton {
public:
    OpenALSingleton();
    ~OpenALSingleton();
    std::unique_ptr<ALCdevice, void (*)(ALCdevice* ptr)> device;
    std::unique_ptr<ALCcontext, void(*)(ALCcontext* ptr)> context;
};

class OpenALSoundLayer {
public:
    OpenALSoundLayer(const std::unique_ptr<OpenALSingleton>& openal);
    ~OpenALSoundLayer();
    bool load(const std::string& filename);
    int play(const std::string& filename, bool loop);
    bool exists(int sound);
    void stop(int sound);
    void stopAll();
    void setVolume(float gain);
private:
    void maintain();
    const std::unique_ptr<OpenALSingleton>& openal;
    std::unordered_map<std::string, std::shared_ptr<OpenALSoundBuffer>> buffers;
    std::unordered_set<ALuint> playing;
    float volume = 1.0f;
};

}

#endif //YGOPEN_SOUND_OPENAL_H
