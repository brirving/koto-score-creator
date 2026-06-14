// KotoNotation.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include "Synth.h"
#include "PDF.h"
#include "HelpBox.h"
#include "BinaryData.h"


class scoreHolder {
public:
	std::string note;
	std::string ornament;
	std::string hand;
	int length;
	bool isSecond = false;
};

class vertLabel : public juce::Label {
public:
	vertLabel() {};

	void copyColourIfSpecified(Label& l, juce::TextEditor& ed, int colourID, int targetColourID)
	{
		if (l.isColourSpecified(colourID) || l.getLookAndFeel().isColourSpecified(colourID))
			ed.setColour(targetColourID, l.findColour(colourID));
	}

	juce::TextEditor* createEditorComponent() override
	{
		auto* ed = new juce::TextEditor(getName());
		ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
		copyAllExplicitColoursTo(*ed);

		copyColourIfSpecified(*this, *ed, textWhenEditingColourId, juce::TextEditor::textColourId);
		copyColourIfSpecified(*this, *ed, backgroundWhenEditingColourId, juce::TextEditor::backgroundColourId);
		copyColourIfSpecified(*this, *ed, outlineWhenEditingColourId, juce::TextEditor::focusedOutlineColourId);

		ed->setMultiLine(true);

		return ed;
	}
};

class DragButton : public juce::TextButton,
	public juce::ComponentDragger {
public:
	DragButton(juce::String text) :TextButton(text) { ; }

	void mouseDown(const juce::MouseEvent& event) override {
		startDraggingComponent(getParentComponent(), event);
	}

	void mouseDrag(const juce::MouseEvent& event) override {
		dragComponent(getParentComponent(), event, nullptr);
	}

	void mouseUp(const juce::MouseEvent& event) override {
		triggerClick();
	}
};

class freeText : public juce::Component {
public:
	//Visibility
	int page = 0;
	bool deleted = false;

	//Text and state
	vertLabel label;
	bool rotated = false;
	bool vertical = false;

	//Coordinates
	juce::Point<int> xyPoint;

	float ogWidth;
	float ogHeight;

	freeText(juce::Point<int> xy, int p, int w, int h) {
		xyPoint = xy;
		page = p;
		ogWidth = w;
		ogHeight = h;

		addAndMakeVisible(label);
		addChildComponent(moveButton);
		addChildComponent(rotateButton);
		addChildComponent(verticalButton);
		addChildComponent(deleteButton);

		label.setText("Click to change text", juce::NotificationType::dontSendNotification);
		label.setEditable(true);

		label.onEditorShow = [this] {
			label.getCurrentTextEditor()->setMultiLine(true);
			showButtons(); };
		moveButton.onClick = [this] { dragText(); };
		rotateButton.onClick = [this] { rotateText(); };
		verticalButton.onClick = [this] { setVertical(); };
		deleteButton.onClick = [this] { setVisible(false); deleted = true; };
		label.onEditorHide = [this] { updateVertical(); hideButtons(); };


		label.setColour(juce::Label::backgroundColourId, juce::Colour(0.0f, 0.0f, 0.0f, 0.0f));
		label.setColour(juce::Label::textColourId, juce::Colours::black);
		label.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);


		moveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		moveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		moveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);


		rotateButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		rotateButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		rotateButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		verticalButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		verticalButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		verticalButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		deleteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);


	}

	~freeText() {}

	void resized() override {
		label.setBounds(0, 25, getWidth(), 20);
		label.setFont(labelFont.withHeight(ogHeight / 50.0f));

		moveButton.setBounds(0, 0, 20, 20);
		rotateButton.setBounds(30, 0, 20, 20);
		verticalButton.setBounds(60, 0, 30, 20);
		deleteButton.setBounds(120, 0, 20, 20);
	}

	void showButtons() {
		moveButton.setVisible(true);
		rotateButton.setVisible(true);
		verticalButton.setVisible(true);
		deleteButton.setVisible(true);
	}

	void hideButtons() {
		if (!(moveButton.isMouseOver() || rotateButton.isMouseOver() || verticalButton.isMouseOver() || deleteButton.isMouseOver())) {
			moveButton.setVisible(false);
			rotateButton.setVisible(false);
			verticalButton.setVisible(false);
			deleteButton.setVisible(false);
		}
	}

	void rotateText() {
		if (rotated) {
			//Rotate horizontal
			setTransform(juce::AffineTransform::rotation(0));

			rotated = false;
		}
		else {
			//Rotate vertical
			setTransform(juce::AffineTransform::fromTargetPoints(xyPoint.getX(), xyPoint.getY(), xyPoint.getX() + getHeight(), xyPoint.getY(),
				xyPoint.getX() + getWidth(), xyPoint.getY(), xyPoint.getX() + getHeight(), xyPoint.getY() + getWidth(),
				xyPoint.getX(), xyPoint.getY() + getHeight(), xyPoint.getX(), xyPoint.getY()));

			rotated = true;
		}

	}

	void dragText() {

		//Update location
		xyPoint = getBoundsInParent().getTopLeft();

	}

	void setVertical() {

		//Rotate label
		if (vertical) {
			//Make wide
			label.setTransform(juce::AffineTransform::rotation(0));
			label.setBounds(juce::Rectangle<int>(0, 25, getWidth(), 20));
			label.setJustificationType(juce::Justification::bottomLeft);

			label.setColour(juce::Label::backgroundColourId, juce::Colour(0.0f, 0.0f, 0.0f, 0.0f));
			label.setColour(juce::Label::textColourId, juce::Colours::black);
			label.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);

			//Remove line breaks
			juce::String output = label.getText().replace("\n", "");

			label.setText(output, juce::NotificationType::dontSendNotification);

			vertical = false;
		}
		else {
			//Make tall 
			label.setTransform(juce::AffineTransform::fromTargetPoints(label.getX(), label.getY(), label.getX(), label.getY() + label.getHeight(),
				label.getX() + (float)label.getWidth(), label.getY(), label.getX(), label.getY() - label.getWidth(),
				label.getX(), label.getY() + (float)label.getHeight(), label.getX() + label.getHeight(), label.getY() + label.getHeight()));
			label.setBounds(0, 25, 20, getWidth());
			label.setJustificationType(juce::Justification::topLeft);

			label.setColour(juce::Label::backgroundColourId, juce::Colour(0.0f, 0.0f, 0.0f, 0.0f));
			label.setColour(juce::Label::textColourId, juce::Colours::black);
			label.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);

			//add line breaks
			juce::String output;
			auto start = label.getText().begin();
			auto end = label.getText().end();

			// Iterating through each token
			while (start != end) {
				output += start.getAndAdvance();
				output += "\n";
			}
			output += "\0";
			label.setText(output, juce::NotificationType::dontSendNotification);

			vertical = true;
		}

	}

	void updateVertical() {
		if (vertical) {
			//Remove line breaks
			juce::String input = label.getText().replace("\n", "");

			//Re-add line breaks to new text
			juce::String output;
			auto start = input.begin();
			auto end = input.end();

			// Iterating through each token
			while (start != end) {
				output += start.getAndAdvance();
				output += "\n";
			}
			label.setText(output, juce::NotificationType::dontSendNotification);
		}
	}

	void mouseExit(const juce::MouseEvent& event) override {
		hideButtons();
	}

