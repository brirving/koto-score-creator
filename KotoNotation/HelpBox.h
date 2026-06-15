#pragma once
#include <iostream>
#include <BinaryData.h>
#include <juce_graphics/juce_graphics.h>

class HelpContents : public juce::Component {
public:
	HelpContents() {
		setSize(390, 420);
	}

	~HelpContents() {}


	void paint(juce::Graphics& g) override {
		g.setColour(juce::Colours::white);
		g.fillRect(10, 10, getWidth() - 12, getHeight() - 20);

		g.setFont(helpFont.withHeight(18));

		//Create help text contents
		juce::String title = juce::CharPointer_UTF8("楽譜書き方・How to write score:");

		juce::String strings = juce::CharPointer_UTF8("\n\n1, 2, 3...    =    一, 二, 三...\nq, w, e, r    =    十, 斗, 為, 巾");
		juce::String rests = juce::CharPointer_UTF8("\n0    =    ⦿・rest\n-    =    〇・rest");
		juce::String length = juce::CharPointer_UTF8("\n/    =     ♪\n//    =    ♬"); //\U0001d160 \U0001d161
		juce::String ornaments = juce::CharPointer_UTF8("\n\no    =    オ\np    =    ヲ\ns    =    ス\nh    =    ヒ\n.    =    ・");
		juce::String bar = juce::CharPointer_UTF8("\n\n,    =    次の小節・new bar");
		juce::String chord = juce::CharPointer_UTF8("\n(...)    =    和音・chord");
		juce::String left = juce::CharPointer_UTF8("\nl...l    =    左・left hand");

		//Draw help text
		g.setColour(juce::Colours::black);
		g.drawMultiLineText(title + strings + rests + length + ornaments + bar + chord + left,
			15, 30, getWidth() - 20);
	}

private:
	juce::FontOptions helpFont{ juce::Typeface::createSystemTypefaceFor(BinaryData::YujiSyukuRegular_ttf, BinaryData::YujiSyukuRegular_ttfSize) };
};

class HelpView : public juce::Viewport {
	public:
	HelpView() {
		setSize(400, 300);

		//Create viewport to scroll over help text
		setViewedComponent(new HelpContents);
		getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colour(250, 190, 120));

	}

private:
};

class HelpWindow : public juce::DocumentWindow {
public:
	HelpWindow(juce::String name)
		: DocumentWindow(name,
			juce::Colour(250, 210, 150),
			juce::DocumentWindow::allButtons)
	{
		//Create window for help text
		setUsingNativeTitleBar(true);
		setContentOwned(new HelpView(), true);
		centreWithSize(getWidth(), getHeight());
		setVisible(true);

		//Resizing
		setResizable(false, false);
		setResizeLimits(250, 200, 8000, 6000);

		//Keep window on top
		setAlwaysOnTop(true);

	}

	void closeButtonPressed() override
	{
		//Delete on close
		delete this;
	}
};