// KotoNotation.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>

class WavetableOscillator
{
public:
    WavetableOscillator(const juce::AudioSampleBuffer& wavetableToUse)
        : wavetable(wavetableToUse),
        tableSize(wavetable.getNumSamples() - 1)
    {
        jassert(wavetable.getNumChannels() == 1);
    }

    void setFrequency(float frequency, float sampleRate)
    {
        auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }

    forcedinline float getNextSample() noexcept
    {
        auto index0 = (unsigned int)currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float)index0;

        auto* table = wavetable.getReadPointer(0);
        auto value0 = table[index0];
        auto value1 = table[index1];

        auto currentSample = value0 + frac * (value1 - value0);

        if ((currentIndex += tableDelta) > (float)tableSize)
            currentIndex -= (float)tableSize;

        return currentSample;
    }

private:
    const juce::AudioSampleBuffer& wavetable;
    const int tableSize;
    float currentIndex = 0.0f, tableDelta = 0.0f;
};

class MainContentComponent : public juce::AudioAppComponent
{
public:
    MainContentComponent()
    {
        createWavetable();
        
        setSize(400, 200);
        setAudioChannels(0, 2);
    }

    ~MainContentComponent() override
    {
        shutdownAudio();
    }

    void createWavetable()
    {
        sineTable.setSize(1, (int)tableSize + 1);
        sineTable.clear();

        auto* samples = sineTable.getWritePointer(0);

        int harmonics[] = { 1, 3, 5, 6, 7, 9, 13, 15 };
        float harmonicWeights[] = { 0.5f, 0.1f, 0.05f, 0.125f, 0.09f, 0.005f, 0.002f, 0.001f };     // [1]

        jassert(juce::numElementsInArray(harmonics) == juce::numElementsInArray(harmonicWeights));

        for (auto harmonic = 0; harmonic < juce::numElementsInArray(harmonics); ++harmonic)
        {
            auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1) * harmonics[harmonic]; // [2]
            auto currentAngle = 0.0;

            for (unsigned int i = 0; i < tableSize; ++i)
            {
                auto sample = std::sin(currentAngle);
                samples[i] += (float)sample * harmonicWeights[harmonic];                           // [3]
                currentAngle += angleDelta;
            }
        }

        samples[tableSize] = samples[0];
    }

    void prepareToPlay(int, double sampleRate) override
    {
        auto numberOfOscillators = 10;

        for (auto i = 0; i < numberOfOscillators; ++i)
        {
            auto* oscillator = new WavetableOscillator(sineTable);

            auto midiNote = juce::Random::getSystemRandom().nextDouble() * 36.0 + 48.0;
            auto frequency = 440.0 * pow(2.0, (midiNote - 69.0) / 12.0);

            oscillator->setFrequency((float)frequency, (float)sampleRate);
            oscillators.add(oscillator);
        }

        level = 0.25f / (float)numberOfOscillators;
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        bufferToFill.clearActiveBufferRegion();

        for (auto oscillatorIndex = 0; oscillatorIndex < oscillators.size(); ++oscillatorIndex)
        {
            auto* oscillator = oscillators.getUnchecked(oscillatorIndex);

            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                auto levelSample = oscillator->getNextSample() * level;

                leftBuffer[sample] += levelSample;
                rightBuffer[sample] += levelSample;
            }
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(250, 190, 120));

        g.setColour(juce::Colours::black);
        g.setFont(24.0f);
        g.drawFittedText("Hello all",
            getLocalBounds(),
            juce::Justification::centred,
            1);
    }

private:

    const unsigned int tableSize = 1 << 7;
    float level = 0.0f;

    juce::AudioSampleBuffer sineTable;
    juce::OwnedArray<WavetableOscillator> oscillators;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
