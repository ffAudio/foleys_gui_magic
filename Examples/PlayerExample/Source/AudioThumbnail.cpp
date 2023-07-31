//
// Created by Daniel Walz on 31.07.23.
//

#include "AudioThumbnail.h"

namespace foleys
{

// ================================================================================

AudioThumbnail::AudioThumbnail (juce::AudioThumbnailCache& cache, juce::AudioFormatManager& managerToUse) : formatManager (managerToUse), thumbnailCache (cache)
{
}

AudioThumbnail::~AudioThumbnail()
{
    masterReference.clear();
}

void AudioThumbnail::setAudioFile (juce::File file)
{
    audioFile = file;

    sendChangeMessage();
}

juce::File AudioThumbnail::getAudioFile() const
{
    return audioFile;
}

juce::AudioThumbnailCache& AudioThumbnail::getCache()
{
    return thumbnailCache;
}

juce::AudioFormatManager& AudioThumbnail::getManager()
{
    return formatManager;
}

// ================================================================================

WaveformDisplay::~WaveformDisplay()
{
    if (audioThumb)
        audioThumb->removeChangeListener (this);
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::red);

    if (thumbnail)
    {
        auto peak = thumbnail->getApproximatePeak();
        thumbnail->drawChannels (g, getLocalBounds().reduced (3), 0.0, thumbnail->getTotalLength(), peak > 0.0f ? 1.0f / peak : 1.0f);
    }
    else
        g.drawFittedText (TRANS ("No File"), getLocalBounds(), juce::Justification::centred, 1);
}

void WaveformDisplay::setAudioThumbnail (AudioThumbnail* thumb)
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
    addAndMakeVisible (waveformDisplay);
}

void WaveformItem::update()
{
    auto& state          = getMagicState();
    auto* audioThumbnail = state.getObjectWithType<AudioThumbnail> ("Waveform");

    waveformDisplay.setAudioThumbnail (audioThumbnail);
}

// ================================================================================

}  // namespace foleys
