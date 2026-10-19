#pragma once
#include "PluginProcessor.h"

// Draws a rotary slider by rotating the knob PNG
class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPos, float startAngle, float endAngle,
        juce::Slider&) override;
private:
    juce::Image knobImage;
};

class HelloVSTEditor : public juce::AudioProcessorEditor
{
public:
    explicit HelloVSTEditor(HelloVSTProcessor&);
    ~HelloVSTEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    static constexpr int numKnobs = 5;

    HelloVSTProcessor& audioProcessor;

    // Declaration order matters: these are destroyed in reverse order
    KnobLookAndFeel lookAndFeel;
    juce::Image background;
    juce::Slider knobs[numKnobs];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[numKnobs];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelloVSTEditor)
};