// KotoNotation.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "BinaryData.h"
#include "Synth.h"
#include <regex>

class scoreHolder {
public:
	std::string note;
	std::string ornament;
	std::string hand;
	int length;
};


class Bar : public juce::Component {
public:
	int placementModifier;
	//juce::DrawableRectangle rect;
	//std::array<juce::DrawablePath, 12> lines;
	std::array<juce::DrawableText, 68> textHolder;


	Bar(int mod = 0) {

		placementModifier = mod;

		for (int i = 0; i < textHolder.size(); i++) {
			addAndMakeVisible(textHolder[i]);
		}

		/*addAndMakeVisible(rect);


		rect.setFill(juce::Colours::blue);
		rect.setStrokeFill(juce::Colours::black);
		rect.setStrokeThickness(1.3f);
		rect.setRectangle(juce::Rectangle(0.0f, 0.0f, 2000.0f, 4000.0f));*/
	}

	void paint(juce::Graphics& g) override {
		//Set bar size
		float boxW = ((getHeight()) * 0.707) / 10;
		float boxH = getHeight() / 4.5;

		//Set bar location
		float w = getWidth();
		float x = (7 - floor(placementModifier / 4)) * (getWidth() / 10) - (2 * floor(placementModifier / 4));
		float y = getHeight() / 14 + (placementModifier % 4 * (getHeight() / 4.5 + 2));

		//Draw bar
		g.setColour(juce::Colours::black);
		g.drawRect(x, y, boxW, boxH, 0.3f);

		//Draw bar lines
		for (int i = 1; i < 4; i++) {
			g.drawLine(x,
				y + (boxH / 4) * i,
				x + boxW,
				y + (boxH / 4) * i,
				0.3f);
		}

		for (int i = 1; i < 8; i += 2) {
			g.drawLine(x,
				y + (boxH / 8) * i,
				x + boxW / 2,
				y + (boxH / 8) * i,
				0.2f);
		}

	}

	void updateInput(std::vector<scoreHolder> newInfo, std::vector<scoreHolder> newInfoLeft) {
		//Set bar size
		float boxW = ((getHeight()) * 0.707) / 10;

		//Set bar location
		float x = (7 - floor(placementModifier / 4)) * (getWidth() / 10) - (2 * floor(placementModifier / 4));
		float xl = x - (boxW / 9);
		x += (boxW / 3);

		//Clear text
		for (int i = 0; i < textHolder.size(); i++) {
			textHolder[i].setVisible(false);
		}



		//TODO: get left hand displaying properly
		fillBars(newInfo, x, 0);
		fillBars(newInfoLeft, xl, newInfo.size());
	}

	void fillBars(std::vector<scoreHolder> newInfo, int x, int imod) {
		//Set bar size
		float boxW = ((getHeight()) * 0.707) / 10;
		float boxH = getHeight() / 4.5;

		//Set bar location
		float w = getWidth();
		float y = getHeight() / 14 + (placementModifier % 4 * (getHeight() / 4.5 + 2));

		for (int i = 0; i < newInfo.size(); i++) {
			std::vector<std::string> note(newInfo[i].note.size());
			std::ranges::copy(newInfo[i].note, begin(note));
			std::vector<std::string> ornmt(newInfo[i].ornament.size());
			std::ranges::copy(newInfo[i].ornament, begin(ornmt));
			int length = newInfo[i].length;

			//Combine notes and ornaments into one string
			std::vector<std::string> combo;
			for (int j = 0; j < ornmt.size(); j++) {
				if (note.size() > j) {
					combo.push_back(note[j]);
				}
				combo.push_back(ornmt[j]);
			}

			//combo = combo.join("").replaceAll(" ", "")
			std::string combo1 = std::accumulate(combo.begin(), combo.end(), std::string{});
			std::string combo2 = std::regex_replace(combo1, std::regex(" "), "");

			//Only keep one staccato for chords
			if (std::ranges::contains(ornmt, ".")) {
				std::string comboStac = std::regex_replace(combo2, std::regex("\\."), "");
				combo2 = comboStac + ".";
			}

			juce::String comboFinal;

			for (int j = 0; j < combo2.length(); j++) {
				if (combo2.size() > 0) {
					std::vector<std::string> c(combo2.size());
					std::ranges::copy(combo2, begin(c));
					if (std::ranges::contains(noteInputVals, c[j])) {
						int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), c[j]));
						comboFinal.append(notes[n], 1);
					}
					else if (std::ranges::contains(ornInputVals, c[j])) {
						int o = std::distance(ornInputVals.begin(), std::find(ornInputVals.begin(), ornInputVals.end(), c[j]));
						comboFinal.append(ornaments[o], 1);
					}
				}
			}



			//Calculate size and placement modifier
			float mod = (boxH / 8);
			float xmod = boxW / 6;
			int size = boxH / 8;

			if (combo2.length() > 1 || note.size() > 2) {
				size -= combo2.length() * (boxW / 24);
				xmod = boxW / 12;
			}

			//length overrules chord sizing
			if (length > 8) {
				size = (boxH / 8) / (length / 8);
			}

			if (length > 4) {
				mod = 0;
			}

			//Smaller left hand
			if (newInfo[i].hand == "l") {
				size -= 1;
			}


			y += boxH / length;

			//Write note out
			if (i < textHolder.size()) {
				textHolder[i + imod].setFontHeight(size);
				textHolder[i + imod].setBoundingBox(juce::Rectangle(x + xmod, (y - mod - size), boxW, float(size)));
				textHolder[i + imod].setText(comboFinal);
				textHolder[i + imod].setVisible(true);
			}

		}
	}

