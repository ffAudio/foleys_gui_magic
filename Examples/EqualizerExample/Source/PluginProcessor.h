/*
  ==============================================================================

    A simple equalizer using the JUCE dsp module

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

template<typename ValueType>
class AttachedValue : private juce::AudioProcessorValueTreeState::Listener
{
public:
    AttachedValue (juce::AudioProcessorValueTreeState& state, std::atomic<ValueType>& value, const juce::String& paramID, std::function<void()> changedLambda=nullptr);
    ~AttachedValue() override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;
private:
    void initialUpdate();

    juce::AudioProcessorValueTreeState& state;
    std::atomic<ValueType>& value;
    juce::String paramID;
    std::function<void()> onParameterChanged;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AttachedValue)
};

//==============================================================================

class EqualizerExampleAudioProcessor  : public foleys::MagicProcessor,
                                        private juce::AudioProcessorValueTreeState::Listener,
                                        private juce::AsyncUpdater
{
public:

    using FilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Gain       = juce::dsp::Gain<float>;

    enum FilterType
    {
        NoFilter = 0,
        HighPass,
        HighPass1st,
        LowShelf,
        BandPass,
        Notch,
        Peak,
        HighShelf,
        LowPass1st,
        LowPass,
        LastFilterID
    };

    //==============================================================================
    EqualizerExampleAudioProcessor();
    ~EqualizerExampleAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================

    class FilterAttachment
    {
    public:
        FilterAttachment (juce::AudioProcessorValueTreeState& state, FilterBand& filter, const juce::String& prefix, const juce::CriticalSection& lock);

        void setSampleRate (double sampleRate);
        bool isActive() const { return active; }

        std::function<void(const FilterAttachment&)> postFilterUpdate;

        juce::dsp::IIR::Coefficients<float>::Ptr coefficients;
        double                                   sampleRate = 0.0;

    private:
        void updateFilter();

        juce::AudioProcessorValueTreeState& state;
        FilterBand&                         filter;
        juce::String                        prefix;
        const juce::CriticalSection&        callbackLock;

        std::atomic<FilterType> type   { NoFilter };
        std::atomic<float>  frequency  { 1000.0f };
        std::atomic<float>  gain       { 0.0f };
        std::atomic<float>  quality    { 1.0f };
        std::atomic<bool>   active     { true };

        AttachedValue<FilterType> typeAttachment;
        AttachedValue<float> frequencyAttachment;
        AttachedValue<float> gainAttachment;
        AttachedValue<float> qualityAttachment;
        AttachedValue<bool>  activeAttachment;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterAttachment)
    };

    //==============================================================================
    void parameterChanged (const juce::String& paramID, float newValue) override;
    void handleAsyncUpdate() override;

    /**
     In this override you create the GUI ValueTree either using the default or loading from the BinaryData::magic_xml
     */
    juce::ValueTree createGuiValueTree() override;

    void postSetStateInformation() override;

    //==============================================================================
    const juce::String getName() const override;

    double getTailLengthSeconds() const override;

    //==============================================================================
    static juce::StringArray filterNames;

private:
    //==============================================================================

    juce::AudioProcessorValueTreeState treeState { *this, nullptr };

    juce::dsp::ProcessorChain<FilterBand, FilterBand, FilterBand, FilterBand, FilterBand, FilterBand, Gain> filter;

    std::atomic<float> gain { 1.0f };
    AttachedValue<float> gainAttachment;

    FilterAttachment attachment1 { treeState, filter.get<0>(), "Q1", getCallbackLock() };
    FilterAttachment attachment2 { treeState, filter.get<1>(), "Q2", getCallbackLock() };
    FilterAttachment attachment3 { treeState, filter.get<2>(), "Q3", getCallbackLock() };
    FilterAttachment attachment4 { treeState, filter.get<3>(), "Q4", getCallbackLock() };
    FilterAttachment attachment5 { treeState, filter.get<4>(), "Q5", getCallbackLock() };
    FilterAttachment attachment6 { treeState, filter.get<5>(), "Q6", getCallbackLock() };

    std::array<FilterAttachment*, 6> attachments
    { &attachment1, &attachment2, &attachment3, &attachment4, &attachment5, &attachment6 };

    foleys::MagicPlotSource*  inputAnalyser  = nullptr;
    foleys::MagicPlotSource*  outputAnalyser = nullptr;

    foleys::MagicFilterPlot*  plotSum = nullptr;
    foleys::MagicLevelSource* outputMeter = nullptr;

    foleys::AtomicValueAttachment<bool> inputAnalysing;
    foleys::AtomicValueAttachment<bool> outputAnalysing;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizerExampleAudioProcessor)
};
