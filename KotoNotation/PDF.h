#pragma once
#include <iostream>
#include <PDFWriter.h>
#include <PDFPage.h>
#include <PageContentContext.h>
#include <PDFFormXObject.h>
#include <regex>
#include <juce_gui_basics/juce_gui_basics.h>

class pdfCreator {
public:

	pdfCreator() {}

	~pdfCreator() {}

	void savePDF(std::vector<juce::Image> imgArray) {
		auto fileToSave = juce::File::createTempFile("pdfScore");

		fc.reset(new juce::FileChooser("Choose a save location...",
			juce::File::getCurrentWorkingDirectory().getChildFile("pdfScore"),
			"*.pdf", true));

		fc->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
			[this, fileToSave, imgArray](const juce::FileChooser& chooser)
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

					//Add score content
					juce::File f = juce::File::createTempFile("png");

					//Write each page
					for (int i = 0; i < imgArray.size(); i++) {
						PDFPage* pdfPage = new PDFPage();
						pdfPage->SetMediaBox(PDFRectangle(0, 0, 595, 842));
						PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(pdfPage);

						juce::PNGImageFormat img_fileForm;
						juce::MemoryOutputStream img_out;

						if (img_fileForm.writeImageToStream(imgArray[i], img_out) == true) {
							juce::FileOutputStream fsaveImg(f);
							fsaveImg.setPosition(0);
							fsaveImg.write(img_out.getData(), img_out.getDataSize());
							fsaveImg.flush();
						}

						PDFFormXObject* image = pdfWriter.CreateFormXObjectFromPNGFile(f.getFullPathName().toStdString());

						pageContentContext->q();
						pageContentContext->cm(0.5, 0, 0, 0.5, 0, 0);
						pageContentContext->Do(
							pdfPage->GetResourcesDictionary().
							AddFormXObjectMapping(image->GetObjectID()));
						pageContentContext->Q();

						delete image;
						pdfWriter.EndPageContentContext(pageContentContext);

						pdfWriter.WritePageAndRelease(pdfPage);
					}

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
					juce::FileOutputStream outputStream(result.getLocalFile());
					outputStream.setPosition(0);
					outputStream.writeText(contents, false, false, "\n");
					outputStream.flush();
				}

			});


	}

	void loadFile(juce::TextEditor& titleInput, juce::TextEditor& authInput, std::array<juce::TextEditor, 13>& tuneInput, std::array<juce::TextEditor, 13>& tuneInput2,
		juce::TextEditor& bpmInput, juce::TextEditor& scoreInput, juce::TextEditor& scoreInput2, juce::ToggleButton& addKotoButton, juce::Label& freeTextString) {


		fc.reset(new juce::FileChooser("Choose a file to open...", juce::File::getCurrentWorkingDirectory(),
			"*.txt", true));

		fc->launchAsync(juce::FileBrowserComponent::openMode
			| juce::FileBrowserComponent::canSelectFiles,
			[this, &titleInput, &authInput, &tuneInput, &tuneInput2, &bpmInput, &scoreInput, &scoreInput2, &addKotoButton, &freeTextString](const juce::FileChooser& chooser)
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
					auto tunes = juce::StringArray::fromTokens(fileString[2], ",", "");
					for (int i = 0; i < tuneInput.size(); i++) {
						tuneInput[i].setText(tunes[i].dropLastCharacters(4));
					}
					auto tunes2 = juce::StringArray::fromTokens(fileString[3], ",", "");
					for (int i = 0; i < tuneInput2.size(); i++) {
						tuneInput2[i].setText(tunes2[i].dropLastCharacters(4));
					}
					bpmInput.setText(fileString[4]);
					scoreInput.setText(fileString[5]);
					scoreInput2.setText(fileString[6]);
					if (fileString[5].isNotEmpty()) {
						addKotoButton.setToggleState(true, juce::sendNotification);
					}
					juce::String freeTextHolder;
					for(int i = 7; i < fileString.size(); i++){
						freeTextHolder += fileString[i] + "@";
					}
					freeTextString.setText(freeTextHolder, juce::NotificationType::sendNotification);
				}
			});
	}

private:
	std::unique_ptr<juce::FileChooser> fc;
};