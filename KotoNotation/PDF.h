#pragma once
#include <iostream>

class pdfCreator {
public:

	pdfCreator() {}

	~pdfCreator() {}

	//void savePDF(scoreComponent score) {}

private:

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