private:
	//Reference arrays
	std::array<std::string, 16> noteInputVals{ "1", "2", "3", "4", "5", "6", "7", "8",
		"9", "q", "w", "e", "r", "0", "-", "," };
	std::array < juce::String, 16> notes{ u8"一", u8"二", u8"三", u8"四", u8"五", u8"六", u8"七", u8"八",
		u8"九", u8"十", u8"斗", u8"為", u8"巾", u8"⦿", u8"〇", u8"," };
	std::array < juce::String, 5> ornaments{ u8"ｵ", u8"ｦ", u8"ﾋ", u8"ｽ", u8"・" };
	std::array < std::string, 5> ornInputVals{ "o", "p", "h", "s", "." };
};


class scoreComponent : public juce::Component {
public:

	juce::DrawableRectangle sheet;
	std::array<Bar, 28> bars;
	int page = 0;

	scoreComponent() {

		addAndMakeVisible(sheet);
		for (int i = 0; i < bars.max_size(); i++) {
			std::vector<scoreHolder> info;
			bars[i].placementModifier = i;
			addAndMakeVisible(bars[i]);
		}


		sheet.setFill(juce::Colours::white);
	}

	void resized() override {
		float w = getWidth();
		float h = getHeight();
		sheet.setRectangle(juce::Rectangle(0.0f, 0.0f, w, h));



		for (int i = 0; i < bars.size(); i++) {
			bars[i].setBounds(0, 0, w, h);
		}

	}

	void writeBars(std::vector<scoreHolder> outputArray, std::vector<scoreHolder> outputArrayLeft) {
		//Take in notation input and draw the bars with the notes
		std::string text;
		for (int i = 0; i < outputArray.size(); i++) {
			if (outputArray[i].note == ",") {
				text += ",";
			}
			else if (outputArray[i].note != "") {
				text += "0";
			}
		}

		std::string textLeft;
		for (int i = 0; i < outputArrayLeft.size(); i++) {
			if (outputArrayLeft[i].note == ",") {
				textLeft += ",";
			}
			else if (outputArrayLeft[i].note != "") {
				textLeft += "0";
			}
		}

		std::vector<std::string> n;
		std::vector<std::string> nLeft;

		std::regex del(",");
		std::sregex_token_iterator it(text.begin(),
			text.end(), del, -1);
		std::sregex_token_iterator itLeft(textLeft.begin(),
			textLeft.end(), del, -1);
		std::sregex_token_iterator end;
		std::sregex_token_iterator endLeft;

		// Iterating through each token
		while (it != end) {
			n.push_back(*it);
			++it;
		}
		while (itLeft != endLeft) {
			nLeft.push_back(*itLeft);
			++itLeft;
		}
		/*
		   //Refresh pages array
		   let prevPageL = pages.slice(-1)
		   pages = []
		   for (let i = 0; i < (n.length / 28); i++) {
			   pages.push(i)
		   }
	   //Change page if number of pages has changed
	   if (prevPageL[0] != pages.slice(-1)[0]) {
		   let p = pages.slice(-1)
			   page = p[0]
	   }*/

	   //Clear bars

		   //Grab bars based on page, 28 bars per page
		int b1 = 0 + (page * 28);
		int bEnd = 28 + (page * 28);

		if (bEnd > n.size()) {
			bEnd = n.size();
		}

		//Get start position
		//https://stackoverflow.com/questions/48584267/get-the-indexes-of-javascript-array-elements-that-satisfy-condition
		int start = 0;
		int startLeft = 0;
		//let arr = []
		if (page == 0) {
			start = 0;
		}
		/*
		else {
			let fulldat = scoreInput.value().replace(/ \(. + ? \) / g, "0").replace(/ [^ 0 - 9 - , qwer] / g, "")
				for (let i = 0; i < fulldat.length; i++) {
					if (fulldat[i] == ',') arr.push(i)
				}
			start = arr[b1 - 1] + 1
		}*/

		//Set unused bars as invisible
		for (int i = bEnd; i < bars.size(); i++) {
			bars[i].setVisible(false);
		}

		//Display each chunk of notation
		if (n[b1].length() > 0) {
			for (int i = 0; i < bEnd - b1; i++) {
				//get number of notes in bar
				int length = n[b1 + i].length();
				int lengthLeft = 0;

				//Slice notes in given bar from arrays
				std::vector<scoreHolder> arraySlice(length + 1);
				std::vector<scoreHolder> arraySliceLeft;
				std::ranges::copy(outputArray.begin() + start, outputArray.begin() + start + length, begin(arraySlice));

				//Get left hand
				if (nLeft.size() > b1 + i) {
					lengthLeft = nLeft[b1 + i].length();
					arraySliceLeft.resize(lengthLeft + 1);
					std::ranges::copy(outputArrayLeft.begin() + startLeft, outputArrayLeft.begin() + startLeft + lengthLeft, begin(arraySliceLeft));
				}

				//Update bar contents
				bars[i].updateInput(arraySlice, arraySliceLeft);
				bars[i].setVisible(true);
				start += length + 1;
				startLeft += lengthLeft + 1;
			}
		}
	}

