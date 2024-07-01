/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

// Some nice example drawing
class Lissajour   : public juce::Component,
                    private juce::Timer
{
public:
    enum ColourIDs
    {
        // we are safe from collissions, because we set the colours on every component directly from the stylesheet
        backgroundColourId,
        drawColourId,
        fillColourId
    };

    Lissajour()
    {
        // make sure you define some default colour, otherwise the juce lookup will choke
        setColour (backgroundColourId, juce::Colours::black);
        setColour (drawColourId, juce::Colours::green);
        setColour (fillColourId, juce::Colours::green.withAlpha (0.5f));

        startTimerHz (30);
    }

    void setFactor (float f)
    {
        factor = f;
    }

    void paint (juce::Graphics& g) override
    {
        const float radius = std::min (getWidth(), getHeight()) * 0.4f;
        const auto  centre = getLocalBounds().getCentre().toFloat();

        g.fillAll (findColour (backgroundColourId));
        juce::Path p;
        p.startNewSubPath (centre + juce::Point<float>(0, std::sin (phase)) * radius);
        for (auto i = 0.1f; i <= juce::MathConstants<float>::twoPi; i += 0.01f)
            p.lineTo (centre + juce::Point<float>(std::sin (i),
                                            std::sin (std::fmod (i * factor + phase,
                                                                 juce::MathConstants<float>::twoPi))) * radius);
        p.closeSubPath();

        g.setColour (findColour (drawColourId));
        g.strokePath (p, juce::PathStrokeType (2.0f));

        const auto fillColour = findColour (fillColourId);
        if (fillColour.isTransparent() == false)
        {
            g.setColour (fillColour);
            g.fillPath (p);
        }
    }

private:
    void timerCallback() override
    {
        phase += 0.1f;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;

        repaint();
    }

    float factor = 3.0f;
    float phase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Lissajour)
};

// This class is creating and configuring your custom component
class LissajourItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (LissajourItem)

    LissajourItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node) : foleys::GuiItem (builder, node)
    {
        // Create the colour names to have them configurable
        setColourTranslation ({
            {"lissajour-background", Lissajour::backgroundColourId},
            {"lissajour-draw", Lissajour::drawColourId},
            {"lissajour-fill", Lissajour::fillColourId} });

        addAndMakeVisible (lissajour);
    }

    std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        std::vector<foleys::SettableProperty> newProperties;

        newProperties.push_back ({ configNode, "factor", foleys::SettableProperty::Number, 1.0f, {} });

        return newProperties;
    }

    // Override update() to set the values to your custom component
    void update() override
    {
        auto factor = getProperty ("factor");
        lissajour.setFactor (factor.isVoid() ? 3.0f : float (factor));
    }

    juce::Component* getWrappedComponent() override
    {
        return &lissajour;
    }

private:
    Lissajour lissajour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajourItem)
};

//==============================================================================

// Silly example class that displays some members from your specific processor
class StatisticsComponent : public juce::Component,
                            private juce::Timer
{
public:
    StatisticsComponent (ExtendingExampleAudioProcessor& processorToUse)
      : processor (processorToUse)
    {
        setOpaque (false);
        startTimerHz (30);
    }

    void paint (juce::Graphics& g) override
    {
        // silly example: display current actual buffer size
        auto sampleRate = processor.statisticsSampleRate.load();
        auto samples    = processor.statisticsSamplesPerBlock.load();

        g.setColour (juce::Colours::white);
        g.drawFittedText ("Samplerate: " + juce::String (sampleRate) + "\n" +
                          "Buffersize: " + juce::String (samples), getLocalBounds(), juce::Justification::centredLeft, 3);
    }

private:
    void timerCallback() override
    {
        repaint();
    }

    ExtendingExampleAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatisticsComponent)
};


class StatisticsComponentItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (StatisticsComponentItem)

    StatisticsComponentItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
      : foleys::GuiItem (builder, node)
    {
        if (auto* proc = dynamic_cast<ExtendingExampleAudioProcessor*>(builder.getMagicState().getProcessor()))
        {
            stats = std::make_unique<StatisticsComponent>(*proc);
            addAndMakeVisible (stats.get());
        }
    }

    void update() override {}

    juce::Component* getWrappedComponent() override
    {
        return stats.get();
    }

private:
    std::unique_ptr<StatisticsComponent> stats;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatisticsComponentItem)
};

//==============================================================================

ExtendingExampleAudioProcessor::ExtendingExampleAudioProcessor() : foleys::MagicProcessor()
{
    FOLEYS_SET_SOURCE_PATH (__FILE__);

    magicState.setGuiValueTree (BinaryData::magic_xml, BinaryData::magic_xmlSize);
}

ExtendingExampleAudioProcessor::~ExtendingExampleAudioProcessor() {}

//==============================================================================

void ExtendingExampleAudioProcessor::initialiseBuilder (foleys::MagicGUIBuilder& builder)
{
    builder.registerJUCEFactories();
    builder.registerJUCELookAndFeels();
    
    builder.registerFactory ("Lissajour", &LissajourItem::factory);
    builder.registerFactory ("Statistics", &StatisticsComponentItem::factory);
}

//==============================================================================
void ExtendingExampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    statisticsSampleRate.store (sampleRate);
}

void ExtendingExampleAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExtendingExampleAudioProcessor::isBusesLayoutSupported (const juce::AudioProcessor::BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ExtendingExampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // silly example: display current actual buffer size
    statisticsSamplesPerBlock.store (buffer.getNumSamples());
}

//==============================================================================
const juce::String ExtendingExampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

double ExtendingExampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExtendingExampleAudioProcessor();
}
