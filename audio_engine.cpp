#include <iostream>
#include "audio_engine.h"

#define TAU M_PI * 2

using std::cout;
using std::endl;

AudioEngine::AudioEngine(int frequency, int volume, WaveForm waveform)
{
    AudioEngine::mFrequency = frequency;
    AudioEngine::mVolume = volume;
    AudioEngine::mTime = 0;

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = mSampleRate;
    desiredSpec.format = AUDIO_S16;
    desiredSpec.channels = 1;
    desiredSpec.samples = mSampleSize;
    desiredSpec.callback = &audioCallback;
    desiredSpec.userdata = this;

    switch(waveform) {
        case SINE:
            generateWave = &generateSine;
            break;
        case SQUARE:
            generateWave = &generateSquare;
            break;
        case TRIANGLE:
            generateWave = &generateTriangle;
            break;
        case SAW:
            generateWave = &generateSaw;
            break;
        default:
            cout << "Undefined waveform" << endl;
            break;
    }

    mDeviceId = SDL_OpenAudioDevice(
        NULL,
        SDL_FALSE,
        &desiredSpec,
        &mReceivedSpec,
        0
    );
    if (mDeviceId == 0) {
        cout << "Failed to open audio device! Error: " << SDL_GetError() << endl;
    }
}

AudioEngine::~AudioEngine()
{
    SDL_CloseAudioDevice(mDeviceId);
}

void AudioEngine::play()
{
    SDL_PauseAudioDevice(mDeviceId, SDL_FALSE);
}

void AudioEngine::pause()
{
    SDL_PauseAudioDevice(mDeviceId, SDL_TRUE);
}

void AudioEngine::audioCallback(void *userdata, Uint8 *stream, int len)
{
    const auto engine = reinterpret_cast<AudioEngine *>(userdata);
    engine->fillBuffer(stream, len);
}

void AudioEngine::fillBuffer(const Uint8* const stream, int len)
{
    short *out = (short *)stream;
    for (int i = 0; i < (len / sizeof(short)); i++) {
        double value = generateWave(mFrequency, mTime);
        out[i] = mVolume * value;
        mTime += mTimeStep;
    }
}

double AudioEngine::generateSine(int frequency, double time)
{
    return sin(frequency * TAU * time);
}

double AudioEngine::generateTriangle(int frequency, double time)
{
    return asin(sin(frequency * TAU * time)) * (2.0 / M_PI);
}

double AudioEngine::generateSquare(int frequency, double time)
{
    return sin(frequency * TAU * time) > 0 ? 1.0 : -1.0;
}

// double AudioEngine::generateSaw(int frequency, double time)
// {
//     double output = 0.0;
//     for (double f = 1.0; f < 40.0; f++) {
//         output += (pow(-1.0, f) / f) * sin(TAU * f * time) / f;
//     }
//     return output * (-2.0 / M_PI);
// }
double AudioEngine::generateSaw(int frequency, double time)
{
    return (2.0 / M_PI) * frequency * M_PI * fmod(time, 1.0 / frequency) - (M_PI / 2.0);
}
