// KotoNotation.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "BinaryData.h"



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

        addAndMakeVisible(titleInput);
        addAndMakeVisible(titleOut);
        addAndMakeVisible(authInput);
        addAndMakeVisible(authOut);
        for (int i = 0; i < 13; i++)
        {
            addAndMakeVisible(tuneInput[i]);
        }
        addAndMakeVisible(hiraButton);
        addAndMakeVisible(infoButton);
        addChildComponent(infoBox);
        addChildComponent(infoContent);
        addAndMakeVisible(bpmInput);
        addAndMakeVisible(notesInput);
        addAndMakeVisible(scoreInput);
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(pdfButton);
        addAndMakeVisible(saveButton);
        addAndMakeVisible(pageNextButton);
        addAndMakeVisible(pageBackButton);

        //Reactivity
        titleInput.onTextChange = [this] {changeTitle(); };
        authInput.onTextChange = [this] {changeAuth(); };
        for (int i = 0; i < 13; i++)
        {
            tuneInput[i].onTextChange = [this] {changeTuning(); };
        }
        hiraButton.onClick = [this] {toHira(); };
        infoButton.onClick = [this] {popInfo(); };
        bpmInput.onTextChange = [this] {changeBPM(); };
        notesInput.onTextChange = [this] {changeNotes(); };
        scoreInput.onTextChange = [this] {changeScore(); };
        playButton.onClick = [this] {playScore(); };
        stopButton.onClick = [this] {stopScore(); };
        pdfButton.onClick = [this] {savePDF(); };
        pageNextButton.onClick = [this] {nextPage(); };
        pageBackButton.onClick = [this] {prevPage(); };

        //Colours
        titleInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        titleInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        titleOut.setColour(juce::Colours::black);

        authInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        authInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        authOut.setColour(juce::Colours::black);

        for (int i = 0; i < 13; i++) {
            tuneInput[i].setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
            tuneInput[i].setColour(juce::TextEditor::textColourId, juce::Colours::black);
        }
        hiraButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        hiraButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        infoButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        infoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        //infoButton.setToggleable(true);
        infoBox.setFill(juce::FillType(juce::Colours::white));
        infoContent.setColour(juce::Colours::black);

        bpmInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        bpmInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

        notesInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        notesInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

        scoreInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
        scoreInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        scoreInput.setMultiLine(true);

        playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        stopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        pdfButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        pdfButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        pageNextButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        pageNextButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        pageBackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
        pageBackButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        
        
        createWavetable();

        setSize(900, 600);
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
        adsr.setParameters(params);

        for (auto i = 0; i < numberOfOscillators; ++i)
        {
            auto* oscillator = new WavetableOscillator(sineTable);

            double freq = 440.0;

            float strLength = sqrt(215 * (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi) / (0.086197 * (freq * freq)));
            float a = (EI * 9.8596) / (2 * 215 * pow(strLength, 2));


            float harmonic = ((i + 2) * freq) * (1 + (a * (pow((i + 2), 2))));

            if (i == 0) {
                harmonic = freq;
            }


            oscillator->setFrequency((float)harmonic, (float)sampleRate);
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
            auto a = adsr.getNextSample();

            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                auto levelSample = oscillator->getNextSample() * a;

                leftBuffer[sample] += levelSample;
                rightBuffer[sample] += levelSample;
            }
        }
    }

    void paint(juce::Graphics& g) override
    {
        //g.fillAll(juce::Colour(250, 190, 120));

        g.setColour(juce::Colours::black);
        g.setFont(24.0f);

        g.setColour(juce::Colour(250, 210, 150));
        g.fillRect((getWidth() / 2) + 20, 20, getWidth() - ((getWidth() / 2) + 40), getHeight() - 60);

        int sheetH = getHeight() - 100;
        int sheetW = sheetH * 0.707;

        if (sheetW > getWidth() - ((getWidth() / 2) + 40)) {
            sheetW = getWidth() - ((getWidth() / 2) + 40) - 40;
            sheetH = sheetW / 0.707;
        }

        int sheetX = (getWidth() / 2) + 20 + ((getWidth() - ((getWidth() / 2) + 40)) / 2) - (sheetW / 2);
        int sheetY = 40;
        
        g.setColour(juce::Colours::white);
        g.fillRect(sheetX, sheetY, sheetW, sheetH);

        //Labels
        //Title
        g.setColour(juce::Colours::black);
        g.setFont(labelFont.withHeight(getWidth() / 25));
        g.drawSingleLineText("Koto score creator", 20, (getWidth() / 25) + 10);
        
        //Inputs
        //Song title
        g.setFont(labelFont.withHeight(20));
        g.drawSingleLineText(juce::CharPointer_UTF8(u8"曲名・Title") , 20, titleInput.getY() - 5);

        //Author
        g.drawSingleLineText(juce::CharPointer_UTF8(u8"作曲・Author"),
            authInput.getX() + authInput.getWidth() + 10,
            authInput.getY() + (authInput.getHeight() - 5));

        //Tuning
        g.drawSingleLineText(juce::CharPointer_UTF8(u8"調子・Tuning (Hz)"), 20, tuneInput[1].getY() - 5);

        //BPM
        g.drawSingleLineText(juce::CharPointer_UTF8(u8"一拍・BPM"),
            bpmInput.getX() + bpmInput.getWidth() + 10,
            bpmInput.getY() + (bpmInput.getHeight() - 5));

        //Notes
        g.drawSingleLineText(juce::CharPointer_UTF8(u8"その他・Notes"), notesInput.getX(), notesInput.getY() - 5);

    }

    void resized() override
    {
        //playNote.setBounds(0, 0, getWidth(), getHeight());
        titleInput.setBounds(20, ((getHeight()/20) * 3), (getWidth()/2) - 40, 20);
        authInput.setBounds(20, ((getHeight() / 20) * 4), (getWidth() / 4) - 40, 20);
        for (int i = 0; i < 13; i++) {
            int y;
            if (i >= 7) {
                y = 30;
            }
            else {
                y = 0;
            }
            tuneInput[i].setBounds((((getWidth() / 2) / 7) - 2.857) * (i % 7) + 20,
                (getHeight() / 10) * 3 + y, ((getWidth() / 2) / 7) - 20, 20);
        }
        hiraButton.setBounds(tuneInput[12].getX() + (((getWidth() / 2) / 7) - 2.857), tuneInput[12].getY(), 60, 20);
        infoButton.setBounds(20, (getHeight() / 10) * 4.5, 20, 20);
        infoBox.setRectangle(juce::Rectangle<float>((getWidth()/2) + 20, 20, (getWidth()/2) - 40, getHeight() - 60));
        infoContent.setBoundingBox(juce::Rectangle<float>((getWidth() / 2) + 40, 40, (getWidth() / 2) - 80, getHeight() - 80));
        infoContent.setFont(labelFont.withHeight(20.0f), true);
        juce::String info1 = juce::CharPointer_UTF8(u8"\n\n1, 2, 3...    =    一, 二, 三...");
        juce::String info2 = juce::CharPointer_UTF8(u8"\nq, w, e, r    =    十, 斗, 為, 巾");
        juce::String info3 = juce::CharPointer_UTF8(u8"\n0 = ⦿・rest");
        juce::String info4 = juce::CharPointer_UTF8(u8"\n- = 〇・rest");
        juce::String info5 = juce::CharPointer_UTF8(u8"\no = オ");
        juce::String info6 = juce::CharPointer_UTF8(u8"\np = ヲ");
        juce::String info7 = juce::CharPointer_UTF8(u8"\ns = ス");
        juce::String info8 = juce::CharPointer_UTF8(u8"\nh = ヒ");
        juce::String info9 = juce::CharPointer_UTF8(u8"\n. = ・");
        juce::String info10 = juce::CharPointer_UTF8(u8"\n, = 次の小節・new bar");
        juce::String info11 = juce::CharPointer_UTF8(u8"\n/ = 半音符・halve note length");
        juce::String info12 = juce::CharPointer_UTF8(u8"\n// = 四分音符・quarter note length");
        juce::String info13 = juce::CharPointer_UTF8(u8"\n(...) = 和音・chord");
        infoContent.setText("Info" + info1 + info2 + info3 + info4 + info5 + info6 + info7 + info8 + info9 + info10 + info11 + info12 + info13);

        bpmInput.setBounds(20, (getHeight() / 10) * 5, getWidth() / 20, 20);
        notesInput.setBounds(bpmInput.getX() + bpmInput.getWidth() + 100, (getHeight() / 10) * 5,
            getWidth() / 2 - bpmInput.getWidth() - 140, 20);
        scoreInput.setBounds(20, (getHeight() / 10) * 5.5, getWidth() / 2 - 40, (getHeight() - (getHeight() / 10) * 5.5) - 40);
        playButton.setBounds(getWidth() / 2 - 12.5, 20, 25, 25);
        stopButton.setBounds(getWidth() / 2 - 12.5, 50, 25, 25);
        pdfButton.setBounds(getWidth() / 2 - 12.5, scoreInput.getY() + scoreInput.getHeight() - 25, 25, 25);
        saveButton.setBounds(getWidth() / 2 - 12.5, scoreInput.getY() + scoreInput.getHeight() - 55, 25, 25);
        pageNextButton.setBounds(getWidth() / 2 + ((getWidth() / 4) - 20), getHeight() - 35, 20, 20);
        pageBackButton.setBounds(pageNextButton.getX() + pageNextButton.getWidth(), pageNextButton.getY(), 20, 20);


        //Score
        int sheetH = getHeight() - 100;
        int sheetW = sheetH * 0.707;

        if (sheetW > getWidth() - ((getWidth() / 2) + 40)) {
            sheetW = getWidth() - ((getWidth() / 2) + 40) - 40;
            sheetH = sheetW / 0.707;
        }

        int sheetX = (getWidth() / 2) + 20 + ((getWidth() - ((getWidth() / 2) + 40)) / 2) - (sheetW / 2);
        int sheetY = 40;


        //For Japanese text
        //titleOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(sheetX + sheetW - (sheetW / 8), sheetY + 20));
        //need to add a converter to add breaks between characters
        //titleOut.setBoundingBox(juce::Rectangle<float>(20, sheetH - 80));

        titleOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(sheetX + sheetW - (sheetW / 8), sheetY + 20));
        titleOut.setBoundingBox(juce::Rectangle<float>(sheetH - 80, 20));
        titleOut.setJustification(juce::Justification::centred);
        titleOut.setFont(labelFont.withHeight(20.0f), true);

        authOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(sheetX + sheetW - (sheetW / 20), sheetY + 40));
        authOut.setBoundingBox(juce::Rectangle<float>(sheetH - 80, 20));
        //authOut.setJustification(juce::Justification::centred);
        authOut.setFont(labelFont.withHeight(20.0f), true);

    }

    void changeTitle() { titleOut.setText(titleInput.getText()); }
    void changeAuth() 
    { 
        juce::String a1 = juce::CharPointer_UTF8(u8"作曲: ");
        juce::String a2 = authInput.getText();
        authOut.setText(a1+a2) ; 
    }
    void changeTuning() {}
    void toHira() {}
    void popInfo() 
    { 
        if (infoBox.isVisible()) {
            infoBox.setVisible(false);
            infoContent.setVisible(false);
        }
        else {
            infoBox.setVisible(true);
            infoContent.setVisible(true);
        }
    }
    void changeBPM() {}
    void changeNotes() {}
    void changeScore() {}
    void playScore() {}
    void stopScore() {}
    void savePDF() {}
    void nextPage() {}
    void prevPage() {}