private:
	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };


	//Buttons
	DragButton moveButton{ u8"\u2725" };
	juce::TextButton rotateButton{ u8"\u2b6f" };
	juce::TextButton verticalButton{ u8"A\u2193" };
	juce::TextButton deleteButton{ "X" };

};


class Bar : public juce::Component {
public:
	int placementModifier;
	std::array<juce::DrawableText, 68> textHolder;
	juce::DrawablePath boldLeft;

	Bar(int mod = 0) {

		placementModifier = mod;

		for (int i = 0; i < textHolder.size(); i++) {
			addAndMakeVisible(textHolder[i]);
			textHolder[i].setFont(labelFont, true);
		}

		//Bold left side for demarking two instruments
		addChildComponent(boldLeft);
		boldLeft.setStrokeFill(juce::Colours::black);
		boldLeft.setStrokeThickness(1.2f);
		setInterceptsMouseClicks(false, false);

	}

	void resized() override {
		//Set bar size
		float boxH = getHeight() / 4.5;

		//Set bar location
		float x = (7 - floor(placementModifier / 4)) * (getWidth() / 10) - (2 * floor(placementModifier / 4));
		float y = getHeight() / 14 + (placementModifier % 4 * (getHeight() / 4.5 + 2));

		juce::Path p;
		p.startNewSubPath(x, y);
		p.lineTo(x, y + boxH);
		boldLeft.setPath(p);
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

	void updateInput(std::vector<scoreHolder> newInfo, std::vector<scoreHolder> newInfoLeft, bool isSecond) {
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

		if (isSecond) {
			boldLeft.setVisible(true);
		}
		else {
			boldLeft.setVisible(false);
		}


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
				//write left side ornaments
				if (ornmt[j] == "o" || ornmt[j] == "p" || ornmt[j] == "." || ornmt[j] == "h") {
					combo.push_back(ornmt[j]);
				}

				//Write notes
				if (note.size() > j) {
					combo.push_back(note[j]);
				}

				//Write lower ornaments
				if (ornmt[j] == "s") {
					combo.push_back("\n" + ornmt[j]);
				}
			}

			//combo = combo.join("").replaceAll(" ", "")
			std::string combo1 = std::accumulate(combo.begin(), combo.end(), std::string{});
			std::string combo2 = std::regex_replace(combo1, std::regex(" "), "");

			//Only keep one staccato for chords
			if (std::ranges::contains(ornmt, ".")) {
				std::string comboStac = std::regex_replace(combo2, std::regex("\\."), "");
				combo2 = "." + comboStac;
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
					else if (c[j] == "\n") {
						comboFinal.append("\n", 2);
					}
				}
			}



			//Calculate size and placement modifier
			float mod = (boxH / 8);
			float xmod = boxW / 6;
			int size = boxH / 7;

			if (note.size() > 1) {
				size -= note.size() * (boxW / 16);
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

			float h = size;

			//Shift left and change text height with staccato
			if (std::ranges::contains(ornmt, "s")) {
				if (length > 4) {
					size /= 2;
					mod = size;
				}
				else {
					h *= 2.0f;
				}

			}


			y += boxH / length;


			//Write note out
			if (i < textHolder.size()) {
				textHolder[i + imod].setFontHeight(size);
				textHolder[i + imod].setBoundingBox(juce::Rectangle(x + xmod, (y - mod - size), boxW, h));
				textHolder[i + imod].setText(comboFinal);
				textHolder[i + imod].setVisible(true);
			}

		}
	}

