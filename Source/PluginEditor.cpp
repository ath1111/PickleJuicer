#include "PluginEditor.h"
#include <BinaryData.h>

KnobLookAndFeel::KnobLookAndFeel()
{
    knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_png,
        BinaryData::knob_pngSize);
}

void KnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float startAngle, float endAngle,
    juce::Slider&)
{
    if (!knobImage.isValid())
        return;

    const float angle = startAngle + sliderPos * (endAngle - startAngle);
    const auto centre = juce::Rectangle<float>((float)x, (float)y,
        (float)width, (float)height).getCentre();
    const float scale = (float)juce::jmin(width, height) / (float)knobImage.getWidth();

    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImageTransformed(knobImage,
        juce::AffineTransform::translation(-knobImage.getWidth() * 0.5f,
            -knobImage.getHeight() * 0.5f)
        .rotated(angle)
        .scaled(scale)
        .translated(centre.x, centre.y));
}

HelloVSTEditor::HelloVSTEditor(HelloVSTProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    background = juce::ImageCache::getFromMemory(BinaryData::background_png,
        BinaryData::background_pngSize);

    // Must match the parameter IDs, in the same order as the positions in resized()
    const char* ids[numKnobs] = { "size", "damping", "width", "drive", "mix" };

    for (int i = 0; i < numKnobs; ++i)
    {
        auto& s = knobs[i];
        s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,   // about 7 o'clock
            juce::MathConstants<float>::pi * 2.75f,   // about 5 o'clock
            true);
        s.setLookAndFeel(&lookAndFeel);
        addAndMakeVisible(s);

        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.apvts, ids[i], s);
    }

    setSize(1000, 500);
}

HelloVSTEditor::~HelloVSTEditor()
{
    for (auto& s : knobs)
        s.setLookAndFeel(nullptr);
}

void HelloVSTEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.drawImage(background, getLocalBounds().toFloat());
}

void HelloVSTEditor::resized()
{
    // Knob centres in pixels on your 1000x500 image. Tune these to your artwork.
    const juce::Point<int> centres[numKnobs] = {
        { 720, 340 },   // size
        { 720, 65 },   // damping
        { 720, 425 },   // width
        { 720, 240 },   // drive
        { 720, 150 }    // mix
    };

    constexpr int knobSize = 100;
    

    for (int i = 0; i < numKnobs; ++i)
        knobs[i].setBounds(centres[i].x - knobSize / 2,
            centres[i].y - knobSize / 2,
            knobSize,
            knobSize);
}