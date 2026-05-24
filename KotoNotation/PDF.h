#pragma once
#include <iostream>
#include <PDFWriter.h>
#include <PDFPage.h>
#include <PageContentContext.h>
#include <PDFFormXObject.h>
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
			textHolder[i].setFont(labelFont, true);
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
	std::array<Bar, 14> bars2;
	int page = 0;
	int maxPage = 0;
	int maxPage1 = 0;
	int maxPage2 = 0;

	std::vector<scoreHolder> contents;
	std::vector<scoreHolder> contentsLeft;

	std::vector<scoreHolder> contents2;
	std::vector<scoreHolder> contentsLeft2;

	bool hasSecondKoto;

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

		//For Japanese text
//titleOut.setDrawableTransform(juce::AffineTransform::rotation(0).translated(sheetX + sheetW - (sheetW / 8), sheetY + 20));
//need to add a converter to add breaks between characters
//titleOut.setBoundingBox(juce::Rectangle<float>(20, sheetH - 80));

		titleOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 8), 20));
		titleOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
		titleOut.setJustification(juce::Justification::centred);
		titleOut.setFont(labelFont.withHeight(20.0f), true);

		authOut.setDrawableTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::halfPi).translated(getWidth() - (getWidth() / 20), 40));
		authOut.setBoundingBox(juce::Rectangle<float>(getHeight() - 80, 20));
		//authOut.setJustification(juce::Justification::centred);
		authOut.setFont(labelFont.withHeight(20.0f), true);

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
		}
		else if (isSecond == false && prev1 != maxPage1 && page == prev1) {
			page = maxPage1;
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

				bars[bNum].updateInput(arraySlice, arraySliceLeft);
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
		}
	}

	void pageDown() {
		if (page > 0) {
			page--;
			writeBars(contents, contentsLeft, false, hasSecondKoto);
			writeBars(contents2, contentsLeft2, true, hasSecondKoto);
		}
	}

	void updateTitle(juce::String title) { titleOut.setText(title); }
	void updateAuth(juce::String auth) {
		juce::String a1 = u8"作曲: ";
		juce::String a2 = auth;
		authOut.setText(a1 + a2);
	}

private:
	juce::FontOptions labelFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };

	//Author and title
	juce::DrawableText titleOut;
	juce::DrawableText authOut;
};

class pdfCreator {
public:

	pdfCreator() {}

	~pdfCreator() {}

	void savePDF(scoreComponent& scoreSheet) {
		auto fileToSave = juce::File::createTempFile("pdfScore");

		fc.reset(new juce::FileChooser("Choose a save location...",
			juce::File::getCurrentWorkingDirectory().getChildFile("pdfScore"),
			"*.pdf", true));

		fc->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
			[this, fileToSave, &scoreSheet](const juce::FileChooser& chooser)
			{
				auto result = chooser.getURLResult();
				auto name = result.isEmpty() ? juce::String()
					: (result.isLocalFile() ? result.getLocalFile().getFullPathName()
						: result.toString(true));

				// Android and iOS file choosers will create placeholder files for chosen
				// paths, so we may as well write into those files.
#if JUCE_ANDROID || JUCE_IOS
				if (!result.isEmpty())
				{
					std::unique_ptr<InputStream>  wi(fileToSave.createInputStream());
					std::unique_ptr<OutputStream> wo(result.createOutputStream());

					if (wi.get() != nullptr && wo.get() != nullptr)
					{
						[[maybe_unused]] auto numWritten = wo->writeFromInputStream(*wi, -1);
						jassert(numWritten > 0);
						wo->flush();
					}
				}
#endif
				//Check the file path is valid
				if (result.isWellFormed()) {
					std::string location = std::regex_replace(result.toString(false).toStdString(), std::regex("%3A"), ":");
					location = std::regex_replace(location, std::regex("file:/*"), "");
					
					//Create pdf to write to
					pdfWriter.StartPDF(location, ePDFVersion13);
					PDFPage* pdfPage = new PDFPage();
					pdfPage->SetMediaBox(PDFRectangle(0, 0, 595, 842));
					PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

					//Add score content
					double mod = 595 / scoreSheet.getWidth();
					juce::Image snap = scoreSheet.createComponentSnapshot(juce::Rectangle(scoreSheet.getWidth(), scoreSheet.getHeight()), true, mod);
					juce::PNGImageFormat img_fileForm;
					juce::MemoryOutputStream img_out;
					if (img_fileForm.writeImageToStream(snap, img_out) == true) {
						juce::File f = juce::File("C:/Users/beei/Downloads/pdfScore.png");
						juce::FileOutputStream fsaveImg(f);
						fsaveImg.setPosition(0);
						fsaveImg.write(img_out.getData(), img_out.getDataSize());
						fsaveImg.flush();
					}
					

					PDFFormXObject* image = pdfWriter.CreateFormXObjectFromPNGFile("C:/Users/beei/Downloads/pdfScore.png");

					pageContentContext->q();
					pageContentContext->cm(1, 0, 0, 1, 0, 0);
					pageContentContext->Do(
						pdfPage->GetResourcesDictionary().
						AddFormXObjectMapping(image->GetObjectID()));
					pageContentContext->Q();

					delete image;
					pdfWriter.EndPageContentContext(pageContentContext);

					pdfWriter.WritePageAndRelease(pdfPage);
					pdfWriter.EndPDF();
				}

			});
	}