private:
	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

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
	int maxPage = 0;
	int maxPage1 = 0;
	int maxPage2 = 0;

	//Holders for inputs
	std::vector<scoreHolder> contents;
	std::vector<scoreHolder> contentsLeft;

	std::vector<scoreHolder> contents2;
	std::vector<scoreHolder> contentsLeft2;

	//Is there a second instrument
	bool hasSecondKoto;

	//Free text holder
	std::vector<std::unique_ptr<freeText>> freeTextHolder;

	scoreComponent() {

		addAndMakeVisible(sheet);
		for (int i = 0; i < bars.max_size(); i++) {
			std::vector<scoreHolder> info;
			bars[i].placementModifier = i;
			addAndMakeVisible(bars[i]);
		}

		addAndMakeVisible(titleOut);
		addAndMakeVisible(authOut);

		//Colours
		sheet.setFill(juce::Colours::white);

		titleOut.setColour(juce::Colours::black);
		authOut.setColour(juce::Colours::black);
	}

	void resized() override {
		float w = getWidth();
		float h = getHeight();
		sheet.setRectangle(juce::Rectangle(0.0f, 0.0f, w, h));



		for (int i = 0; i < bars.size(); i++) {
			bars[i].setBounds(0, 0, w, h);
		}

		for (int i = 0; i < freeTextHolder.size(); i++) {

			float w = static_cast<float>(getWidth()) / freeTextHolder[i]->ogWidth;


			juce::Point xyPlain = freeTextHolder[i]->xyPoint;


			auto xyP = xyPlain * w;



			if (freeTextHolder[i]->rotated) {
				freeTextHolder[i]->setTransform(juce::AffineTransform::fromTargetPoints(xyP.getX(), xyP.getY(), xyP.getX() + freeTextHolder[i]->getHeight(), xyP.getY(),
					xyP.getX() + freeTextHolder[i]->getWidth(), xyP.getY(), xyP.getX() + freeTextHolder[i]->getHeight(), xyP.getY() + freeTextHolder[i]->getWidth(),
					xyP.getX(), xyP.getY() + freeTextHolder[i]->getHeight(), xyP.getX(), xyP.getY()));
			}

			freeTextHolder[i]->setBounds(xyP.getX(), xyP.getY(), freeTextHolder[i]->getWidth(), freeTextHolder[i]->getHeight());


			//Pass in new position
			freeTextHolder[i]->ogWidth = getWidth();
			freeTextHolder[i]->ogHeight = getHeight();
			freeTextHolder[i]->xyPoint = xyP;


			freeTextHolder[i]->label.setFont(labelFont.withHeight(getHeight() / 50));

		}

		//Maintain vertical or horizontal text direction on resize
		if (titleToggleState) {
			titleOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(getWidth() - (getWidth() / 8) - 20, 20));
			titleOut.setBoundingBox(juce::Rectangle<float>(20, getHeight() - 80));
		}
		else {
			titleOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 8), 20));
			titleOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
			titleOut.setJustification(juce::Justification::centred);
			titleOut.setFont(labelFont.withHeight(20.0f), true);
		}

		if (authToggleState) {
			authOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(getWidth() - (getWidth() / 20) - 20, 40));
			authOut.setBoundingBox(juce::Rectangle<float>(20, getHeight() - 80));
		}
		else {
			authOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 20), 40));
			authOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
			authOut.setFont(labelFont.withHeight(20.0f), true);
		}



	}

	void writeBars(std::vector<scoreHolder> outputArray, std::vector<scoreHolder> outputArrayLeft, bool isSecond, bool secondActive) {
		if (isSecond) {
			contents2 = outputArray;
			contentsLeft2 = outputArrayLeft;
			hasSecondKoto = secondActive;
		}
		else {
			contents = outputArray;
			contentsLeft = outputArrayLeft;
			hasSecondKoto = secondActive;
		}

		//Set number of bars for each koto per page
		int barsPerPage = 28;
		if (secondActive) {
			barsPerPage = 12;
		}

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

		//Refresh number of pages
		int prev1 = maxPage1;
		int prev2 = maxPage2;

		if (isSecond) {
			maxPage2 = floor((n.size() - 1) / barsPerPage);
		}
		else {
			maxPage1 = floor((n.size() - 1) / barsPerPage);
		}

		if (maxPage2 > maxPage1) {
			maxPage = maxPage2;
		}
		else {
			maxPage = maxPage1;
		}

		//Change page if number of pages has changed and user was on the last page
		if (isSecond && prev2 != maxPage2 && page == prev2) {
			page = maxPage2;

			//Rewrite free text
			for (int i = 0; i < freeTextHolder.size(); i++) {
				if (freeTextHolder[i]->page == page && freeTextHolder[i]->deleted == false) {
					freeTextHolder[i]->setVisible(true);
				}
				else {
					freeTextHolder[i]->setVisible(false);
				}
			}
		}
		else if (isSecond == false && prev1 != maxPage1 && page == prev1) {
			page = maxPage1;

			//Rewrite free text
			for (int i = 0; i < freeTextHolder.size(); i++) {
				if (freeTextHolder[i]->page == page && freeTextHolder[i]->deleted == false) {
					freeTextHolder[i]->setVisible(true);
				}
				else {
					freeTextHolder[i]->setVisible(false);
				}
			}
		}

		//Hide title and author if page isn't 0
		if (page != 0) {
			authOut.setVisible(false);
			titleOut.setVisible(false);
		}
		else {
			authOut.setVisible(true);
			titleOut.setVisible(true);
		}


		//Grab bars based on page and number of bars per page
		int b1 = 0 + (page * barsPerPage);
		int bEnd = barsPerPage + (page * barsPerPage);

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
			startLeft = 0;
		}
		else {
			for (int i = 0; i < b1; i++) {
				if (i < n.size()) {
					start += n[i].length() + 1;
					startLeft += nLeft[i].length() + 1;
				}

			}
		}

		//Set unused bars as invisible
		int bMax = bEnd - b1;
		if (bMax < 0) {
			bMax = 0;
		}
		for (int i = bMax; i < bars.size(); i++) {
			if (secondActive) {
				int bNum = i;
				if ((isSecond && i < 4) || (isSecond == false && i >= 4 && i < 8)) {
					bNum += 4;
				}
				else if ((isSecond && i >= 4 && i < 8) || (isSecond == false && i >= 8)) {
					bNum += 8;
				}
				else if (isSecond && i >= 8) {
					bNum += 12;
				}
				if (bNum < bars.size()) {
					bars[bNum].setVisible(false);
				}
			}
			else if (secondActive == false && isSecond == false) {
				bars[i].setVisible(false);
			}

		}

		//Display each chunk of notation
		if (n.size() > b1 && n[b1].length() > 0) {
			for (int i = 0; i < bEnd - b1; i++) {
				if (isSecond && secondActive == false) {
					//Skip the loop if this is the second koto and it's not active
					break;
				}
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
				int bNum = i;
				if (secondActive) {
					if ((isSecond && i < 4) || (isSecond == false && i >= 4 && i < 8)) {
						bNum += 4;
					}
					else if ((isSecond && i >= 4 && i < 8) || (isSecond == false && i >= 8)) {
						bNum += 8;
					}
					else if (isSecond && i >= 8) {
						bNum += 12;
					}
				}

				bars[bNum].updateInput(arraySlice, arraySliceLeft, isSecond);
				bars[bNum].setVisible(true);
				start += length + 1;
				startLeft += lengthLeft + 1;
			}
		}
	}

	void pageUp() {
		if (page < maxPage) {
			page++;
			writeBars(contents, contentsLeft, false, hasSecondKoto);
			writeBars(contents2, contentsLeft2, true, hasSecondKoto);

			for (int i = 0; i < freeTextHolder.size(); i++) {
				if (freeTextHolder[i]->page == page && freeTextHolder[i]->deleted == false) {
					freeTextHolder[i]->setVisible(true);
				}
				else {
					freeTextHolder[i]->setVisible(false);
				}
			}
		}
	}

	void pageDown() {
		if (page > 0) {
			page--;
			writeBars(contents, contentsLeft, false, hasSecondKoto);
			writeBars(contents2, contentsLeft2, true, hasSecondKoto);

			for (int i = 0; i < freeTextHolder.size(); i++) {
				if (freeTextHolder[i]->page == page && freeTextHolder[i]->deleted == false) {
					freeTextHolder[i]->setVisible(true);
				}
				else {
					freeTextHolder[i]->setVisible(false);
				}
			}
		}
	}

	void updateTitle(juce::String title) {
		if (titleToggleState) {
			//Remove line breaks
			juce::String input = title.replace("\n", "");

			//Re-add line breaks to new text
			juce::String output;
			auto start = input.begin();
			auto end = input.end();

			// Iterating through each token
			while (start != end) {
				output += start.getAndAdvance();
				output += "\n";
			}
			titleOut.setText(output);
		}
		else {
			titleOut.setText(title);
		}

	}
	void updateAuth(juce::String auth) {
		if (authToggleState) {
			//Remove line breaks
			juce::String input = auth.replace("\n", "").replace("作曲..", "");

			//Re-add line breaks to new text
			juce::String output = u8"作\n曲\n..\n";
			auto start = input.begin();
			auto end = input.end();

			// Iterating through each token
			while (start != end) {
				output += start.getAndAdvance();
				output += "\n";
			}
			authOut.setText(output);
		}
		else {
			juce::String a1 = u8"作曲: ";
			juce::String a2 = auth;
			authOut.setText(a1 + a2);
		}
	}

	void toggleCursor(bool toggleState) {
		//Update state
		freeTextActive = toggleState;
		//Change cursor to I beam when over the scoresheet if free text is enabled
		if (toggleState) {
			setMouseCursor(juce::MouseCursor::StandardCursorType(4));
		}
		else {
			setMouseCursor(juce::MouseCursor::StandardCursorType(0));
		}
	}

	void mouseDown(const juce::MouseEvent& event) override {
		if (event.mouseWasClicked() && freeTextActive) {
			auto pointClicked = event.getMouseDownPosition();
			addFreeText(pointClicked);
		}
	}

	void addFreeText(juce::Point<int> xy) {
		//Create new freeText component
		freeTextHolder.emplace_back(new freeText(xy, page, getWidth(), getHeight()));

		//Set the location
		freeTextHolder.back()->setBounds(xy.getX(), xy.getY(), getWidth(), 50);

		//Add as child component of the score
		addAndMakeVisible(*freeTextHolder.back());
	}

	void mouseExit(const juce::MouseEvent& event) override {
		//Close free text buttons if any are still open
		for (int i = 0; i < freeTextHolder.size(); i++) {
			freeTextHolder[i]->hideButtons();
		}
	}

	void setVertical(std::string value, bool vertToggleState) {
		if (vertToggleState) {
			//Set long
			if (value == "title") {
				titleOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(getWidth() - (getWidth() / 8) - 20, 20));
				titleOut.setBoundingBox(juce::Rectangle<float>(20, getHeight() - 80));

				//add line breaks
				juce::String output;
				auto start = titleOut.getText().begin();
				auto end = titleOut.getText().end();

				// Iterating through each token
				while (start != end) {
					output += start.getAndAdvance();
					output += "\n";
				}
				output += "\0";
				titleOut.setText(output);

				titleToggleState = vertToggleState;
			}
			else {
				authOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(getWidth() - (getWidth() / 20) - 20, 40));
				authOut.setJustification(juce::Justification::topLeft);
				authOut.setBoundingBox(juce::Rectangle<float>(20, getHeight() - 80));

				//add line breaks
				juce::String output;
				auto start = authOut.getText().begin();
				auto end = authOut.getText().end();

				// Iterating through each token
				while (start != end) {
					output += start.getAndAdvance();
					output += "\n";
				}
				output += "\0";
				authOut.setText(output.replace(":\n ", ".."));

				authToggleState = vertToggleState;
			}


		}
		else {
			//Set wide
			if (value == "title") {
				titleOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 8), 20));
				titleOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
				titleOut.setJustification(juce::Justification::centred);
				titleOut.setFont(labelFont.withHeight(20.0f), true);

				//Remove line breaks
				juce::String output = titleOut.getText().replace("\n", "");
				titleOut.setText(output);

				titleToggleState = vertToggleState;
			}
			else {
				authOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 20), 40));
				authOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
				authOut.setJustification(juce::Justification::topLeft);
				authOut.setFont(labelFont.withHeight(20.0f), true);

				//Remove line breaks
				juce::String output = authOut.getText().replace("\n", "");
				authOut.setText(output.replace("..", ": "));

				authToggleState = vertToggleState;
			}
		}
	}

