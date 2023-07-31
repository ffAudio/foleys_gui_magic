//
// Created by Daniel Walz on 31.07.23.
//

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>


namespace foleys
{

// ================================================================================

class AudioThumbnail : public juce::ChangeBroadcaster
{
public:
    AudioThumbnail (juce::AudioThumbnailCache& cache, juce::AudioFormatManager& manager);
    ~AudioThumbnail() override;

    void       setAudioFile (juce::File file);
    juce::File getAudioFile() const;

    juce::AudioThumbnailCache& getCache();
    juce::AudioFormatManager&  getManager();

private:
    juce::AudioFormatManager&  formatManager;
    juce::AudioThumbnailCache& thumbnailCache;
    juce::File                 audioFile;

    JUCE_DECLARE_WEAK_REFERENCEABLE (AudioThumbnail)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioThumbnail)
};

// ================================================================================

class WaveformDisplay
  : public juce::Component
  , juce::ChangeListener
{
public:
    WaveformDisplay() = default;
    ~WaveformDisplay() override;

    void paint (juce::Graphics& g) override;

    void setAudioThumbnail (AudioThumbnail* thumb);
    void updateAudioFile();

    void changeListenerCallback ([[maybe_unused]] juce::ChangeBroadcaster* sender) override;

private:
    AudioThumbnail*                       audioThumb = nullptr;
    std::unique_ptr<juce::AudioThumbnail> thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};

// ================================================================================

class WaveformItem : public GuiItem
{
public:
    WaveformItem (MagicGUIBuilder& builder, const juce::ValueTree& node);

    void             update() override;
    juce::Component* getWrappedComponent() override { return &waveformDisplay; }
    FOLEYS_DECLARE_GUI_FACTORY (WaveformItem)
private:
    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformItem)
};

// ================================================================================

}  // namespace foleys
