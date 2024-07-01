//
// Created by Daniel Walz on 31.07.23.
//

#include "Waveform.h"

namespace foleys
{

// ================================================================================

WaveformHolder::WaveformHolder (juce::AudioThumbnailCache& cache, juce::AudioFormatManager& managerToUse) : formatManager (managerToUse), thumbnailCache (cache)
{
}

WaveformHolder::~WaveformHolder()
{
    masterReference.clear();
}

void WaveformHolder::setAudioFile (juce::File file)
{
    audioFile = file;

    sendChangeMessage();
}

juce::File WaveformHolder::getAudioFile() const
{
    return audioFile;
}

juce::AudioThumbnailCache& WaveformHolder::getCache()
{
    return thumbnailCache;
}

juce::AudioFormatManager& WaveformHolder::getManager()
{
    return formatManager;
}

// ================================================================================

WaveformDisplay::WaveformDisplay()
{
    setColour (ColourIds::waveformBackgroundColour, juce::Colours::transparentBlack);
    setColour (ColourIds::waveformForegroundColour, juce::Colours::orangered);
}

WaveformDisplay::~WaveformDisplay()
{
    if (audioThumb)
        audioThumb->removeChangeListener (this);
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    auto background = findColour (ColourIds::waveformBackgroundColour);
    auto foreground = findColour (ColourIds::waveformForegroundColour);

    if (!background.isTransparent())
    {
        g.fillAll (background);
    }

    g.setColour (foreground);

    if (thumbnail)
    {
        auto peak = thumbnail->getApproximatePeak();
        thumbnail->drawChannels (g, getLocalBounds().reduced (3), 0.0, thumbnail->getTotalLength(), peak > 0.0f ? 1.0f / peak : 1.0f);
    }
    else
        g.drawFittedText (TRANS ("No File"), getLocalBounds(), juce::Justification::centred, 1);
}

void WaveformDisplay::setAudioThumbnail (WaveformHolder* thumb)
{
    if (audioThumb)
        audioThumb->removeChangeListener (this);

    audioThumb = thumb;

    updateAudioFile();

    if (audioThumb)
        audioThumb->addChangeListener (this);
}

void WaveformDisplay::updateAudioFile()
{
    if (!audioThumb)
    {
        thumbnail.reset();
        return;
    }

    if (!audioThumb->getAudioFile().existsAsFile())
        return;

    thumbnail = std::make_unique<juce::AudioThumbnail> (256, audioThumb->getManager(), audioThumb->getCache());
    thumbnail->setSource (new juce::FileInputSource (audioThumb->getAudioFile()));
    thumbnail->addChangeListener (this);
}

void WaveformDisplay::changeListenerCallback ([[maybe_unused]] juce::ChangeBroadcaster* sender)
{
    if (sender == audioThumb)
        updateAudioFile();

    repaint();
}


// ================================================================================

WaveformItem::WaveformItem (MagicGUIBuilder& builder, const juce::ValueTree& node) : GuiItem (builder, node)
{
    setColourTranslation ({ { "waveform-background", WaveformDisplay::ColourIds::waveformBackgroundColour },
                            { "waveform-colour", WaveformDisplay::ColourIds::waveformForegroundColour } });

    addAndMakeVisible (waveformDisplay);
}

void WaveformItem::update()
{
    auto& state          = getMagicState();
    auto* audioThumbnail = state.getObjectWithType<WaveformHolder> ("Waveform");

    waveformDisplay.setAudioThumbnail (audioThumbnail);
}

// ================================================================================

}  // namespace foleys
