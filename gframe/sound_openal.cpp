#include "sound_openal.h"
#include <array>
#include <iterator>
#include <mpg123.h>
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

union array_int32 {
    char c[4];
    int32_t i;
};

union array_int16 {
    char c[2];
    int16_t i;
};

static inline void read32(std::istream& in, array_int32& out) {
    in.read(out.c, 4);
}

static inline void read16(std::istream& in, array_int16& out) {
    in.read(out.c, 2);
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

static inline ALenum alUtilFormatFromWav(const int16_t channels, const int16_t bitsPerSample) {
    if (channels == 1) {
        if (bitsPerSample == 8) {
            return AL_FORMAT_MONO8;
        }
        else { // Assume 16
            return AL_FORMAT_MONO16;
        }
    }
    else { // Assume 2
        if (bitsPerSample == 8) {
            return AL_FORMAT_STEREO8;
        }
        else { // Assume 16
            return AL_FORMAT_STEREO16;
        }
    }
}

static constexpr std::array<char, 4> RIFF({ 'R','I','F','F' });
static constexpr std::array<char, 4> WAVE({ 'W','A','V','E' });
static constexpr std::array<char, 4> FMT_({ 'f','m','t',' ' });
static constexpr std::array<char, 4> LIST({ 'L','I','S','T' });
static constexpr std::array<char, 4> DATA({ 'd','a','t','a' });

static std::shared_ptr<OpenALSoundBuffer> loadWav(const std::string& filename) {
    std::array<char, 4> header;
    array_int32 throwawayInt32;
    std::array<char, 4> format;
    std::array<char, 4> info;
    array_int16 audioFormat, channels;
    array_int32 sampleRate, byteRate;
    array_int16 throwawayInt16, bitsPerSample;
    std::array<char, 4> prefix;
    array_int32 size;
    auto data = std::make_shared<OpenALSoundBuffer>();
    std::ifstream file(filename, std::ios_base::binary);

    if (!file.good()) return nullptr;
    file.read(header.data(), 4);
    if (header != RIFF) return nullptr;
    read32(file, throwawayInt32);
    file.read(format.data(), 4);
    if (format != WAVE) return nullptr;
    file.read(info.data(), 4);
    if (info != FMT_) return nullptr;
    read32(file, throwawayInt32);
    read16(file, audioFormat);
    read16(file, channels);
    read32(file, sampleRate);
    read32(file, byteRate);
    read16(file, throwawayInt16);
    read16(file, bitsPerSample);

    file.read(prefix.data(), 4);
    if (prefix == LIST) {
        read32(file, throwawayInt32);
        file.seekg(throwawayInt32.i, std::ios_base::cur);
        file.read(prefix.data(), 4);
    }
    if (prefix != DATA) return nullptr;
    read32(file, size);
    data->buffer.reserve(size.i);
    data->buffer.insert(data->buffer.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());
    data->format = alUtilFormatFromWav(channels.i, bitsPerSample.i);
    data->frequency = sampleRate.i;

    return alUtilInitBuffer(data) ? data : nullptr;
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

bool OpenALSoundLayer::load(const std::string& filename)
{
    std::shared_ptr<OpenALSoundBuffer> data(nullptr);
    auto ext = ygo::Utils::GetFileExtension(filename);
    if (ext == "wav") {
        data = loadWav(filename);
    } else if (ext == "mp3") {
        data = loadMp3(filename);
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
