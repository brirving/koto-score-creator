// KotoNotation.cpp : Defines the entry point for the application.
//

#include "KotoNotation.h"
#include "Synth.h"

class KotoApplication : public juce::JUCEApplication
{
public:
    KotoApplication() = default;

    const juce::String getApplicationName() override { return "KotoNotation"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String&) override
    {
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colour(250, 190, 120),
                juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainContentComponent(), true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
            setResizable(true, true);
            setResizeLimits(650, 450, 8000, 6000);
 
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(KotoApplication)