private:
	std::unique_ptr<juce::FileChooser> fc;
	PDFWriter pdfWriter;
};


class saver {
public:

	saver() {}
	~saver() {}

	void saveFile(juce::String contents) {

		auto fileToSave = juce::File::createTempFile("kotoScore");

		fc.reset(new juce::FileChooser("Choose a save location...",
			juce::File::getCurrentWorkingDirectory().getChildFile("kotoScore"),
			"*.txt", true));

		fc->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
			[this, fileToSave, contents](const juce::FileChooser& chooser)
			{
				auto result = chooser.getURLResult();
				auto name = result.isEmpty() ? juce::String()
					: (result.isLocalFile() ? result.getLocalFile().getFullPathName()
						: result.toString(true));

				// Android and iOS file choosers will create placeholder files for chosen
				// paths, so we may as well write into those files.
#if JUCE_ANDROID || JUCE_IOS
				if (!result.isEmpty())
				{
					std::unique_ptr<InputStream>  wi(fileToSave.createInputStream());
					std::unique_ptr<OutputStream> wo(result.createOutputStream());

					if (wi.get() != nullptr && wo.get() != nullptr)
					{
						[[maybe_unused]] auto numWritten = wo->writeFromInputStream(*wi, -1);
						jassert(numWritten > 0);
						wo->flush();
					}
				}
#endif
				//Check the file path is valid
				if (result.isWellFormed()) {
					auto outputStream = result.createOutputStream();
					outputStream->writeString(contents);
				}

			});


	}

	void loadFile(juce::TextEditor& titleInput, juce::TextEditor& authInput, std::array<juce::TextEditor, 13>& tuneArray,
		juce::TextEditor& bpmInput, juce::TextEditor& scoreInput, juce::TextEditor& scoreInput2, juce::ToggleButton& addKotoButton) {


		fc.reset(new juce::FileChooser("Choose a file to open...", juce::File::getCurrentWorkingDirectory(),
			"*.txt", true));

		fc->launchAsync(juce::FileBrowserComponent::openMode
			| juce::FileBrowserComponent::canSelectFiles,
			[this, &titleInput, &authInput, &tuneArray, &bpmInput, &scoreInput, &scoreInput2, &addKotoButton](const juce::FileChooser& chooser)
			{
				auto result = chooser.getURLResult();

				//Check if it's a valid file location
				if (result.isWellFormed()) {
					//Get file contents
					auto inputStream = result.createInputStream(juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
						.withConnectionTimeoutMs(0));
					std::vector<juce::String> fileString;
					while (!inputStream->isExhausted()) {
						fileString.push_back(inputStream->readNextLine());
					}

					//Send contents to inputs
					titleInput.setText(fileString[0]);
					authInput.setText(fileString[1]);
					auto tunes = juce::StringArray::fromTokens(fileString[2], ",");
					for (int i = 0; i < tuneArray.size(); i++) {
						tuneArray[i].setText(tunes[i].dropLastCharacters(4));
					}
					bpmInput.setText(fileString[3]);
					scoreInput.setText(fileString[4]);
					if (fileString.size() >= 6) {
						scoreInput2.setText(fileString[5]);
						if (fileString[5].isNotEmpty()) {
							addKotoButton.setToggleState(true, juce::sendNotification);
						}
					}

				}
			});


	}

private:
	std::unique_ptr<juce::FileChooser> fc;
};