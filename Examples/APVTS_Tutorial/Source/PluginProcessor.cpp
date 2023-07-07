/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/


#include "../JuceLibraryCode/JuceHeader.h"

class TutorialProcessor  : public foleys::MagicProcessor
{
public:
    //==============================================================================
    TutorialProcessor()
    : parameters (*this, nullptr, juce::Identifier ("APVTSTutorial"),
    {
        std::make_unique<juce::AudioParameterFloat> ("gain",            // parameterID
                                                     "Gain",            // parameter name
                                                     0.0f,              // minimum value
                                                     1.0f,              // maximum value
                                                     0.5f),             // default value
        std::make_unique<juce::AudioParameterBool> ("invertPhase",      // parameterID
                                                    "Invert Phase",     // parameter name
                                                    false)              // default value
    })
    {
        FOLEYS_SET_SOURCE_PATH (__FILE__);

        phaseParameter = parameters.getRawParameterValue ("invertPhase");
        gainParameter  = parameters.getRawParameterValue ("gain");
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        previousGain = *gainParameter * phase;
    }

    void releaseResources() override {}

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
        auto currentGain = *gainParameter * phase;

        if (currentGain == previousGain)
        {
            buffer.applyGain (currentGain);
        }
        else
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
    }

    //==============================================================================
    const juce::String getName() const override            { return "APVTS Tutorial"; }

    double getTailLengthSeconds() const override           { return 0; }

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    float previousGain; // [1]

    std::atomic<float>* phaseParameter = nullptr;
    std::atomic<float>* gainParameter  = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialProcessor();
}
