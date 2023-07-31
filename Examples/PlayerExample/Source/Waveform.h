//
// Created by Daniel Walz on 31.07.23.
//

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>


namespace foleys
{

// ================================================================================

/*!
 * @class AudioThumbnail
 * @brief An AudioThumbnail backend to be displayed in the GUI
 *
 * This serves as backend for an audio file to be displayed in the GUI.
 * The WaveformDisplay will react to a change of the file.
 */
class WaveformHolder : public juce::ChangeBroadcaster
{
public:
    WaveformHolder (juce::AudioThumbnailCache& cache, juce::AudioFormatManager& manager);
    ~WaveformHolder() override;

    /*!
     * Set the audiofile to display
     * @param file the audiofile
     */
    void       setAudioFile (juce::File file);

    /*!
     * @return the currently displayed audio file
     */
    juce::File getAudioFile() const;

    juce::AudioThumbnailCache& getCache();
    juce::AudioFormatManager&  getManager();

private:
    juce::AudioFormatManager&  formatManager;
    juce::AudioThumbnailCache& thumbnailCache;
    juce::File                 audioFile;

    JUCE_DECLARE_WEAK_REFERENCEABLE (WaveformHolder)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformHolder)
};

// ================================================================================

/*!
 * @class WaveformDisplay
 * @brief This widget displays an audio file using juce::AudioThumbnail
 */
class WaveformDisplay
  : public juce::Component
  , juce::ChangeListener
{
public:
    enum ColourIds
    {
        waveformBackgroundColour = 0x2002020,
        waveformForegroundColour
    };

    WaveformDisplay();
    ~WaveformDisplay() override;

    void paint (juce::Graphics& g) override;

    void setAudioThumbnail (WaveformHolder* thumb);
    void updateAudioFile();

    void changeListenerCallback ([[maybe_unused]] juce::ChangeBroadcaster* sender) override;

private:
    WaveformHolder*                       audioThumb = nullptr;
    std::unique_ptr<juce::AudioThumbnail> thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};

// ================================================================================

/*!
 * @class WaveformItem
 * @brief This is a wrapper class so the WaveformDisplay can be used in GuiMagic
 */
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
