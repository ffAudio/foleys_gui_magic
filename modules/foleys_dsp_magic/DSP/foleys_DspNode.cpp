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
}  // namespace NodeIDs

DspNode::DspNode (DspProgram& program, const juce::ValueTree& node) : config (node)
{
    nodeType = node.getType().toString();
}

DspNode::~DspNode()
{
    masterReference.clear();
}

/* static */
int DspNode::getUID (const juce::ValueTree& tree)
{
    return tree.getProperty (NodeIDs::uid, 0);
}

juce::String DspNode::getName() const
{
    return config.getProperty (NodeIDs::name, "unnamed").toString();
}

int DspNode::getUID() const
{
    return config.getProperty (NodeIDs::uid, 0);
}

void DspNode::setName (const juce::String& newName)
{
    config.setProperty (NodeIDs::name, newName, nullptr);
}

void DspNode::setUID (int newUID)
{
    config.setProperty (NodeIDs::uid, newUID, nullptr);
}


}  // namespace foleys::dsp
