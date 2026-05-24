// KotoNotation.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "BinaryData.h"
#include "Synth.h"
#include "PDF.h"
#include <regex>



class MainContentComponent : public juce::AudioAppComponent
{
public:
	MainContentComponent()
	{

		addAndMakeVisible(scoreSheet);
		addAndMakeVisible(titleInput);
		addAndMakeVisible(authInput);
		for (int i = 0; i < 13; i++)
		{
			addAndMakeVisible(tuneInput[i]);
		}
		addAndMakeVisible(hiraButton);
		addAndMakeVisible(infoButton);
		addChildComponent(infoBox);
		addChildComponent(infoContent);
		addAndMakeVisible(bpmInput);
		addAndMakeVisible(addKotoButton);
		addAndMakeVisible(scoreInput);
		addChildComponent(scoreInput2);
		addAndMakeVisible(playButton);
		addAndMakeVisible(stopButton);
		addAndMakeVisible(pdfButton);
		addAndMakeVisible(saveButton);
		addAndMakeVisible(loadButton);
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
		addKotoButton.onStateChange = [this] {addKoto(); };
		scoreInput.onTextChange = [this] {
			changeScore(scoreInput.getText().toStdString());
			changeScore(scoreInput2.getText().toStdString(), true);
			};
		scoreInput2.onTextChange = [this] {
			changeScore(scoreInput.getText().toStdString());
			changeScore(scoreInput2.getText().toStdString(), true);
			};
		playButton.onClick = [this] {playScore(); };
		stopButton.onClick = [this] {stopScore(); };
		pdfButton.onClick = [this] {savePDF(); };
		saveButton.onClick = [this] {saveFile(); };
		loadButton.onClick = [this] {loadFile(); };
		pageNextButton.onClick = [this] {nextPage(); };
		pageBackButton.onClick = [this] {prevPage(); };

		//Colours
		titleInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		titleInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

		authInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		authInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

		for (int i = 0; i < 13; i++) {
			tuneInput[i].setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
			tuneInput[i].setColour(juce::TextEditor::textColourId, juce::Colours::black);
		}
		hiraButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		hiraButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		infoButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		infoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
		//infoButton.setToggleable(true);
		infoBox.setFill(juce::Colours::white);
		infoContent.setColour(juce::Colours::black);

		bpmInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		bpmInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

		addKotoButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::white);
		addKotoButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		addKotoButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);

		scoreInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		scoreInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);
		scoreInput.setMultiLine(true);

		scoreInput2.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		scoreInput2.setColour(juce::TextEditor::textColourId, juce::Colours::black);
		scoreInput2.setMultiLine(true);

		playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		stopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		pdfButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pdfButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		pageNextButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pageNextButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		pageBackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pageBackButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);


		//Input setup
		bpmInput.setInputRestrictions(4, "1234567890");
		bpmInput.setText("120");

		for (int i = 0; i < tuneInput.size(); i++) {
			tuneInput[i].setInputRestrictions(7, "1234567890.");
		}


		setSize(900, 600);
		setAudioChannels(0, 2);
	}

	~MainContentComponent() override
	{
		shutdownAudio();
	}

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		sr = sampleRate;
		for (int i = 0; i < synthArray.size(); i++) {
			//Update main synths
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);

			//Update full string press synths
			synthArrayOsu[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu[i].updateFrequency(sr);

			//Update half string press synths
			synthArrayHanOsu[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu[i].updateFrequency(sr);
		}
	}

	void releaseResources() override {}

	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
	{

		for (int i = 0; i < playbackPlaying.size(); i++) {
			if (playbackPlaying[i].note == ",") {
				continue;
			}
			if (playbackPlaying[i].length >= playbackPointer && playbackPlaying[i].length < (playbackPointer + bufferToFill.numSamples)) {
				int n = std::stoi(playbackPlaying[i].note);
				std::string orn = playbackPlaying[i].ornament;



				if (n >= 0 && n < 13) {
					if (std::regex_search(orn, std::regex("o"))) {
						synthArrayOsu[n].playNote();
					}
					else if (std::regex_search(orn, std::regex("p"))) {
						synthArrayHanOsu[n].playNote();
					}
					else {
						synthArray[n].playNote();
					}
				}

			}
		}
		playbackPointer += bufferToFill.numSamples;
	}

	void paint(juce::Graphics& g) override
	{
		//g.fillAll(juce::Colour(250, 190, 120));

		g.setColour(juce::Colours::black);
		g.setFont(24.0f);

		g.setColour(juce::Colour(250, 210, 150));
		g.fillRect((getWidth() / 2) + 20, 20, getWidth() - ((getWidth() / 2) + 40), getHeight() - 60);


		//Labels
		//Title
		g.setColour(juce::Colours::black);
		g.setFont(labelFont.withHeight(getWidth() / 25));
		g.drawSingleLineText("Koto score creator", 20, (getWidth() / 25) + 10);

		//Inputs
		//Song title
		g.setFont(labelFont.withHeight(20));
		g.drawSingleLineText(u8"曲名・Title", 20, titleInput.getY() - 5);

		//Author
		g.drawSingleLineText(u8"作曲・Author",
			authInput.getX() + authInput.getWidth() + 10,
			authInput.getY() + (authInput.getHeight() - 5));

		//Tuning
		g.drawSingleLineText(u8"調子・Tuning (Hz)", 20, tuneInput[1].getY() - 5);

		//BPM
		g.drawSingleLineText(u8"一拍・BPM",
			bpmInput.getX() + bpmInput.getWidth() + 10,
			bpmInput.getY() + (bpmInput.getHeight() - 5));

		//Second koto button
		g.drawSingleLineText(u8"第二箏",
			addKotoButton.getX() + addKotoButton.getWidth(),
			addKotoButton.getY() + (addKotoButton.getHeight() - 5));

		//Notes
		g.drawSingleLineText(u8"その他・Notes", notesInput.getX(), notesInput.getY() - 5);

	}

	void resized() override
	{
		//playNote.setBounds(0, 0, getWidth(), getHeight());
		titleInput.setBounds(20, ((getHeight() / 20) * 3), (getWidth() / 2) - 40, 20);
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
		infoBox.setRectangle(juce::Rectangle<float>((getWidth() / 2) + 20, 20, (getWidth() / 2) - 40, getHeight() - 60));
		infoContent.setBoundingBox(juce::Rectangle<float>((getWidth() / 2) + 40, 40, (getWidth() / 2) - 80, getHeight() - 80));
		infoContent.setFont(labelFont.withHeight(20.0f), true);
		juce::String info1 = u8"\n\n1, 2, 3...    =    一, 二, 三...";
		juce::String info2 = u8"\nq, w, e, r    =    十, 斗, 為, 巾";
		juce::String info3 = u8"\n0 = ⦿・rest";
		juce::String info4 = u8"\n- = 〇・rest";
		juce::String info5 = u8"\no = オ";
		juce::String info6 = u8"\np = ヲ";
		juce::String info7 = u8"\ns = ス";
		juce::String info8 = u8"\nh = ヒ";
		juce::String info9 = u8"\n. = ・";
		juce::String info10 = u8"\n, = 次の小節・new bar";
		juce::String info11 = u8"\n/ = 半音符・halve note length";
		juce::String info12 = u8"\n// = 四分音符・quarter note length";
		juce::String info13 = u8"\n(...) = 和音・chord";
		infoContent.setText("Info" + info1 + info2 + info3 + info4 + info5 + info6 + info7 + info8 + info9 + info10 + info11 + info12 + info13);

		bpmInput.setBounds(20, (getHeight() / 10) * 5, getWidth() / 20, 20);
		addKotoButton.setBounds(bpmInput.getX() + bpmInput.getWidth() + 100, (getHeight() / 10) * 5, 30, 20);
		if (addKotoButton.getToggleState()) {
			scoreInput.setBounds(20, (getHeight() / 10) * 5.5, getWidth() / 4 - 30, (getHeight() - (getHeight() / 10) * 5.5) - 40);
		}
		else {
			scoreInput.setBounds(20, (getHeight() / 10) * 5.5, getWidth() / 2 - 40, (getHeight() - (getHeight() / 10) * 5.5) - 40);
		}
		scoreInput2.setBounds(scoreInput.getX() + getWidth() / 4 - 10, (getHeight() / 10) * 5.5, getWidth() / 4 - 30, (getHeight() - (getHeight() / 10) * 5.5) - 40);
		playButton.setBounds(getWidth() / 2 - 12.5, 20, 25, 25);
		stopButton.setBounds(getWidth() / 2 - 12.5, 50, 25, 25);
		pdfButton.setBounds(getWidth() / 2 - 12.5, scoreInput.getY() + scoreInput.getHeight() - 25, 25, 25);
		saveButton.setBounds(getWidth() / 2 - 12.5, scoreInput.getY() + scoreInput.getHeight() - 55, 25, 25);
		loadButton.setBounds(getWidth() / 2 - 12.5, scoreInput.getY(), 25, 25);
		pageNextButton.setBounds(getWidth() / 2 + ((getWidth() / 4) - 20), getHeight() - 35, 20, 20);
		pageBackButton.setBounds(pageNextButton.getX() + pageNextButton.getWidth(), pageNextButton.getY(), 20, 20);


		//Score
		float sheetH = getHeight() - 100;
		float sheetW = sheetH * 0.707;

		if (sheetW > getWidth() - ((getWidth() / 2) + 40)) {
			sheetW = getWidth() - ((getWidth() / 2) + 40) - 40;
			sheetH = sheetW / 0.707;
		}

		float sheetX = (getWidth() / 2) + 20 + ((getWidth() - ((getWidth() / 2) + 40)) / 2) - (sheetW / 2);
		float sheetY = 40;

		scoreSheet.setBounds(sheetX, sheetY, sheetW, sheetH);



		//Set off process to update bar contents
		changeScore(scoreInput.getText().toStdString());
		changeScore(scoreInput2.getText().toStdString(), true);

	}
	void changeTitle() { scoreSheet.updateTitle(titleInput.getText()); }
	void changeAuth() { scoreSheet.updateAuth(authInput.getText()); }
	void changeTuning() {
		//Update tuneArray
		for (int i = 0; i < tuneInput.size(); i++) {
			if (tuneInput[i].getText().length() > 0) {
				try
				{
					tuneArray[i] = std::stod(tuneInput[i].getText().toStdString());
				}
				catch (const std::exception&)
				{
					continue;
				}

			}
		}

		//Update synths
		for (int i = 0; i < synthArray.size(); i++) {
			//Update main synths
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);

			//Update full string press synths
			synthArrayOsu[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu[i].updateFrequency(sr);

			//Update half string press synths
			synthArrayHanOsu[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu[i].updateFrequency(sr);
		}
	}
	void toHira() {
		//Return tuneArray to default hirachōshi tuning
		tuneArray = hiraTuning;

		//Update synths
		for (int i = 0; i < synthArray.size(); i++) {
			//Update main synths
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);

			//Update full string press synths
			synthArrayOsu[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu[i].updateFrequency(sr);

			//Update half string press synths
			synthArrayHanOsu[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu[i].updateFrequency(sr);
		}
	}
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
	void changeBPM() {
		if (bpmInput.getText().length() > 0) {
			//Update score which creates the playback array
			changeScore(scoreInput.getText().toStdString());
			changeScore(scoreInput2.getText().toStdString(), true);
		}
	}
	void addKoto() {
		if (addKotoButton.getToggleState()) {
			//Add input
			scoreInput.setBounds(20, (getHeight() / 10) * 5.5, getWidth() / 4 - 30, (getHeight() - (getHeight() / 10) * 5.5) - 40);
			scoreInput2.setVisible(true);

			//Refresh score output
			changeScore(scoreInput.getText().toStdString());
			changeScore(scoreInput2.getText().toStdString(), true);
		}
		else {
			//Remove input
			scoreInput.setBounds(20, (getHeight() / 10) * 5.5, getWidth() / 2 - 40, (getHeight() - (getHeight() / 10) * 5.5) - 40);
			scoreInput2.setVisible(false);

			//Refresh score output
			changeScore(scoreInput.getText().toStdString());
			changeScore(scoreInput2.getText().toStdString(), true);
		}
	}
	void changeNotes() {}
	void changeScore(std::string text, bool isSecond = false) {
		//Get score input
		std::regex del("(?![^(]*?\\))");
		std::sregex_token_iterator it(text.begin(),
			text.end(), del, -1);
		std::sregex_token_iterator end;

		std::vector<std::string> inputArray;
		// Iterating through each token
		while (it != end) {
			inputArray.push_back(*it);
			++it;
		}

		//Set up arrays to store information
		std::vector<scoreHolder> outputArray;
		std::vector<scoreHolder> outputArrayLeft;

		std::vector<std::string> noteArray;
		std::vector<std::string> ornamentArray;
		std::vector<std::string> handArray;
		std::vector<int> lengthArray;

		std::string hand = "r";
		int length = 4;

		//Iterate through input to create arrays for visualising and playing the score
		for (int i = 0; i < inputArray.size(); i++) {
			//get note length
			if (inputArray[i] == "/") {
				length *= 2;
				continue;
			};

			//get note hand
			if (inputArray[i] == "l") {
				if (hand == "r") {
					hand = "l";
				}
				else {
					hand = "r";
				}
			};

			//get note or chord
			if (inputArray[i].size() > 1) {
				if (inputArray[i].size() > 0) {
					std::vector<std::string> chord(inputArray[i].size());
					std::ranges::copy(inputArray[i], begin(chord));
					std::vector<std::string> chordArray;
					std::vector<std::string> chordOrnArray;
					for (int j = 0; j < chord.size(); j++) {
						if (std::ranges::contains(noteInputVals, chord[j])) {
							chordArray.push_back(chord[j]);
							if (chord.size() > j + 1) {
								if (std::ranges::contains(ornInputVals, chord[j + 1])) {
									chordOrnArray.push_back(chord[j + 1]);
								}
								else {
									chordOrnArray.push_back(" ");
								}
							}
						}
					}
					std::string n = std::accumulate(chordArray.begin(), chordArray.end(), std::string{});
					std::string o = std::accumulate(chordOrnArray.begin(), chordOrnArray.end(), std::string{});
					noteArray.push_back(n);
					lengthArray.push_back(length);
					ornamentArray.push_back(o);
					handArray.push_back(hand);
					length = 4;
				}

			}

			if (std::ranges::contains(noteInputVals, inputArray[i])) {
				int num = i;
				noteArray.push_back(inputArray[i]);
				handArray.push_back(hand);
				lengthArray.push_back(length);
				length = 4;
				if (inputArray.size() > num + 1) {
					if (std::ranges::contains(ornInputVals, inputArray[num + 1])) {
						ornamentArray.push_back(inputArray[num + 1]);

						//Check for second ornament
						if (inputArray.size() > num + 2) {
							if (std::ranges::contains(ornInputVals, inputArray[num + 2])) {
								std::string o1 = ornamentArray.back();
								ornamentArray.pop_back();
								ornamentArray.push_back(o1 + inputArray[num + 2]);
							}
						}
					}
					else {
						ornamentArray.push_back(" ");
					}
				}
				else {
					ornamentArray.push_back(" ");
				}
			}
		}


		int leftLength = 0;

		for (int i = 0; i < noteArray.size(); i++) {
			if (handArray[i] == "r") {
				outputArray.push_back({ noteArray[i], ornamentArray[i], handArray[i], lengthArray[i] });
				if (leftLength > 0) {
					leftLength -= lengthArray[i];
				}
				else if (noteArray[i] == ",") {
					outputArrayLeft.push_back({ ",", " ", "l", lengthArray[i] });
				}
				else {
					outputArrayLeft.push_back({ " ", " ", "l", lengthArray[i] });
				}

			}
			else {
				outputArrayLeft.push_back({ noteArray[i], ornamentArray[i], handArray[i], lengthArray[i] });
				leftLength += lengthArray[i];
			}
		}

		scoreSheet.writeBars(outputArray, outputArrayLeft, isSecond, addKotoButton.getToggleState());
		updatePlayback(outputArray, outputArrayLeft, isSecond);
	}

	void updatePlayback(std::vector<scoreHolder>& outputArray, std::vector<scoreHolder>& outputArrayLeft, bool isSecond) {
		std::vector<scoreHolder> playbackHolder(outputArray.size());
		std::ranges::copy(outputArray, begin(playbackHolder));

		std::vector<scoreHolder> playbackHolderLeft(outputArrayLeft.size());
		std::ranges::copy(outputArrayLeft, begin(playbackHolderLeft));


		std::vector<scoreHolder> playbackHolderSukui;

		std::string bpmString = bpmInput.getText().toStdString();
		int bpm = std::stoi(bpmString);
		double beat = 60.0f / bpm;

		int timeToPlay = 0;
		//convert notes to synth numbers
		for (int i = 0; i < playbackHolder.size(); i++) {
			if (playbackHolder[i].note == ",") {
				continue;
			}
			int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), playbackHolder[i].note));
			playbackHolder[i].note = std::to_string(n);

			//get length of note in samples
			double t = (beat * sr) * (4.0f / playbackHolder[i].length);

			playbackHolder[i].length = timeToPlay;

			//Check for sukui
			std::string orn = playbackHolder[i].ornament;
			if (std::regex_search(orn, std::regex("s"))) {
				scoreHolder s;
				s.note = playbackHolder[i].note;
				s.ornament = playbackHolder[i].ornament + "d";
				s.length = playbackHolder[i].length + (t / 2);
				playbackHolderSukui.push_back(s);
			}

			//add note length to playtime
			timeToPlay += t;
		}


		timeToPlay = 0;
		//convert notes to synth numbers for left hand
		for (int i = 0; i < playbackHolderLeft.size(); i++) {
			if (playbackHolderLeft[i].note == ",") {
				continue;
			}

			if (std::ranges::contains(noteInputVals, playbackHolderLeft[i].note)) {
				int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), playbackHolderLeft[i].note));
				playbackHolderLeft[i].note = std::to_string(n);
			}
			else {
				playbackHolderLeft[i].note = "14";
			}


			//get length of note in samples
			double t = (beat * sr) * (4.0f / playbackHolderLeft[i].length);

			playbackHolderLeft[i].length = timeToPlay;

			//add note length to playtime
			timeToPlay += t;
		}

		//Append left hand
		playbackHolder.insert(playbackHolder.end(), playbackHolderLeft.begin(), playbackHolderLeft.end());

		//Append sukui
		playbackHolder.insert(playbackHolder.end(), playbackHolderSukui.begin(), playbackHolderSukui.end());

		if (isSecond) {
			playbackHold2 = playbackHolder;
		}
		else {
			playbackHold = playbackHolder;
		}

	}

	void playScore() {
		for (int i = 0; i < synthArray.size(); i++) {
			//Refresh synths
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);


			synthArrayOsu[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu[i].updateFrequency(sr);


			synthArrayHanOsu[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu[i].updateFrequency(sr);


			synthArray2[i].fund = tuneArray[i];
			synthArray2[i].updateFrequency(sr);


			synthArrayOsu2[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu2[i].updateFrequency(sr);


			synthArrayHanOsu2[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu2[i].updateFrequency(sr);
		}

		playbackPlaying = playbackHold;

		//Append second koto
		playbackPlaying.insert(playbackPlaying.end(), playbackHold2.begin(), playbackHold2.end());
		playbackPointer = 0;
	}
	void stopScore() {
		std::vector<scoreHolder> emptyScore;
		playbackPlaying = emptyScore;
	}
	void savePDF() { pdfMaker.savePDF(scoreSheet); }
	void saveFile() {
		//Get contents of inputs
		//Title
		juce::String title = titleInput.getText();

		//Author
		juce::String auth = authInput.getText();

		//Tuning
		std::string tune;
		for (int i = 0; i < tuneArray.size(); i++) {
			tune += std::to_string(tuneArray[i]) + ", ";
		}

		//BPM
		juce::String bpm = bpmInput.getText();

		//Score
		juce::String score = scoreInput.getText();
		juce::String score2 = scoreInput2.getText();

		//Save contents in txt file

		fileSaver.saveFile(title + "\n" + auth + "\n" + juce::String(tune) + "\n" + bpm + "\n" + score + "\n" + score2);
	}
	void loadFile() { 
		fileSaver.loadFile(titleInput, authInput, tuneInput, bpmInput, scoreInput, scoreInput2, addKotoButton);
	}
	void nextPage() { scoreSheet.pageUp(); }
	void prevPage() { scoreSheet.pageDown(); }

private:

	const unsigned int tableSize = 1 << 7;
	float level = 0.0f;
	float EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;

	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

	//Sheet
	scoreComponent scoreSheet;

	//Title
	juce::TextEditor titleInput;

	//Author
	juce::TextEditor authInput;

	//Tuning
	std::array<juce::TextEditor, 13> tuneInput;

	//Set to hirachōshi D
	juce::TextButton hiraButton{ u8"平調子D" };

	//Info
	juce::TextButton infoButton{ "?" };
	juce::DrawableRectangle infoBox;
	juce::DrawableText infoContent;

	//BPM
	juce::TextEditor bpmInput;

	//Notes
	juce::TextEditor notesInput;
	juce::Label notesOut;

	//Second koto button
	juce::ToggleButton addKotoButton;

	//Notation input
	juce::TextEditor scoreInput;
	juce::TextEditor scoreInput2;

	//Load file button
	juce::TextButton loadButton{ u8"\U0001F5CE" };

	//Play button
	juce::TextButton playButton{ u8"▶" };

	//Stop button
	juce::TextButton stopButton{ u8"■" };

	//PDF button
	juce::TextButton pdfButton{ u8"\U0001F5B6" };

	//Save button
	juce::TextButton saveButton{ u8"\U0001F5AB" };

	//Next page button
	juce::TextButton pageNextButton{ u8"◀" };

	//Back page button
	juce::TextButton pageBackButton{ u8"▶" };

	//Reference arrays
	std::array<std::string, 16> noteInputVals{ "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "q", "w", "e", "r", "0", "-", "," };
	std::array < std::string, 5> ornInputVals{ "o", "p", "h", "s", "." };

	//Audio components
	//Three synth arrays to avoid unwanted pitch bend from string presses
	std::array<kotoSynth, 13>  synthArray;
	std::array<kotoSynth, 13>  synthArrayOsu;
	std::array<kotoSynth, 13>  synthArrayHanOsu;

	//Synths for second koto
	std::array<kotoSynth, 13>  synthArray2;
	std::array<kotoSynth, 13>  synthArrayOsu2;
	std::array<kotoSynth, 13>  synthArrayHanOsu2;

	std::array<double, 13> tuneArray{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };
	std::array<double, 13> hiraTuning{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };

	std::vector<scoreHolder> playbackHold;
	std::vector<scoreHolder> playbackHold2;
	std::vector<scoreHolder> playbackPlaying;
	double sr;
	double playbackPointer = 0;

	//Component for saving and loading files
	saver fileSaver;
	pdfCreator pdfMaker;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
