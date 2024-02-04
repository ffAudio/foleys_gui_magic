
#include "BinaryData.h"
#include <foleys_dsp_magic/foleys_dsp_magic.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new foleys::PluginProcessor (BinaryData::magic_xml, BinaryData::magic_xmlSize);
}
