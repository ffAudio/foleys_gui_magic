//
// Created by Daniel Walz on 11.04.24.
//

#pragma once

namespace foleys::dsp
{


namespace Parameters
{

enum ParameterType
{
    Invalid = 0,
    Group,
    Float,
    Boolean,
    Choice,
    Integer,
    Separator
};

/**
 * Create a parameter to add in the parameter tree
 * @param type
 * @return a ValueTree
 */
juce::ValueTree createNode (ParameterType type);

ParameterType getParameterType (const juce::ValueTree& node);

juce::String getParameterDescription (const juce::ValueTree& node);

juce::Array<juce::PropertyComponent*> createPropertyEditors (juce::ValueTree node, juce::UndoManager* undo);

/**
 * Restore a node from a ValueTree into an AudioProcessorParameterGroup
 * @param group the AudioProcessorParameterGroup to add the restored node to
 * @param tree the definition of the parameter
 */
void restoreParameterNode (juce::AudioProcessorParameterGroup* group, juce::ValueTree tree);

/**
 * Restore a whole parameterTree to the root AudioProcessorParameterGroup
 * @param root the root to populate
 * @param tree the ValueTree of parameters and parameterGroups
 */
void restoreParameterTree (juce::AudioProcessorParameterGroup* root, juce::ValueTree tree);


}  // namespace Parameters

}  // namespace foleys::dsp
