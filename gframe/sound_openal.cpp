#include "sound_openal.h"
#include <array>
#include <iterator>
#include <mpg123.h>
#include <sndfile.h>
#include "utils.h"

namespace YGOpen {

static void delete_ALCdevice(ALCdevice* ptr)
{
    if (ptr) {
        alcCloseDevice(ptr);
    }
}
static void delete_ALCcontext(ALCcontext* ptr)
{
    if (ptr) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(ptr);
    }
}

OpenALSingleton::OpenALSingleton() : device(nullptr, delete_ALCdevice), context(nullptr, delete_ALCcontext) {
    device.reset(alcOpenDevice(nullptr));
    if (!device) {
        throw std::runtime_error("Failed to create OpenAL audio device!");
    }
    context.reset(alcCreateContext(device.get(), nullptr));
    if (!alcMakeContextCurrent(context.get())) {
        throw std::runtime_error("Failed to set OpenAL audio context!");
    }
    mpg123_init();
}

OpenALSingleton::~OpenALSingleton() {
    mpg123_exit();
}

OpenALSoundLayer::OpenALSoundLayer(const std::unique_ptr<OpenALSingleton>& openal) : openal(openal), buffers(), playing() {}

OpenALSoundLayer::~OpenALSoundLayer() {
    stopAll();
    for (const auto& iter : buffers) {
        alDeleteBuffers(1, &iter.second->id);
    }   
}

static inline bool alUtilInitBuffer(std::shared_ptr<OpenALSoundBuffer> data) {
    alGetError();
    alGenBuffers(1, &data->id);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) return false;

    alBufferData(data->id, data->format, data->buffer.data(), data->buffer.size(), data->frequency);
    error = alGetError();
    if (error != AL_NO_ERROR) return false;

    return true;
}

static inline ALenum alUtilFormatFromMp3(const int channels, const int encoding) {
    if (channels & MPG123_STEREO) {
        if (encoding & MPG123_ENC_SIGNED_16) {
            return AL_FORMAT_STEREO16;
        }
        else {
            return AL_FORMAT_STEREO8;
        }
    }
    else {
        if (encoding & MPG123_ENC_SIGNED_16) {
            return AL_FORMAT_MONO16;
        }
        else {
            return AL_FORMAT_MONO8;
        }
    }
}

static std::shared_ptr<OpenALSoundBuffer> loadMp3(const std::string& filename) {
    auto data = std::make_shared<OpenALSoundBuffer>();
    std::vector<unsigned char> buffer;
    size_t bufferSize;
    int mpgError, channels, encoding;
    long rate;
    auto mpgHandle = mpg123_new(nullptr, &mpgError);
    if (!mpgHandle) return nullptr;
    if (mpg123_open(mpgHandle, filename.c_str()) != MPG123_OK ||
        mpg123_getformat(mpgHandle, &rate, &channels, &encoding) != MPG123_OK) {
        mpg123_delete(mpgHandle);
        return nullptr;
    }
    mpg123_format_none(mpgHandle);
    mpg123_format(mpgHandle, rate, channels, encoding);
    
    bufferSize = mpg123_outblock(mpgHandle);
    buffer.resize(bufferSize);
    for(size_t read = 1; read != 0 && mpgError == MPG123_OK; ) {
        mpgError = mpg123_read(mpgHandle, buffer.data(), bufferSize, &read);
        data->buffer.insert(data->buffer.end(), buffer.begin(), buffer.end());
    }
    mpg123_close(mpgHandle);
    mpg123_delete(mpgHandle);
    if (mpgError != MPG123_DONE) return nullptr;

    data->format = alUtilFormatFromMp3(channels, encoding);
    data->frequency = rate;

    return alUtilInitBuffer(data) ? data : nullptr;
}

static std::shared_ptr<OpenALSoundBuffer> loadSnd(const std::string& filename) {
    auto data = std::make_shared<OpenALSoundBuffer>();
    SF_INFO info;
    std::unique_ptr<SNDFILE, std::function<void(SNDFILE*)>> file(
        sf_open(filename.c_str(), SFM_READ, &info), 
        [](SNDFILE* ptr) { sf_close(ptr);  });
    if (!file) return nullptr;
    data->frequency = info.samplerate;
    data->format = info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    data->buffer.resize(info.frames * info.channels * sizeof(short));
    if (sf_readf_short(file.get(), reinterpret_cast<short*>(data->buffer.data()), info.frames) != info.frames) return nullptr;
    return alUtilInitBuffer(data) ? data : nullptr;
}

bool OpenALSoundLayer::load(const std::string& filename)
{
    std::shared_ptr<OpenALSoundBuffer> data(nullptr);
    auto ext = ygo::Utils::GetFileExtension(filename);
    if (ext == "mp3") {
        data = loadMp3(filename);
    }
    else {
        data = loadSnd(filename);
    }
    if (!data) return false;
    buffers[filename] =  data;
    return true;
}

int OpenALSoundLayer::play(const std::string& filename, bool loop)
{
    maintain();
    if (buffers.find(filename) == buffers.end()) {
        if (!load(filename)) return -1;
    }
    ALuint buffer = buffers.at(filename)->id;

    ALuint source;
    alGetError();
    alGenSources(1, &source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) return -1;
    
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcef(source, AL_GAIN, volume);
    alSourcePlay(source);
    error = alGetError();
    if (error != AL_NO_ERROR) {
        alDeleteSources(1, &source);
        return -1;
    }
    playing.insert(source);
    return source;
}

bool OpenALSoundLayer::exists(int sound)
{
    maintain();
    return playing.find(sound) != playing.end();
}

void OpenALSoundLayer::stop(int sound)
{
    maintain();
    const auto iter = playing.find(sound);
    if (playing.find(sound) != playing.end()) {
        alSourceStop(sound);
        alDeleteSources(1, &*iter);
        playing.erase(iter);
    }
}

void OpenALSoundLayer::stopAll()
{
    for (const auto& iter : playing) {
        alSourceStop(iter);
        alDeleteSources(1, &iter);
    }
    playing.clear();
}

void OpenALSoundLayer::setVolume(float gain)
{
    volume = gain;
    for (const auto& iter : playing) {
        alSourcef(iter, AL_GAIN, volume);
    }
}

void OpenALSoundLayer::maintain() {
    std::unordered_set<ALuint> toDelete;
    for (const auto& iter : playing) {
        ALint state;
        alGetSourcei(iter, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            toDelete.insert(iter);
        }
    }
    for (const auto& iter : toDelete) {
        alSourceStop(iter);
        alDeleteSources(1, &iter);
        playing.erase(iter);
    }
}

} // namespace YGOpen
