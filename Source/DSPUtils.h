#pragma once

#include <JuceHeader.h>

class DSPUtils
{
public:
    static float dbToGain(float db)
    {
        return std::pow(10.0f, db / 20.0f);
    }
    
    static float gainToDb(float gain)
    {
        return 20.0f * std::log10(gain);
    }
    
    static float clipSample(float sample, float threshold = 1.0f)
    {
        return juce::jlimit(-threshold, threshold, sample);
    }
    
    static float softClip(float sample)
    {
        if (sample > 1.0f)
            return 2.0f / 3.0f;
        else if (sample < -1.0f)
            return -2.0f / 3.0f;
        else
            return sample - (sample * sample * sample) / 3.0f;
    }
    
    static float interpolateLinear(float a, float b, float t)
    {
        return a + t * (b - a);
    }
    
    static float frequencyToMidi(float frequency)
    {
        return 69.0f + 12.0f * std::log2(frequency / 440.0f);
    }
    
    static float midiToFrequency(float midiNote)
    {
        return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
    }
};
