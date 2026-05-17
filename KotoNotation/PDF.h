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
		
		auto fileToSave = juce::File::createTempFile("saveKotoNotation");

		fc.reset(new juce::FileChooser("Choose a file to save...",
			juce::File::getCurrentWorkingDirectory().getChildFile(fileToSave.getFileName()),
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

				auto outputStream = result.createOutputStream();
				outputStream->writeString(contents);

			});

		
	}

	void loadFile() {
		
		fc.reset(new juce::FileChooser("Choose a file to open...", juce::File::getCurrentWorkingDirectory(),
			"*.txt", true));

		fc->launchAsync(juce::FileBrowserComponent::openMode
			| juce::FileBrowserComponent::canSelectFiles,
			[this](const juce::FileChooser& chooser)
			{
				juce::String chosen;
				auto results = chooser.getURLResults();

				for (auto result : results)
					chosen << (result.isLocalFile() ? result.getLocalFile().getFullPathName()
						: result.toString(false)) << "\n";


			}); 
	}

private:
	std::unique_ptr<juce::FileChooser> fc;
};