#include "PluginProcessor.h"
#include "PluginEditor.h"

HelloVSTProcessor::HelloVSTProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "PARAMS", createLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout HelloVSTProcessor::createLayout()
{
    using P = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<P>(juce::ParameterID{ "size",    1 }, "Size", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<P>(juce::ParameterID{ "damping", 1 }, "Damping", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<P>(juce::ParameterID{ "width",   1 }, "Width", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<P>(juce::ParameterID{ "mix",     1 }, "Mix", 0.0f, 1.0f, 0.3f));

    layout.add(std::make_unique<P>(juce::ParameterID{ "drive", 1 }, "Drive",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    return layout;
}

void HelloVSTProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate,
                                  (juce::uint32)samplesPerBlock,
                                  (juce::uint32)getTotalNumOutputChannels() };
    reverb.prepare(spec);
    reverb.reset();

    smoothedDrive.reset(sampleRate, 0.05);   // 50 ms ramp
    smoothedDrive.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(apvts.getRawParameterValue("drive")->load()));
}

bool HelloVSTProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void HelloVSTProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // --- Saturation ---
    const float driveDb = apvts.getRawParameterValue("drive")->load();
    smoothedDrive.setTargetValue(juce::Decibels::decibelsToGain(driveDb));

    auto* const* channels = buffer.getArrayOfWritePointers();
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        const float g = smoothedDrive.getNextValue();   // g >= 1
        const float norm = 1.0f / std::tanh(g);         // keeps full-scale input at full-scale output

        for (int ch = 0; ch < numChannels; ++ch)
            channels[ch][i] = std::tanh(g * channels[ch][i]) * norm;
    }

    // --- Reverb (unchanged) ---
    const float mix = apvts.getRawParameterValue("mix")->load();

    juce::dsp::Reverb::Parameters p;
    p.roomSize = apvts.getRawParameterValue("size")->load();
    p.damping = apvts.getRawParameterValue("damping")->load();
    p.width = apvts.getRawParameterValue("width")->load();
    p.wetLevel = mix;
    p.dryLevel = 1.0f - mix;
    p.freezeMode = 0.0f;
    reverb.setParameters(p);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

juce::AudioProcessorEditor* HelloVSTProcessor::createEditor()
{
    return new HelloVSTEditor(*this);
}

void HelloVSTProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void HelloVSTProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HelloVSTProcessor();
}