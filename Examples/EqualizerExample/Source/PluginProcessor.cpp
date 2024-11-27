/*
  ==============================================================================

    A simple equalizer using the JUCE dsp module

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================

namespace IDs
{
static juce::String paramOutput { "output" };
static juce::String paramType { "type" };
static juce::String paramFreq { "freq" };
static juce::String paramGain { "gain" };
static juce::String paramQuality { "quality" };
static juce::String paramActive { "active" };
}  // namespace IDs

juce::StringArray filterNames = { NEEDS_TRANS ("No filter"), NEEDS_TRANS ("High pass"),  NEEDS_TRANS ("1st order high pass"),
                                  NEEDS_TRANS ("Low shelf"), NEEDS_TRANS ("Band pass"),  NEEDS_TRANS ("Notch"),
                                  NEEDS_TRANS ("Peak"),      NEEDS_TRANS ("High shelf"), NEEDS_TRANS ("1st order low pass"),
                                  NEEDS_TRANS ("Low pass") };

static float maxLevel = 24.0f;

static std::unique_ptr<juce::AudioProcessorParameterGroup> createParametersForFilter (const juce::String& prefix, const juce::String& name,
                                                                                      EqualizerExampleAudioProcessor::FilterType type, float frequency,
                                                                                      float gain = 0.0f, float quality = 1.0f, bool active = true)
{
    auto typeParameter =
      std::make_unique<juce::AudioParameterChoice> (juce::ParameterID (prefix + IDs::paramType, 1), name + ": " + TRANS ("Filter Type"), filterNames, type);

    auto actvParameter = std::make_unique<juce::AudioParameterBool> (
      juce::ParameterID (prefix + IDs::paramActive, 1), name + ": " + TRANS ("Active"), active, juce::String(),
      [] (float value, int) { return value > 0.5f ? TRANS ("active") : TRANS ("bypassed"); }, [] (juce::String text) { return text == TRANS ("active"); });

    auto freqParameter = std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID (prefix + IDs::paramFreq, 1), name + ": " + TRANS ("Frequency"), foleys::Conversions::makeLogarithmicRange<float> (20.0f, 20000.0f),
      frequency, juce::String(), juce::AudioProcessorParameter::genericParameter,
      [] (float value, int) { return (value < 1000) ? juce::String (value, 0) + " Hz" : juce::String (value / 1000.0) + " kHz"; },
      [] (juce::String text) { return text.endsWith (" kHz") ? text.getFloatValue() * 1000.0f : text.getFloatValue(); });

    auto qltyParameter = std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID (prefix + IDs::paramQuality, 1), name + ": " + TRANS ("Quality"),
      juce::NormalisableRange<float> { 0.1f, 10.0f, 0.1f, std::log (0.5f) / std::log (0.9f / 9.9f) }, quality, juce::String(), juce::AudioProcessorParameter::genericParameter,
      [] (float value, int) { return juce::String (value, 1); }, [] (const juce::String& text) { return text.getFloatValue(); });

    auto gainParameter = std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID (prefix + IDs::paramGain, 1), name + ": " + TRANS ("Gain"), juce::NormalisableRange<float> { -maxLevel, maxLevel, 0.1f }, gain,
      juce::String(), juce::AudioProcessorParameter::genericParameter, [] (float value, int) { return juce::String (value, 1) + " dB"; },
      [] (juce::String text) { return text.getFloatValue(); });

    auto group = std::make_unique<juce::AudioProcessorParameterGroup> ("band" + prefix, name, "|", std::move (typeParameter), std::move (actvParameter),
                                                                       std::move (freqParameter), std::move (qltyParameter), std::move (gainParameter));

    return group;
}

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::AudioProcessorParameterGroup>> params;

    params.push_back (createParametersForFilter ("Q1", NEEDS_TRANS ("Q1"), EqualizerExampleAudioProcessor::HighPass, 40.0f));
    params.push_back (createParametersForFilter ("Q2", NEEDS_TRANS ("Q2"), EqualizerExampleAudioProcessor::LowShelf, 250.0f));
    params.push_back (createParametersForFilter ("Q3", NEEDS_TRANS ("Q3"), EqualizerExampleAudioProcessor::Peak, 500.0f));
    params.push_back (createParametersForFilter ("Q4", NEEDS_TRANS ("Q4"), EqualizerExampleAudioProcessor::Peak, 1000.0f));
    params.push_back (createParametersForFilter ("Q5", NEEDS_TRANS ("Q5"), EqualizerExampleAudioProcessor::HighShelf, 5000.0f));
    params.push_back (createParametersForFilter ("Q6", NEEDS_TRANS ("Q6"), EqualizerExampleAudioProcessor::LowPass, 12000.0f));

    auto param = std::make_unique<juce::AudioParameterFloat> (
      juce::ParameterID (IDs::paramOutput, 1), TRANS ("Output"), juce::NormalisableRange<float> (0.0f, 2.0f, 0.01f), 1.0f, juce::String(),
      juce::AudioProcessorParameter::genericParameter, [] (float value, int) { return juce::String (juce::Decibels::gainToDecibels (value), 1) + " dB"; },
      [] (juce::String text) { return juce::Decibels::decibelsToGain (text.getFloatValue()); });

    auto group = std::make_unique<juce::AudioProcessorParameterGroup> ("global", TRANS ("Globals"), "|", std::move (param));
    params.push_back (std::move (group));

    return { params.begin(), params.end() };
}

static auto createPostUpdateLambda (foleys::MagicProcessorState& magicState, const juce::String& plotID)
{
    return [plot = magicState.getObjectWithType<foleys::MagicFilterPlot> (plotID)] (const EqualizerExampleAudioProcessor::FilterAttachment& a)
    {
        if (plot != nullptr)
        {
            plot->setIIRCoefficients (a.coefficients, maxLevel);
            plot->setActive (a.isActive());
        }
    };
}

//==============================================================================
EqualizerExampleAudioProcessor::EqualizerExampleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  : MagicProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                      .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                      ),
#else
  :
#endif
    treeState (*this, nullptr, JucePlugin_Name, createParameterLayout()),
    gainAttachment (treeState, gain, IDs::paramOutput)
{
    FOLEYS_SET_SOURCE_PATH (__FILE__);

    // GUI MAGIC: add plots to be displayed in the GUI
    for (size_t i = 0; i < attachments.size(); ++i)
    {
        auto name = "plot" + juce::String (i + 1);
        magicState.createAndAddObject<foleys::MagicFilterPlot> (name);
        attachments.at (i)->postFilterUpdate = createPostUpdateLambda (magicState, name);
    }

    plotSum = magicState.createAndAddObject<foleys::MagicFilterPlot> ("plotSum");

    // GUI MAGIC: add analyser plots
    inputAnalyser  = magicState.createAndAddObject<foleys::MagicAnalyser> ("input");
    outputAnalyser = magicState.createAndAddObject<foleys::MagicAnalyser> ("output");

    // MAGIC GUI: add a meter at the output
    outputMeter = magicState.createAndAddObject<foleys::MagicLevelSource> ("outputMeter");

    for (auto* parameter: getParameters())
        if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*> (parameter))
            treeState.addParameterListener (p->paramID, this);

    // MAGIC GUI: add properties to connect visibility to
    magicState.getPropertyAsValue ("analyser:input").setValue (true);
    magicState.getPropertyAsValue ("analyser:output").setValue (true);

    inputAnalysing.attachToValue (magicState.getPropertyAsValue ("analyser:input"));
    outputAnalysing.attachToValue (magicState.getPropertyAsValue ("analyser:output"));
}

EqualizerExampleAudioProcessor::~EqualizerExampleAudioProcessor()
{
    for (auto* parameter: getParameters())
        if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*> (parameter))
            treeState.removeParameterListener (p->paramID, this);
}

//==============================================================================
void EqualizerExampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const auto numChannels = getTotalNumOutputChannels();

    // GUI MAGIC: call this to set up the visualisers
    magicState.prepareToPlay (sampleRate, samplesPerBlock);
    outputMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = juce::uint32 (samplesPerBlock);
    spec.numChannels      = juce::uint32 (numChannels);

    filter.get<6>().setGainLinear (*treeState.getRawParameterValue (IDs::paramOutput));

    for (auto* a: attachments)
        a->setSampleRate (sampleRate);

    filter.prepare (spec);
}

void EqualizerExampleAudioProcessor::releaseResources() { }

#ifndef JucePlugin_PreferredChannelConfigurations
bool EqualizerExampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
    #else

        // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

void EqualizerExampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    ignoreUnused (midiMessages);

    filter.setBypassed<0> (attachment1.isActive() == false);
    filter.setBypassed<1> (attachment2.isActive() == false);
    filter.setBypassed<2> (attachment3.isActive() == false);
    filter.setBypassed<3> (attachment4.isActive() == false);
    filter.setBypassed<4> (attachment5.isActive() == false);
    filter.setBypassed<5> (attachment6.isActive() == false);

    filter.get<6>().setGainLinear (gain);

    // GUI MAGIC: measure before processing
    if (inputAnalysing.get())
        inputAnalyser->pushSamples (buffer);

    juce::dsp::AudioBlock<float>              ioBuffer (buffer);
    juce::dsp::ProcessContextReplacing<float> context (ioBuffer);
    filter.process (context);

    // GUI MAGIC: measure after processing
    if (outputAnalysing.get())
        outputAnalyser->pushSamples (buffer);

    outputMeter->pushSamples (buffer);
}

//==============================================================================

EqualizerExampleAudioProcessor::FilterAttachment::FilterAttachment (juce::AudioProcessorValueTreeState& stateToUse, FilterBand& filterToControl,
                                                                    const juce::String& prefixToUse, const juce::CriticalSection& lock)
  : state (stateToUse),
    filter (filterToControl),
    prefix (prefixToUse),
    callbackLock (lock),
    typeAttachment (state, type, prefix + IDs::paramType, [&] { updateFilter(); }),
    frequencyAttachment (state, frequency, prefix + IDs::paramFreq, [&] { updateFilter(); }),
    gainAttachment (state, gain, prefix + IDs::paramGain, [&] { updateFilter(); }),
    qualityAttachment (state, quality, prefix + IDs::paramQuality, [&] { updateFilter(); }),
    activeAttachment (state, active, prefix + IDs::paramActive,
                      [&]
                      {
                          if (postFilterUpdate)
                              postFilterUpdate (*this);
                      })
{
    updateFilter();
}

void EqualizerExampleAudioProcessor::FilterAttachment::updateFilter()
{
    if (sampleRate < 20.0)
        return;

    switch (type)
    {
        case NoFilter: coefficients = new juce::dsp::IIR::Coefficients<float> (1, 0, 1, 0); break;
        case LowPass: coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate, frequency, quality); break;
        case LowPass1st: coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass (sampleRate, frequency); break;
        case LowShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf (sampleRate, frequency, quality, juce::Decibels::decibelsToGain (gain.load()));
            break;
        case BandPass: coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass (sampleRate, frequency, quality); break;
        case Notch: coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch (sampleRate, frequency, quality); break;
        case Peak:
            coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter (sampleRate, frequency, quality, juce::Decibels::decibelsToGain (gain.load()));
            break;
        case HighShelf:
            coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf (sampleRate, frequency, quality, juce::Decibels::decibelsToGain (gain.load()));
            break;
        case HighPass1st: coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (sampleRate, frequency); break;
        case HighPass: coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, frequency, quality); break;
        case LastFilterID:
        default: return;
    }

    {
        juce::ScopedLock processLock (callbackLock);
        *filter.state = *coefficients;
    }

    if (postFilterUpdate)
        postFilterUpdate (*this);
}

void EqualizerExampleAudioProcessor::FilterAttachment::setSampleRate (double sampleRateToUse)
{
    sampleRate = sampleRateToUse;
    updateFilter();
}

//==============================================================================

template<typename ValueType>
AttachedValue<ValueType>::AttachedValue (juce::AudioProcessorValueTreeState& stateToUse, std::atomic<ValueType>& valueToUse, const juce::String& paramToUse,
                                         std::function<void()> changedLambda)
  : state (stateToUse), value (valueToUse), paramID (paramToUse), onParameterChanged (changedLambda)
{
    // Oh uh, tried to attach to a non existing parameter
    jassert (state.getParameter (paramID) != nullptr);

    initialUpdate();
    state.addParameterListener (paramID, this);
}

template<typename ValueType>
AttachedValue<ValueType>::~AttachedValue()
{
    state.removeParameterListener (paramID, this);
}

template<typename ValueType>
void AttachedValue<ValueType>::initialUpdate()
{
    value = ValueType (*state.getRawParameterValue (paramID));
}

template<>
void AttachedValue<EqualizerExampleAudioProcessor::FilterType>::initialUpdate()
{
    value = EqualizerExampleAudioProcessor::FilterType (juce::roundToInt (state.getRawParameterValue (paramID)->load()));
}

template<typename ValueType>
void AttachedValue<ValueType>::parameterChanged (const juce::String&, float newValue)
{
    value = newValue;
    if (onParameterChanged)
        onParameterChanged();
}

template<>
void AttachedValue<bool>::parameterChanged (const juce::String&, float newValue)
{
    value = (newValue > 0.5f);
    if (onParameterChanged)
        onParameterChanged();
}

template<>
void AttachedValue<EqualizerExampleAudioProcessor::FilterType>::parameterChanged (const juce::String&, float newValue)
{
    value = EqualizerExampleAudioProcessor::FilterType (juce::roundToInt (newValue));
    if (onParameterChanged)
        onParameterChanged();
}

//==============================================================================
void EqualizerExampleAudioProcessor::parameterChanged (const juce::String&, float)
{
    triggerAsyncUpdate();
}

void EqualizerExampleAudioProcessor::handleAsyncUpdate()
{
    std::vector<juce::dsp::IIR::Coefficients<float>::Ptr> coefficients;
    for (auto* a: attachments)
        if (a->isActive())
            coefficients.push_back (a->coefficients);

    plotSum->setIIRCoefficients (gain, coefficients, maxLevel);
}

//==============================================================================

juce::ValueTree EqualizerExampleAudioProcessor::createGuiValueTree()
{
    juce::String text (BinaryData::magic_xml, BinaryData::magic_xmlSize);
    return juce::ValueTree::fromXml (text);
}

void EqualizerExampleAudioProcessor::postSetStateInformation()
{
    // MAGIC GUI: let the magicState conveniently handle save and restore the state.
    //            You don't need to use that, but it also takes care of restoring the last editor size
    inputAnalysing.attachToValue (magicState.getPropertyAsValue ("analyser:input"));
    outputAnalysing.attachToValue (magicState.getPropertyAsValue ("analyser:output"));
}

//==============================================================================
const juce::String EqualizerExampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

double EqualizerExampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EqualizerExampleAudioProcessor();
}
