
#include "BinaryData.h"
#include <foleys_gui_magic/foleys_gui_magic.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new foleys::NoCodeProcessor (BinaryData::magic_xml, BinaryData::magic_xmlSize);
}
