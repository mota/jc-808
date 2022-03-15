#include <cmath>
#include <iostream>
#include "instrument.h"

Instrument::Instrument(int freq, Envelope env)
: mFreq(freq), mEnv(env)
{
    mDuration = mEnv.getDuration();
    mElapsed = 0.0;
    mTriggered = false;
    mPlaying = false;
}

Instrument::~Instrument() {}

void Instrument::trigger()
{
    m_triggered = true;
    m_playing = true;
    m_elapsed = 0.0;
}

void Instrument::release()
{
    m_triggered = false;
}

bool Instrument::isTriggered()
{
    return m_triggered;
}

bool Instrument::isPlaying()
{
    return m_playing;
}

void Instrument::updateBy(double time)
{
    if (!m_triggered) return;

    m_elapsed += time;
    if (m_elapsed > m_duration) {
        m_playing = false;
        m_elapsed = 0.0;
    }
}

double Instrument::getSample()
{
    return sin(m_freq * TAU * m_elapsed) * m_env->getAmplitude(m_elapsed);
}

std::string Instrument::getName()
{
    return m_name;
}
