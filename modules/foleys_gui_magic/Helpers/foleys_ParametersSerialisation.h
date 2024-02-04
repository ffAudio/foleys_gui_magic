//
// Created by Daniel Walz on 03.02.24.
//

#pragma once

namespace foleys::ParametersSerialisation
{

namespace IDs
{
static constexpr auto* parameters   = "Parameters";
static constexpr auto* parameter    = "Parameter";
static constexpr auto* paramID      = "ParamID";
static constexpr auto* paramName    = "ParamName";
static constexpr auto* paramType    = "ParamType";
static constexpr auto* typeFloat    = "Float";
static constexpr auto* typeChoice   = "Choice";
static constexpr auto* choiceName   = "Name";
static constexpr auto* typeBoolean  = "Boolean";
static constexpr auto* paramMin     = "ParamMin";
static constexpr auto* paramMax     = "ParamMax";
static constexpr auto* paramStep    = "ParamStep";
static constexpr auto* paramDefault = "Default";
static constexpr auto* group        = "Group";
static constexpr auto* groupID      = "GroupID";
static constexpr auto* groupName    = "GroupName";
static constexpr auto* separator    = "Separator";
}  // namespace IDs

/**
 * This is for developing purposes serialise an existing parameter or group
 * @param tree
 * @param node
 */
static inline void saveParameterNode (juce::ValueTree tree, const juce::AudioProcessorParameterGroup::AudioProcessorParameterNode* node)
{
    if (auto* parameter = node->getParameter())
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
            tree.appendChild ({ IDs::parameter,
                                { { IDs::paramID, ranged->getParameterID() },
                                  { IDs::paramName, ranged->getName (255) },
                                  { IDs::paramMin, ranged->getNormalisableRange().start },
                                  { IDs::paramMax, ranged->getNormalisableRange().end },
                                  { IDs::paramStep, ranged->getNormalisableRange().interval } } },
                              nullptr);
    }
    else
    {
        if (auto* group = node->getGroup())
        {
            juce::ValueTree child { IDs::group, { { IDs::groupID, group->getID() }, { IDs::groupName, group->getName() }, { IDs::separator, group->getSeparator() } } };

            for (auto* childNode: *group)
                saveParameterNode (child, childNode);

            tree.appendChild (child, nullptr);
        }
    }
}

/**
 * This is for developing purposes serialise an existing parameter layout
 * @param parameterTree the current parameterTree to serialise
 * @return a ValueTree that can be used to populate an AudioProcessor
 */
static inline juce::ValueTree saveParameterTree (const juce::AudioProcessorParameterGroup& parameterTree)
{
    juce::ValueTree tree { IDs::parameters };

    for (auto* node: parameterTree)
        saveParameterNode (tree, node);

    return tree;
}

/**
 * Restore a node from a ValueTree into an AudioProcessorParameterGroup
 * @param group the AudioProcessorParameterGroup to add the restored node to
 * @param tree the definition of the parameter
 */
static inline void restoreParameterNode (juce::AudioProcessorParameterGroup* group, juce::ValueTree tree)
{
    if (tree.hasType (IDs::parameter))
    {
        auto type = tree.getProperty (IDs::paramType, IDs::typeFloat).toString();
        if (type.equalsIgnoreCase (IDs::typeBoolean))
            group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID (tree.getProperty (IDs::paramID), 1),
                                                                         tree.getProperty (IDs::paramName), tree.getProperty (IDs::paramDefault, false)));
        else if (type.equalsIgnoreCase (IDs::typeChoice))
        {
            juce::StringArray choices;
            for (const auto& item: tree)
                if (item.hasType (IDs::typeChoice))
                    choices.add (item.getProperty (IDs::choiceName));

            group->addChild (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID (tree.getProperty (IDs::paramID), 1),
                                                                           tree.getProperty (IDs::paramName), choices, tree.getProperty (IDs::paramDefault, 0)));
        }
        else
            group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID (tree.getProperty (IDs::paramID), 1), tree.getProperty (IDs::paramName),
                                                                          juce::NormalisableRange<float> (tree.getProperty (IDs::paramMin), tree.getProperty (IDs::paramMax),
                                                                                                          tree.getProperty (IDs::paramStep)),
                                                                          tree.getProperty (IDs::paramDefault, tree.getProperty (IDs::paramMin))));
    }
    else if (tree.hasType (IDs::group))
    {
        auto childGroup = std::make_unique<juce::AudioProcessorParameterGroup> (tree.getProperty (IDs::groupID), tree.getProperty (IDs::groupName),
                                                                                tree.getProperty (IDs::separator));

        for (const auto& child: tree)
            restoreParameterNode (childGroup.get(), child);

        return group->addChild (std::move (childGroup));
    }
}

/**
 * Restore a whole parameterTree to the root AudioProcessorParameterGroup
 * @param root the root to populate
 * @param tree the ValueTree of parameters and parameterGroups
 */
static inline void restoreParameterTree (juce::AudioProcessorParameterGroup* root, juce::ValueTree tree)
{
    auto paramNode = tree.getChildWithName (IDs::parameters);

    if (paramNode.isValid())
    {
        for (const auto& node: paramNode)
        {
            restoreParameterNode (root, node);
        }
    }
}


}  // namespace foleys::ParametersSerialisation
