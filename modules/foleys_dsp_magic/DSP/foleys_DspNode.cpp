//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspNode.h"

namespace foleys::dsp
{

namespace NodeIDs
{
DECLARE_ID (name)
DECLARE_ID (uid)
}

DspNode::DspNode (MagicDspBuilder& builder, const juce::ValueTree& node) : config (node)
{
    nodeName = node.getProperty (NodeIDs::name, "noname");
    nodeType = node.getType().toString();
    if (config.hasProperty(NodeIDs::uid))
    {
        uid = config.getProperty(NodeIDs::uid);
        builder.setNextUID(uid);
    }
    else
    {
        uid = builder.nextUID();
        config.setProperty(NodeIDs::uid, uid, nullptr);
    }
}

DspNode::~DspNode()
{
    masterReference.clear();
}


}  // namespace foleys::dsp
