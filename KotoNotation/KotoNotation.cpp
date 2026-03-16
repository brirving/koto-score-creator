// KotoNotation.cpp : Defines the entry point for the application.
//

#include "KotoNotation.h"
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        setSize(400, 200);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);

        g.setColour(juce::Colours::black);
        g.setFont(24.0f);
        g.drawFittedText("Hello Me",
            getLocalBounds(),
            juce::Justification::centred,
            1);
    }
};

class HelloJuceApplication : public juce::JUCEApplication
{
public:
    HelloJuceApplication() = default;

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
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
                juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(HelloJuceApplication)
