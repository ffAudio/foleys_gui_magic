//
// Created by Daniel Walz on 11.04.24.
//

#include "foleys_PluginParameters.h"


namespace foleys::dsp
{


namespace Parameters
{

namespace IDs
{
static constexpr auto* parameters    = "Parameters";
static constexpr auto* parameter     = "Parameter";
static constexpr auto* paramID       = "paramID";
static constexpr auto* paramName     = "paramName";
static constexpr auto* paramType     = "paramType";
static constexpr auto* typeFloat     = "float";
static constexpr auto* typeChoice    = "choice";
static constexpr auto* typeInt       = "integer";
static constexpr auto* typeSeparator = "separator";
static constexpr auto* choices       = "choices";
static constexpr auto* typeBoolean   = "boolean";
static constexpr auto* paramMin      = "paramMin";
static constexpr auto* paramMax      = "paramMax";
static constexpr auto* paramStep     = "paramStep";
static constexpr auto* paramDefault  = "default";
static constexpr auto* group         = "Group";
static constexpr auto* groupID       = "groupID";
static constexpr auto* groupName     = "groupName";
static constexpr auto* separator     = "Separator";
}  // namespace IDs


juce::ValueTree createNode (ParameterType type)
{
    switch (type)
    {
        case ParameterType::Group: return juce::ValueTree (IDs::group, { { IDs::paramType, IDs::group } });
        case ParameterType::Float: return juce::ValueTree (IDs::parameter, { { IDs::paramType, IDs::typeFloat } });
        case ParameterType::Choice: return juce::ValueTree (IDs::parameter, { { IDs::paramType, IDs::typeChoice } });
        case ParameterType::Integer: return juce::ValueTree (IDs::parameter, { { IDs::paramType, IDs::typeInt } });
        case ParameterType::Boolean: return juce::ValueTree (IDs::parameter, { { IDs::paramType, IDs::typeBoolean } });
        case ParameterType::Separator: return juce::ValueTree (IDs::parameter, { { IDs::paramType, IDs::typeSeparator } });
        default: return {};
    }
}

ParameterType getParameterType (const juce::ValueTree& node)
{
    if (node.hasType (IDs::group))
        return ParameterType::Group;

    auto typeName = node.getProperty (IDs::paramType).toString();
    if (typeName.equalsIgnoreCase (IDs::typeFloat))
        return ParameterType::Float;

    if (typeName.equalsIgnoreCase (IDs::typeInt))
        return ParameterType::Integer;

    if (typeName.equalsIgnoreCase (IDs::typeBoolean))
        return ParameterType::Boolean;

    if (typeName.equalsIgnoreCase (IDs::typeChoice))
        return ParameterType::Choice;

    if (typeName.equalsIgnoreCase (IDs::typeSeparator))
        return ParameterType::Separator;

    return ParameterType::Invalid;
}

juce::String getParameterDescription (const juce::ValueTree& node)
{
    if (node.hasType (IDs::parameters))
        return TRANS ("Parameters (root)");

    if (node.hasType (IDs::group))
        return TRANS ("Group: ") + node.getProperty (IDs::groupName).toString();

    auto typeName = node.getProperty (IDs::paramType).toString();

    if (typeName.equalsIgnoreCase (IDs::separator))
        return "------------";

    return TRANS ("Parameter ") + typeName + ": " + node.getProperty (IDs::paramName).toString();
}

juce::Array<juce::PropertyComponent*> createPropertyEditors (juce::ValueTree node, juce::UndoManager* undo)
{
    auto type = getParameterType (node);

    if (type == ParameterType::Separator || type == ParameterType::Invalid)
        return {};

    juce::Array<juce::PropertyComponent*> comps;

    if (type == ParameterType::Group)
    {
        comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::groupID, undo), TRANS ("Group ID"), 64, false));
        comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::groupName, undo), TRANS ("Group Name"), 64, false));
        return comps;
    }

    comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramID, undo), TRANS ("Parameter ID"), 64, false));
    comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramName, undo), TRANS ("Parameter Name"), 64, false));

    switch (type)
    {
        case ParameterType::Float:
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramMin, undo), TRANS ("Min Value"), 32, false));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramMax, undo), TRANS ("Max Value"), 32, false));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramStep, undo), TRANS ("Interval"), 32, false));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramDefault, undo), TRANS ("Default Value"), 32, false));
            break;
        case ParameterType::Choice:
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::choices, undo), TRANS ("Choices"), 512, true));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramDefault, undo), TRANS ("Default Value"), 32, false));
            break;
        case ParameterType::Integer:
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramMin, undo), TRANS ("Min Value"), 32, false));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramMax, undo), TRANS ("Max Value"), 32, false));
            comps.add (new juce::TextPropertyComponent (node.getPropertyAsValue (IDs::paramDefault, undo), TRANS ("Default Value"), 32, false));
            break;
        case ParameterType::Boolean:
            comps.add (new juce::BooleanPropertyComponent (node.getPropertyAsValue (IDs::paramDefault, undo), TRANS ("Default Value"), TRANS ("On")));
            break;
        default: break;
    }

    return comps;
}


/**
 * Restore a node from a ValueTree into an AudioProcessorParameterGroup
 * @param group the AudioProcessorParameterGroup to add the restored node to
 * @param tree the definition of the parameter
 */
void restoreParameterNode (juce::AudioProcessorParameterGroup* group, juce::ValueTree tree)
{
    if (tree.hasType (IDs::parameter))
    {
        auto type = tree.getProperty (IDs::paramType, IDs::typeFloat).toString();
        if (type.equalsIgnoreCase (IDs::typeBoolean))
            group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID (tree.getProperty (IDs::paramID), 1),
                                                                         tree.getProperty (IDs::paramName), tree.getProperty (IDs::paramDefault, false)));
        else if (type.equalsIgnoreCase (IDs::typeChoice))
        {
            juce::StringArray choices = juce::StringArray::fromTokens (tree.getProperty (IDs::choices).toString(), ';');

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
void restoreParameterTree (juce::AudioProcessorParameterGroup* root, juce::ValueTree tree)
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

/**
 * This is for developing purposes serialise an existing parameter or group
 * @param tree
 * @param node
 */
void saveParameterNode (juce::ValueTree tree, const juce::AudioProcessorParameterGroup::AudioProcessorParameterNode* node)
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
juce::ValueTree saveParameterTree (const juce::AudioProcessorParameterGroup& parameterTree)
{
    juce::ValueTree tree { IDs::parameters };

    for (auto* node: parameterTree)
        saveParameterNode (tree, node);

    return tree;
}

}  // namespace Parameters

}  // namespace foleys::dsp