private:
	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

	//Author and title
	juce::DrawableText titleOut;
	juce::DrawableText authOut;

	//Is free text enabled
	bool freeTextActive = false;

	//Direction of author and title text
	bool titleToggleState = false;
	bool authToggleState = false;
};


class MainContentComponent : public juce::AudioAppComponent
{
public:
	MainContentComponent()
	{

		addAndMakeVisible(scoreSheet);
		addAndMakeVisible(titleInput);
		addAndMakeVisible(titleVertButton);
		addAndMakeVisible(authInput);
		addAndMakeVisible(authVertButton);
		addAndMakeVisible(tuneKoto2);
		for (int i = 0; i < 13; i++)
		{
			addAndMakeVisible(tuneInput[i]);
		}
		for (int i = 0; i < 13; i++)
		{
			addChildComponent(tuneInput2[i]);
		}
		addAndMakeVisible(tuneDropDown);
		addAndMakeVisible(infoButton);
		addAndMakeVisible(bpmInput);
		addAndMakeVisible(addKotoButton);
		addAndMakeVisible(scoreInput);
		addChildComponent(scoreInput2);
		addAndMakeVisible(playButton);
		addAndMakeVisible(stopButton);
		addAndMakeVisible(freeTextButton);
		addAndMakeVisible(pdfButton);
		addAndMakeVisible(saveButton);
		addAndMakeVisible(loadButton);
		addAndMakeVisible(pageNextButton);
		addAndMakeVisible(pageBackButton);

		//Reactivity
		titleInput.onTextChange = [this] {changeTitle(); };
		titleVertButton.onClick = [this] {scoreSheet.setVertical("title", titleVertButton.getToggleState()); };
		authInput.onTextChange = [this] {changeAuth(); };
		authVertButton.onClick = [this] {scoreSheet.setVertical("auth", authVertButton.getToggleState()); };
		tuneKoto2.onStateChange = [this] {showTuning2(); };
		for (int i = 0; i < 13; i++)
		{
			tuneInput[i].onTextChange = [this] {changeTuning(tuneInput, tuneArray, synthArray, synthArrayOsu, synthArrayHanOsu); };
		}
		for (int i = 0; i < 13; i++)
		{
			tuneInput2[i].onTextChange = [this] {changeTuning(tuneInput2, tuneArray2, synthArray2, synthArrayOsu2, synthArrayHanOsu2); };
		}
		tuneDropDown.onChange = [this] {toSetTune(); };
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
		freeTextButton.onClick = [this] {textBoxToggled(); };
		pdfButton.onClick = [this] {savePDF(); };
		saveButton.onClick = [this] {saveFile(); };
		loadButton.onClick = [this] {loadFile(); };
		freeTextPlaceHolder.onTextChange = [this] {updateFreeText(); };
		pageNextButton.onClick = [this] {nextPage(); };
		pageBackButton.onClick = [this] {prevPage(); };

		//Colours
		titleInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		titleInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

		titleVertButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		titleVertButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		titleVertButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
		titleVertButton.setClickingTogglesState(true);

		authInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
		authInput.setColour(juce::TextEditor::textColourId, juce::Colours::black);

		authVertButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		authVertButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		authVertButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
		authVertButton.setClickingTogglesState(true);

		tuneKoto2.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::white);
		tuneKoto2.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		tuneKoto2.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);

		for (int i = 0; i < 13; i++) {
			tuneInput[i].setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
			tuneInput[i].setColour(juce::TextEditor::textColourId, juce::Colours::black);
		}
		for (int i = 0; i < 13; i++) {
			tuneInput2[i].setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
			tuneInput2[i].setColour(juce::TextEditor::textColourId, juce::Colours::black);
		}
		tuneDropDown.setColour(juce::ComboBox::buttonColourId, juce::Colour(250, 210, 150));
		tuneDropDown.setColour(juce::ComboBox::backgroundColourId, juce::Colour(250, 210, 150));
		tuneDropDown.setColour(juce::PopupMenu::backgroundColourId, juce::Colour(250, 210, 150));
		tuneDropDown.setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(250, 190, 150));
		tuneDropDown.setColour(juce::ComboBox::textColourId, juce::Colours::black);
		tuneDropDown.setColour(juce::PopupMenu::textColourId, juce::Colours::black);

		infoButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		infoButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		infoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

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
		playButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		stopButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		stopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		freeTextButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		freeTextButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 190, 120));
		freeTextButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
		freeTextButton.setToggleable(true);
		freeTextButton.setClickingTogglesState(true);

		pdfButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pdfButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		pdfButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		saveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		loadButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		pageNextButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pageNextButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		pageNextButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

		pageBackButton.setColour(juce::TextButton::buttonColourId, juce::Colour(250, 210, 150));
		pageBackButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(250, 210, 150));
		pageBackButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);


		//Input setup
		bpmInput.setInputRestrictions(4, "1234567890");
		bpmInput.setText("120");

		for (int i = 0; i < tuneInput.size(); i++) {
			tuneInput[i].setInputRestrictions(7, "1234567890.");
		}
		for (int i = 0; i < tuneInput.size(); i++) {
			tuneInput2[i].setInputRestrictions(7, "1234567890.");
		}

		//Drop-down setup
		tuneDropDown.addItem(u8"平調子", 1); //hira
		tuneDropDown.addItem(u8"雲井調子", 2); //kumoi
		tuneDropDown.addItem(u8"本雲井調子", 3); //honkumoi
		tuneDropDown.addItem(u8"半雲井調子", 4); //hankumoi
		tuneDropDown.addItem(u8"楽調子", 5); //raku
		tuneDropDown.addItem(u8"乃木調子", 6); //nogi
		tuneDropDown.addItem(u8"中空調子", 7); //chūkū
		tuneDropDown.addItem(u8"古今調子", 8); //kokon


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

			synthArray2[i].fund = tuneArray[i];
			synthArray2[i].updateFrequency(sr);

			//Update full string press synths
			synthArrayOsu[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu[i].updateFrequency(sr);

			synthArrayOsu2[i].fund = tuneArray[i] * 1.122462;
			synthArrayOsu2[i].updateFrequency(sr);

			//Update half string press synths
			synthArrayHanOsu[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu[i].updateFrequency(sr);

			synthArrayHanOsu2[i].fund = tuneArray[i] * 1.059463;
			synthArrayHanOsu2[i].updateFrequency(sr);
		}
	}

	void releaseResources() override {}

	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
	{

		for (int i = 0; i < playbackPlaying.size(); i++) {
			//Skip if note is ,
			if (playbackPlaying[i].note == ",") {
				continue;
			}

			//If the assigned start point (length) is within the current buffer, play the note
			if (playbackPlaying[i].length >= playbackPointer && playbackPlaying[i].length < (playbackPointer + bufferToFill.numSamples)) {
				int n = std::stoi(playbackPlaying[i].note);
				std::string orn = playbackPlaying[i].ornament;


				//Make sure the note is paired with a valid synth
				if (n >= 0 && n < 13) {
					//Check wich set of synths to play
					if (playbackPlaying[i].isSecond) {
						//If there's a string press, play the relevant pressed synth, otherwise play the regular one
						if (std::regex_search(orn, std::regex("o"))) {
							synthArrayOsu2[n].playNote();
						}
						else if (std::regex_search(orn, std::regex("p"))) {
							synthArrayHanOsu2[n].playNote();
						}
						else {
							synthArray2[n].playNote();
						}
					}
					else {
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

		//Watermark
		g.setColour(juce::Colours::black);
		g.setFont(labelFont.withHeight(18));
		g.drawSingleLineText("built by Briar Irving", 20, (getWidth() / 25) + 30);


		//Inputs
		//Song title
		g.setFont(labelFont.withHeight(20));
		g.drawSingleLineText(u8"曲名・Title", 20, titleInput.getY() - 5);

		//Author
		g.drawSingleLineText(u8"作曲・Author",
			authVertButton.getX() + authVertButton.getWidth() + 10,
			authInput.getY() + (authInput.getHeight() - 5));

		//Tuning
		g.drawSingleLineText(u8"調子・Tuning (Hz)", 20, tuneInput[1].getY() - 10);

		g.drawSingleLineText(u8"二箏調子・2nd Instrument Tuning",
			tuneKoto2.getX() + tuneKoto2.getWidth(),
			tuneKoto2.getY() + (tuneKoto2.getHeight() - 5));

		//BPM
		g.drawSingleLineText(u8"一拍・BPM",
			bpmInput.getX() + bpmInput.getWidth() + 10,
			bpmInput.getY() + (bpmInput.getHeight() - 5));

		//Second koto button
		g.drawSingleLineText(u8"第二箏",
			addKotoButton.getX() + addKotoButton.getWidth(),
			addKotoButton.getY() + (addKotoButton.getHeight() - 5));

		//Github
		g.setFont(labelFont.withHeight(18));
		g.drawSingleLineText("See github brirving for code and walkthrough",
			scoreInput.getX(),
			scoreInput.getY() + (scoreInput.getHeight() + 25));


	}

	void resized() override
	{
		titleInput.setBounds(20, ((getHeight() / 20) * 3), (getWidth() / 3) - 40, 20);
		titleVertButton.setBounds(titleInput.getWidth() + 25, titleInput.getY(), 30, 20);

		authInput.setBounds(20, ((getHeight() / 20) * 4), (getWidth() / 4) - 40, 20);
		authVertButton.setBounds(authInput.getWidth() + 25, authInput.getY(), 30, 20);

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
		for (int i = 0; i < 13; i++) {
			int y;
			if (i >= 7) {
				y = 30;
			}
			else {
				y = 0;
			}
			tuneInput2[i].setBounds((((getWidth() / 2) / 7) - 2.857) * (i % 7) + 20,
				(getHeight() / 10) * 3 + y, ((getWidth() / 2) / 7) - 20, 20);
		}

		juce::Font f = labelFont;
		tuneKoto2.setBounds(80 + f.getStringWidth(u8"調子・Tuning(Hz)"), tuneInput[1].getY() - 25, 30, 20);
		tuneDropDown.setBounds(20, tuneInput[12].getY() + 25, getWidth() / 5, 20);

		infoButton.setBounds(20, (getHeight() / 10) * 4.5, 20, 20);

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

		freeTextButton.setBounds(getWidth() / 2 - 12.5, titleInput.getY() + 50, 25, 25);

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

	void showTuning2() {
		if (tuneKoto2.getToggleState()) {
			for (int i = 0; i < 13; i++)
			{
				tuneInput2[i].setVisible(true);
				tuneInput[i].setVisible(false);
			}
		}
		else {
			for (int i = 0; i < 13; i++)
			{
				tuneInput2[i].setVisible(false);
				tuneInput[i].setVisible(true);
			}
		}
	}
	void changeTuning(std::array<juce::TextEditor, 13>& tuneInputChange, std::array<double, 13>& tuneArrayChange,
		std::array<kotoSynth, 13>& synthArrayChange, std::array<kotoSynth, 13>& synthArrayOsuChange,
		std::array<kotoSynth, 13>& synthArrayHanOsuChange) {

		//Update tuneArray
		for (int i = 0; i < tuneInputChange.size(); i++) {
			if (tuneInputChange[i].getText().length() > 0) {
				try
				{
					tuneArrayChange[i] = std::stod(tuneInputChange[i].getText().toStdString());
				}
				catch (const std::exception&)
				{
					continue;
				}
			}
		}

		//Update synths
		for (int i = 0; i < synthArrayChange.size(); i++) {
			//Update main synths
			synthArrayChange[i].fund = tuneArrayChange[i];
			synthArrayChange[i].updateFrequency(sr);

			//Update full string press synths
			synthArrayOsuChange[i].fund = tuneArrayChange[i] * 1.122462;
			synthArrayOsuChange[i].updateFrequency(sr);

			//Update half string press synths
			synthArrayHanOsuChange[i].fund = tuneArrayChange[i] * 1.059463;
			synthArrayHanOsuChange[i].updateFrequency(sr);
		}
	}
	void toSetTune() {
		if (tuneKoto2.getToggleState()) {
			//Set tuneArray2 to the selected preset
			tuneArray2 = setTuneArray[tuneDropDown.getSelectedId() - 1];

			//Update inputs
			for (int i = 0; i < tuneArray2.size(); i++) {
				std::string fund = std::to_string(tuneArray2[i]).substr(0, 6);
				tuneInput2[i].setText(fund);
			}

		}
		else {
			//Set tuneArray to the selected preset
			tuneArray = setTuneArray[tuneDropDown.getSelectedId() - 1];

			//Update inputs
			for (int i = 0; i < tuneArray.size(); i++) {
				std::string fund = std::to_string(tuneArray[i]).substr(0, 6);
				tuneInput[i].setText(fund);
			}
		}

	}
	void popInfo()
	{
		helpWindow = (new HelpWindow("Koto score creator help"));
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


		std::vector<scoreHolder> playbackHolderChord;
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

			//assign instrument
			if (isSecond) {
				playbackHolder[i].isSecond = true;
			}

			//check for chords
			if (playbackHolder[i].note.length() > 1) {
				//Get iterator for the chord
				std::regex del("");
				std::sregex_token_iterator it(playbackHolder[i].note.begin() + 1,
					playbackHolder[i].note.end(), del, -1);
				std::sregex_token_iterator end;

				std::vector<std::string> noteArray;

				while (it != end) {
					//Add each note to the array
					noteArray.push_back(*it);

					++it;
				}

				for (int j = 1; j < noteArray.size(); j++) { //starts at 1 because the above code always outputs a blank string first
					//Add copy of chord to the chord array
					playbackHolderChord.push_back(playbackHolder[i]);

					//Set the note for the copies to each individual note
					int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), noteArray[j]));
					playbackHolderChord.back().note = std::to_string(n);
				}

				//Set the note of the original to just the first note in the chord
				playbackHolder[i].note = playbackHolder[i].note.front();

			}

			//Set the note to the synth number
			int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), playbackHolder[i].note));
			playbackHolder[i].note = std::to_string(n);


			//add note length to playtime
			timeToPlay += t;




		}


		timeToPlay = 0;
		//convert notes to synth numbers for left hand
		for (int i = 0; i < playbackHolderLeft.size(); i++) {
			if (playbackHolderLeft[i].note == ",") {
				continue;
			}


			//get length of note in samples
			double t = (beat * sr) * (4.0f / playbackHolderLeft[i].length);

			playbackHolderLeft[i].length = timeToPlay;

			//assign instrument
			if (isSecond) {
				playbackHolderLeft[i].isSecond = true;
			}


			//check for chords
			if (playbackHolderLeft[i].note.length() > 1) {
				//Get iterator for the chord
				std::regex del("");
				std::sregex_token_iterator it(playbackHolderLeft[i].note.begin() + 1,
					playbackHolderLeft[i].note.end(), del, -1);
				std::sregex_token_iterator end;

				std::vector<std::string> noteArray;

				while (it != end) {
					//Add each note to the array
					noteArray.push_back(*it);

					++it;
				}

				for (int j = 1; j < noteArray.size(); j++) { //starts at 1 because the above code always outputs a blank string first
					//Add copy of chord to the chord array
					playbackHolderChord.push_back(playbackHolderLeft[i]);

					//Set the note for the copies to each individual note
					int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), noteArray[j]));
					playbackHolderChord.back().note = std::to_string(n);
				}

				//Set the note of the original to just the first note in the chord
				playbackHolderLeft[i].note = playbackHolderLeft[i].note.front();

			}

			//Set the note to the synth number
			if (std::ranges::contains(noteInputVals, playbackHolderLeft[i].note)) {
				int n = std::distance(noteInputVals.begin(), std::find(noteInputVals.begin(), noteInputVals.end(), playbackHolderLeft[i].note));
				playbackHolderLeft[i].note = std::to_string(n);
			}
			else {
				playbackHolderLeft[i].note = "14";
			}

			//add note length to playtime
			timeToPlay += t;
		}

		//Append left hand
		playbackHolder.insert(playbackHolder.end(), playbackHolderLeft.begin(), playbackHolderLeft.end());

		//Append chords
		playbackHolder.insert(playbackHolder.end(), playbackHolderChord.begin(), playbackHolderChord.end());

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


			synthArray2[i].fund = tuneArray2[i];
			synthArray2[i].updateFrequency(sr);


			synthArrayOsu2[i].fund = tuneArray2[i] * 1.122462;
			synthArrayOsu2[i].updateFrequency(sr);


			synthArrayHanOsu2[i].fund = tuneArray2[i] * 1.059463;
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
	void textBoxToggled() { scoreSheet.toggleCursor(freeTextButton.getToggleState()); }
	void savePDF() {
		//Get size of page to modify for PDF, doubled to reduce pixelation
		double mod = 2 * (595.0f / (scoreSheet.getWidth()));

		//Create array of page images
		std::vector<juce::Image> imgArray;
		scoreSheet.page = 1;
		prevPage();
		for (int i = 0; i <= scoreSheet.maxPage; i++) {
			juce::Image snap = scoreSheet.createComponentSnapshot(juce::Rectangle(scoreSheet.getWidth(), scoreSheet.getHeight()), true, mod);
			imgArray.push_back(snap);
			nextPage();
		}

		//Write pdf
		pdfMaker.savePDF(imgArray);
	}
	void saveFile() {
		//Get contents of inputs
		//Title
		juce::String title = titleInput.getText();

		//Author
		juce::String auth = authInput.getText();

		//Tuning
		std::string tune;
		for (int i = 0; i < tuneArray.size(); i++) {
			tune += std::to_string(tuneArray[i]) + ",";
		}
		std::string tune2;
		for (int i = 0; i < tuneArray2.size(); i++) {
			tune2 += std::to_string(tuneArray2[i]) + ",";
		}

		//BPM
		juce::String bpm = bpmInput.getText();

		//Score
		juce::String score = scoreInput.getText();
		juce::String score2 = scoreInput2.getText();

		//Free text
		std::string freeTextString;
		for (int i = 0; i < scoreSheet.freeTextHolder.size(); i++) {

			//Visibility
			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->page) + "|";
			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->deleted) + "|";

			//Text and state
			freeTextString += scoreSheet.freeTextHolder[i]->label.getText().replace("\n", "").toStdString() + "|";
			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->rotated) + "|";
			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->vertical) + "|";

			//Coordinates
			//Undo transformations before grabbing coordinates
			freeTextString += scoreSheet.freeTextHolder[i]->xyPoint.toString().toStdString() + "|";

			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->ogWidth) + "|";
			freeTextString += std::to_string(scoreSheet.freeTextHolder[i]->ogHeight) + "\n";
		}


		//Save contents in txt file

		fileSaver.saveFile(title + "\n" + auth + "\n" + juce::String(tune) + "\n" + juce::String(tune2) + "\n" + bpm + "\n" + score + "\n" + score2 + "\n" + freeTextString);
	}
	void loadFile() {
		juce::String freeTextString = "placeholder";
		fileSaver.loadFile(titleInput, authInput, tuneInput, tuneInput2, bpmInput, scoreInput, scoreInput2, addKotoButton, freeTextPlaceHolder);
	}
	void updateFreeText() {
		juce::String freeTextString = freeTextPlaceHolder.getText();

		if (freeTextString != "placeholder") {
			scoreSheet.freeTextHolder.clear();

			auto freeTextArray = juce::StringArray::fromTokens(freeTextString, "@", "");

			for (int i = 0; i < freeTextArray.size(); i++) {
				auto freeTextSub = juce::StringArray::fromTokens(freeTextArray[i], "|", "");

				if (freeTextSub.size() == 8) {
					//Visibility
					auto page = std::stoi(freeTextSub[0].toStdString());
					bool deleted = std::stoi(freeTextSub[1].toStdString());

					//Text and state
					auto labelText = freeTextSub[2];
					bool rotated = std::stoi(freeTextSub[3].toStdString());
					bool vertical = std::stoi(freeTextSub[4].toStdString());

					//Coordinates
					auto xyString = juce::StringArray::fromTokens(freeTextSub[5], ",", "");
					auto x = std::stoi(xyString[0].toStdString());
					auto y = std::stoi(xyString[1].toStdString());
					juce::Point<int> xy(x, y);

					auto ogWidth = std::stoi(freeTextSub[6].toStdString());
					auto ogHeight = std::stoi(freeTextSub[7].toStdString());

					if (scoreSheet.getWidth() != ogWidth) {
						xy *= static_cast<float>(scoreSheet.getWidth()) / ogWidth;
					}

					//Create the free text with the above values
					scoreSheet.freeTextHolder.emplace_back(new freeText(xy, page, scoreSheet.getWidth(), scoreSheet.getHeight()));
					scoreSheet.freeTextHolder[i]->deleted = deleted;
					scoreSheet.freeTextHolder[i]->label.setText(labelText, juce::dontSendNotification);

					//Set location
					scoreSheet.freeTextHolder.back()->setBounds(xy.getX(), xy.getY(), scoreSheet.getWidth(), 50);
					if (vertical) {
						scoreSheet.freeTextHolder[i]->setVertical();
					}
					if (rotated) {
						scoreSheet.freeTextHolder[i]->rotateText();
					}

					//Add as child component of the score
					if (!deleted && page == scoreSheet.page) {
						scoreSheet.addAndMakeVisible(*scoreSheet.freeTextHolder.back());
					}
					else {
						scoreSheet.addChildComponent(*scoreSheet.freeTextHolder.back());
					}

					freeTextPlaceHolder.setText("placeholder", juce::dontSendNotification);
				}
			}
		}
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
	juce::TextButton titleVertButton{ u8"A\u2193" };

	//Author
	juce::TextEditor authInput;
	juce::TextButton authVertButton{ u8"A\u2193" };

	//Tuning
	std::array<juce::TextEditor, 13> tuneInput;
	std::array<juce::TextEditor, 13> tuneInput2;

	juce::ToggleButton tuneKoto2;

	//Set to hirachōshi D
	juce::ComboBox tuneDropDown;

	//Info
	juce::TextButton infoButton{ "?" };
	juce::Component::SafePointer<HelpWindow> helpWindow;

	//BPM
	juce::TextEditor bpmInput;

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

	//Free text button
	juce::TextButton freeTextButton{ u8"\U0001F130" };

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
	std::array<double, 13> tuneArray2{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };

	//Tunings
	std::array<double, 13> hiraTuning{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 440, 466.16, 587.33, 622.25, 783.99, 880 };
	std::array<double, 13> kumoiTuning{ 146.83, 196, 207.65, 261.63, 293.66, 311.13, 392, 415.3, 523.25, 587.33, 622.25, 783.99, 880 };
	std::array<double, 13> honkumoiTuning{ 146.83, 196, 207.65, 261.63, 293.66, 311.13, 392, 415.3, 523.25, 587.33, 622.25, 783.99, 830.61 };
	std::array<double, 13> hankumoiTuning{ 146.83, 196, 220, 233.08, 293.66, 311.13, 392, 415.3, 523.25, 587.33, 622.25, 783.99, 880 };
	std::array<double, 13> rakuTuning{ 146.83, 196, 220, 261.63, 293.66, 329.63, 392, 440, 523.25, 587.33, 659.25, 783.99, 880 };
	std::array<double, 13> nogiTuning{ 146.83, 196, 220, 246.94, 293.66, 329.63, 392, 440, 493.88, 587.33, 659.25, 783.99, 880 };
	std::array<double, 13> chukuTuning{ 146.83, 196, 220, 233.08, 293.66, 329.63, 349.23, 440, 466.16, 587.33, 659.25, 698.46, 880 };
	std::array<double, 13> kokonTuning{ 146.83, 392, 220, 261.63, 293.66, 311.13, 392, 440, 523.25, 587.33, 622.25, 783.99, 880 };
	std::array<std::array<double, 13>, 8> setTuneArray{ hiraTuning, kumoiTuning, honkumoiTuning, hankumoiTuning, rakuTuning, nogiTuning, chukuTuning, kokonTuning };

	//Playback
	std::vector<scoreHolder> playbackHold;
	std::vector<scoreHolder> playbackHold2;
	std::vector<scoreHolder> playbackPlaying;
	double sr;
	double playbackPointer = 0;

	//Component for saving and loading files
	saver fileSaver;
	pdfCreator pdfMaker;
	juce::Label freeTextPlaceHolder{ "placeholder" };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