private:

    const unsigned int tableSize = 1 << 7;
    float level = 0.0f;
    float EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;

    juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

    //Title
    juce::TextEditor titleInput;
    juce::DrawableText titleOut;

    //Author
    juce::TextEditor authInput;
    juce::DrawableText authOut;

    //Tuning
    std::array<juce::TextEditor, 13> tuneInput;

    //Set to hirachōshi D
    juce::TextButton hiraButton{ juce::CharPointer_UTF8(u8"平調子D") };

    //Info
    juce::TextButton infoButton{ "?" };
    juce::DrawableRectangle infoBox;
    juce::DrawableText infoContent;

    //BPM
    juce::TextEditor bpmInput{ "120" };

    //Notes
    juce::TextEditor notesInput;
    juce::Label notesOut;

    //Notation input
    juce::TextEditor scoreInput;

    //Play button
    juce::TextButton playButton { juce::CharPointer_UTF8(u8"▶") };

    //Stop button
    juce::TextButton stopButton{ juce::CharPointer_UTF8(u8"■") };

    //PDF button
    juce::TextButton pdfButton{ juce::CharPointer_UTF8(u8"\U00002B73")};

    //Save button
    juce::TextButton saveButton{ juce::CharPointer_UTF8(u8"\U0001F5AB") };

    //Next page button
    juce::TextButton pageNextButton{ juce::CharPointer_UTF8(u8"◀") };

    //Back page button
    juce::TextButton pageBackButton{ juce::CharPointer_UTF8(u8"▶") };

    juce::ADSR adsr;
    juce::ADSR::Parameters params{ 0.001f, 0.1f, 0.0f, 1.0f };

    juce::AudioSampleBuffer sineTable;
    juce::OwnedArray<WavetableOscillator> oscillators;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
