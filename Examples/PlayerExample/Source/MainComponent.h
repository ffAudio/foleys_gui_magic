/*
  ==============================================================================

    An example of an AudioPlayer app made with FoleysGuiMagic
    Note that this example is not a plugin, which was also important to show.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace foleys
{
class WaveformHolder;
}

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent
  : public juce::AudioAppComponent
  , public juce::Slider::Listener
  , private juce::Value::Listener
  , private juce::ChangeListener
  , private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    void loadFile (const juce::File& file);

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void valueChanged (juce::Value&) override;
    void sliderValueChanged (juce::Slider*) override;
    void changeListenerCallback (juce::ChangeBroadcaster*) override;
    void timerCallback() override;

    void updatePositionSlider();

    //==============================================================================
    // Your private member variables go here...

    juce::AudioFormatManager manager;
    juce::File               lastFolder = juce::File::getSpecialLocation (juce::File::userMusicDirectory);

    std::unique_ptr<juce::AudioFormatReaderSource> source;
    juce::AudioTransportSource                     transport;
    juce::Value                                    gainValue { 1.0f };

    juce::AudioThumbnailCache thumbnailCache { 64 };

    foleys::MagicGUIState   magicState;
    foleys::MagicGUIBuilder magicBuilder { magicState };
    foleys::WaveformHolder* audioThumbnail = nullptr;

    foleys::MagicPlotSource*  outputAnalyser { nullptr };
    foleys::MagicLevelSource* outputLevel { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
