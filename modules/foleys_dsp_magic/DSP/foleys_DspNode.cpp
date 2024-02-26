//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_DspNode.h"

namespace foleys::dsp
{

DspNode::DspNode(const juce::ValueTree& node)
{

}

DspNode::~DspNode()
{
    masterReference.clear();
}



} // namespace foleys::dsp