	void removeBar() {
	}

private:
};

class MainContentComponent : public juce::AudioAppComponent
{
public:
	MainContentComponent()
	{

		addAndMakeVisible(scoreSheet);
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
		infoBox.setFill(juce::Colours::white);
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

				if (n >= 0 && n < 13) {
					synthArray[n].playNote();
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
		float sheetH = getHeight() - 100;
		float sheetW = sheetH * 0.707;

		if (sheetW > getWidth() - ((getWidth() / 2) + 40)) {
			sheetW = getWidth() - ((getWidth() / 2) + 40) - 40;
			sheetH = sheetW / 0.707;
		}

		float sheetX = (getWidth() / 2) + 20 + ((getWidth() - ((getWidth() / 2) + 40)) / 2) - (sheetW / 2);
		float sheetY = 40;

		scoreSheet.setBounds(sheetX, sheetY, sheetW, sheetH);

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

		//Set off process to update bar contents
		changeScore();

	}

	void changeTitle() { titleOut.setText(titleInput.getText()); }
	void changeAuth()
	{
		juce::String a1 = u8"作曲: ";
		juce::String a2 = authInput.getText();
		authOut.setText(a1 + a2);
	}
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
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);
		}
	}
	void toHira() {
		//Return tuneArray to default hirachōshi tuning
		tuneArray = hiraTuning;

		//Update synths
		for (int i = 0; i < synthArray.size(); i++) {
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);
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
			changeScore();
		}
	}
	void changeNotes() {}
	void changeScore() {
		std::regex del("(?![^(]*?\\))");
		std::string text = scoreInput.getText().toStdString();
		std::sregex_token_iterator it(text.begin(),
			text.end(), del, -1);
		std::sregex_token_iterator end;

		std::vector<std::string> inputArray;
		// Iterating through each token
		while (it != end) {
			inputArray.push_back(*it);
			++it;
		}
		std::vector<scoreHolder> outputArray;
		std::vector<scoreHolder> outputArrayLeft;
		std::vector<std::string> noteArray;
		std::vector<std::string> ornamentArray;
		std::vector<std::string> handArray;
		std::vector<int> lengthArray;

		std::string hand = "r";
		int length = 4;
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

		scoreSheet.writeBars(outputArray, outputArrayLeft);
		updatePlayback(outputArray, outputArrayLeft);
	}

	void updatePlayback(std::vector<scoreHolder>& outputArray, std::vector<scoreHolder>& outputArrayLeft) {
		std::vector<scoreHolder> playbackHolder(outputArray.size());
		std::ranges::copy(outputArray, begin(playbackHolder));

		std::vector<scoreHolder> playbackHolderLeft(outputArrayLeft.size());
		std::ranges::copy(outputArrayLeft, begin(playbackHolderLeft));

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

		playbackHold = playbackHolder;
	}

	void playScore() {
		for (int i = 0; i < synthArray.size(); i++) {
			synthArray[i].fund = tuneArray[i];
			synthArray[i].updateFrequency(sr);
		}

		playbackPlaying = playbackHold;
		playbackPointer = 0;
	}
	void stopScore() {
		std::vector<scoreHolder> emptyScore;
		playbackPlaying = emptyScore;
	}
	void savePDF() {}
	void nextPage() {}
	void prevPage() {}

private:

	const unsigned int tableSize = 1 << 7;
	float level = 0.0f;
	float EI = (2.52 * juce::MathConstants<double>::pi * 0.0000522) / 4;

	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

	//Sheet
	scoreComponent scoreSheet;

	//Title
	juce::TextEditor titleInput;
	juce::DrawableText titleOut;

	//Author
	juce::TextEditor authInput;
	juce::DrawableText authOut;

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

	//Notation input
	juce::TextEditor scoreInput;

	//Play button
	juce::TextButton playButton{ u8"▶" };

	//Stop button
	juce::TextButton stopButton{ u8"■" };

	//PDF button
	juce::TextButton pdfButton{ u8"\U00002B73" };

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
	std::array<kotoSynth, 13>  synthArray;
	std::array<double, 13> tuneArray{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };
	std::array<double, 13> hiraTuning{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };

	std::vector<scoreHolder> playbackHold;
	std::vector<scoreHolder> playbackPlaying;
	double sr;
	double playbackPointer = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
