#pragma once
#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>

struct SineWaveSound : public juce::SynthesiserSound
{
    SineWaveSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

struct Voice : public juce::SynthesiserVoice
{
    Voice()
    {

    }

    struct Oscillator
    {
        float getNextSample()
        {
            const auto s = (std::sin(phase));
            phase += delta;

            phase += delta;
            if (phase >= juce::MathConstants<float>::twoPi)
                phase -= juce::MathConstants<float>::twoPi;

            return s;
        }

        float delta = 0;
        float phase = 0;
    };

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
        juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        const auto fund = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber - 12); //Because the keyboard marks C3 as 60 for some reason
        double freq = fund;
        double length = sqrt(215 * (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi) / (0.086197 * (fund * fund)));
        double EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;
        double a = (EI * 9.8596) / (2 * 215 * pow(length, 2));
        float attk = 0.001f;
        float dec = 2.0f;

        for (size_t i = 0; i < 7; i++)
        {
            auto& osc = oscillators[i];

            osc.delta = ((freq) / (double)getSampleRate()) * juce::MathConstants<double>::twoPi;
            osc.phase = 0.0f;// +(0.1f * i);

            freq = ((i + 1) * fund) * (1 + (a * (pow((i + 1), 2))));


            adsr[i].setSampleRate(getSampleRate());
            adsr[i].setParameters(juce::ADSR::Parameters::Parameters(attk, dec, 0.0f, 0.0f));
            adsr[i].noteOn();


            attk -= 0.0018f;
            dec /= 2.0f;
        }

        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), freq * 1.5));
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        for (size_t i = 0; i < 7; i++)
        {
            adsr[i].noteOff();
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        constexpr auto oscillatorCount = 7;


        auto* l = outputBuffer.getWritePointer(0);
        auto* r = outputBuffer.getWritePointer(1);

        for (int i = 0; i < numSamples; i++)
        {
            float left = 0;
            float right = 0;

            for (size_t o = 0; o < oscillatorCount; o++)
            {
                auto at = adsr[o].getNextSample();
                auto& osc = oscillators[o];
                auto s = at * osc.getNextSample();

                s = filter.processSingleSampleRaw(s);

                left += s;
                right += s;
            }


            l[i] += left;
            r[i] += right;
        }

    }


    std::array<juce::ADSR, 7> adsr;
    std::array<Oscillator, 7> oscillators;
    juce::IIRFilter filter;

private:
};

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState)
        //: keyboardState(keyState)
    {
        for (auto i = 0; i < 4; ++i)
            synth.addVoice(new Voice());

        synth.addSound(new SineWaveSound());
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate(sampleRate);
        midiCollector.reset(sampleRate); // [10]
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples); // [11]

        //keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample,
          //  bufferToFill.numSamples, true);

        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi,
            bufferToFill.startSample, bufferToFill.numSamples);
    }

    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

private:
    //juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
    juce::MidiMessageCollector midiCollector;
};

//==============================================================================
class SynthContentComponent : public juce::AudioAppComponent,
    private juce::Timer
{
public:
    SynthContentComponent()
        : synthAudioSource(keyboardState),
        keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible(midiInputListLabel);
        midiInputListLabel.setText("MIDI Input:", juce::dontSendNotification);
        midiInputListLabel.attachToComponent(&midiInputList, true);

        auto midiInputs = juce::MidiInput::getAvailableDevices();
        addAndMakeVisible(midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");

        juce::StringArray midiInputNames;
        for (auto input : midiInputs)
            midiInputNames.add(input.name);

        midiInputList.addItemList(midiInputNames, 1);
        midiInputList.onChange = [this] { setMidiInput(midiInputList.getSelectedItemIndex()); };

        for (auto input : midiInputs)
        {
            if (deviceManager.isMidiInputDeviceEnabled(input.identifier))
            {
                setMidiInput(midiInputs.indexOf(input));
                break;
            }
        }

        if (midiInputList.getSelectedId() == 0)
            setMidiInput(0);

        addAndMakeVisible(keyboardComponent);
        setAudioChannels(0, 2);

        setSize(600, 190);
        startTimer(400);
    }

    ~SynthContentComponent() override
    {
        shutdownAudio();
    }

    void resized() override
    {
        midiInputList.setBounds(200, 10, getWidth() - 210, 20);
        keyboardComponent.setBounds(10, 40, getWidth() - 20, getHeight() - 50);
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        synthAudioSource.getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override
    {
        synthAudioSource.releaseResources();
    }

private:
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }

    void setMidiInput(int index)
    {
        auto list = juce::MidiInput::getAvailableDevices();

        deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier,
            synthAudioSource.getMidiCollector()); // [12]

        auto newInput = list[index];

        if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback(newInput.identifier, synthAudioSource.getMidiCollector()); // [13]
        midiInputList.setSelectedId(index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }

    //==========================================================================
    juce::MidiKeyboardState keyboardState;
    SynthAudioSource synthAudioSource;
    juce::MidiKeyboardComponent keyboardComponent;

    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthContentComponent)
};


