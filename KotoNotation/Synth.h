#pragma once
#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>


class kotoSynth : public juce::AudioAppComponent
{
public:
    std::array <juce::ADSR, 6> adsr;
    double fund = 146.83;

    kotoSynth()
    {

        // Open audio device (0 inputs, 2 outputs)
        setAudioChannels(0, 2);

    }

    ~kotoSynth() override
    {
        shutdownAudio();
    }

    void resized() override
    {
    }

    void playNote() {
        for (int i = 0; i < adsr.size(); i++) {
            adsr[i].noteOn();
        };
    }


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        for (int i = 0; i < oscArray.size(); i++) {
            oscArray[i].initialise({ [](float x) {return std::sin(x); } });
        };


        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.numChannels = 2;

        
        double freq = fund;
        double length = sqrt(215 * (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi) / (0.086197 * (fund * fund)));
        double EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;
        double a = (EI * 9.8596) / (2 * 215 * pow(length, 2));
        float attk = 0.001f;
        float dec = 1.5f;

        for (int i = 0; i < oscArray.size(); i++) {
            oscArray[i].prepare(spec);

            oscArray[i].setFrequency(freq);
            adsr[i].setSampleRate(sampleRate);

            // ADSR:
            adsr[i].setParameters(juce::ADSR::Parameters(attk, dec, 0.0f, 0.0f));

            freq = ((i + 1) * fund) * (1 + (a * (pow((i + 1), 2))));
            attk -= 0.0018;
            dec /= 3.0f;


        };


        filter.prepare(spec);
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq * 1.5);

        //Reverb
        verb.prepare(spec);
        verb.setParameters(juce::dsp::Reverb::Parameters{ 0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f });


    }

    void updateFrequency(double sampleRate) {
        double freq = fund;
        double length = sqrt(215 * (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi) / (0.086197 * (fund * fund)));
        double EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;
        double a = (EI * 9.8596) / (2 * 215 * pow(length, 2));
        float attk = 0.001f;
        float dec = 1.5f;

        for (int i = 0; i < oscArray.size(); i++) {
            oscArray[i].setFrequency(freq);

            // ADSR:
            adsr[i].setParameters(juce::ADSR::Parameters(attk, dec, 0.0f, 0.0f));

            freq = ((i + 1) * fund) * (1 + (a * (pow((i + 1), 2))));
            attk -= 0.0018;
            dec /= 3.0f;
        };


        if (freq > sampleRate / 4) {
            freq = sampleRate/4;
        }

        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq * 1.5);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {


        auto* buffer = bufferToFill.buffer;
        int numChannels = buffer->getNumChannels();
        int numSamples = buffer->getNumSamples();


        for (int i = 0; i < numSamples; ++i)
        {

            float mix = 0;
            float m = mix;

            for (int i = 0; i < oscArray.size(); i++) {
                float env = adsr[i].getNextSample();
                float p = oscArray[i].processSample(m);
                mix += p * env;
            };


            for (int ch = 0; ch < numChannels; ++ch)
            {
                float sample = buffer->getSample(ch, i);
                sample += mix;


                sample = filter.processSample(sample);

                buffer->getWritePointer(ch)[i] = sample;



            }

            juce::dsp::AudioBlock<float> block(*buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);

        }
    }

    void releaseResources() override {}

private:
    std::array<juce::dsp::Oscillator<double>, 6> oscArray;
    juce::dsp::IIR::Filter<float> filter;
    juce::dsp::Reverb verb;
};