class MainContentComponent2 : public juce::AudioAppComponent
{
public:
    MainContentComponent2()
    {
        setSize(600, 400);

        // Open audio device (0 inputs, 2 outputs)
        setAudioChannels(0, 2);

        // Add button to GUI
        addAndMakeVisible(triggerButton);

        // Make it toggle on/off
        triggerButton.setClickingTogglesState(true);

        // Connect button → atomic parameter
        triggerButton.onClick = [this]()
            {
                for (int i = 0; i < adsr.size(); i++) {
                    adsr[i].noteOn();
                };
            };
    }

    ~MainContentComponent2() override
    {
        shutdownAudio();
    }

    void resized() override
    {
        triggerButton.setBounds(20, 20, 100, 40);
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

        double fund = 146.83;
        double freq = fund;
        double length = sqrt(215 * (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi) / (0.086197 * (fund * fund)));
        double EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;
        double a = (EI * 9.8596) / (2 * 215 * pow(length, 2));
        float attk = 0.001f;
        float dec = 2.0f;

        for (int i = 0; i < oscArray.size(); i++) {
            oscArray[i].prepare(spec);

            oscArray[i].setFrequency(freq);
            adsr[i].setSampleRate(sampleRate);

            // ADSR:
            adsr[i].setParameters(juce::ADSR::Parameters(attk, dec, 0.0f, 0.0f));

            freq = ((i + 1) * fund) * (1 + (a * (pow((i + 1), 2))));
            attk -= 0.0018;
            dec /= 2;


        };


        //lfo.init(sampleRate, 4, 0.0f, 0.25f);  // sine wave, 0.25 Hz

        // Filter
        /*filter.init(sampleRate);
        filter.setFilterType(ZDSVF::LP);  // low-pass
        filter.setQ(0.0f);                // resonance
        filter.setCutoff(freq*1.5f);         // cutoff in MIDI pitch
        filter.setDrive(1.5f);   */         // neutral drive

        filter.prepare(spec);
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq * 1.5);

        //Reverb
        verb.prepare(spec);
        verb.setParameters(juce::dsp::Reverb::Parameters{ 0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.0f });


    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {


        auto* buffer = bufferToFill.buffer;
        int numChannels = buffer->getNumChannels();
        int numSamples = buffer->getNumSamples();


        for (int i = 0; i < numSamples; ++i)
        {
            //float lfoValue;
            //lfo.dsp(lfoValue);

            //float lfoGain = 0.5f * (lfoValue + 1.0f);
           // lfoGain *= 0.2f;

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

                // Apply modulation chain
                //sample *= lfoGain;



                sample = filter.processSample(sample);

                buffer->getWritePointer(ch)[i] = sample;



            }

            juce::dsp::AudioBlock<float> block(*buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            //verb.process(context);

        }
    }

    void releaseResources() override {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.setFont(24.0f);

        g.drawText("jdsplib test",
            getLocalBounds(),
            juce::Justification::centred,
            true);
    }

private:
    std::array<juce::dsp::Oscillator<double>, 6> oscArray;
    //LFO lfo;
    juce::dsp::IIR::Filter<float> filter;
    std::array <juce::ADSR, 6> adsr;
    juce::dsp::Reverb verb;

    std::atomic<float> trigger{ 0.0f };
    juce::TextButton triggerButton{ "Trigger" };
};