/*
 ==============================================================================
    Copyright (c) 2019 Foleys Finest Audio Ltd. - Daniel Walz
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 */

namespace foleys
{

namespace IDs
{
    static juce::Identifier magic        { "magic" };
    static juce::Identifier styles       { "Styles"  };
    static juce::Identifier style        { "Style"   };
    static juce::Identifier div          { "Div" };
    static juce::Identifier slider       { "Slider" };
    static juce::Identifier textButton   { "TextButton" };
    static juce::Identifier toggleButton { "ToggleButton" };
    static juce::Identifier comboBox     { "ComboBox" };

    static juce::Identifier caption      { "caption" };
    static juce::Identifier lookAndFeel  { "lookAndFeel" };
    static juce::Identifier parameter    { "parameter" };

    static juce::Identifier name         { "name"     };
    static juce::Identifier selected     { "selected" };
    static juce::Identifier id           { "id" };
    static juce::Identifier styleClass   { "class" };

    static juce::Identifier backgroundColour  { "backgroundColour" };
    static juce::Identifier borderColour      { "borderColour" };
    static juce::Identifier border       { "border" };
    static juce::Identifier margin       { "margin" };
    static juce::Identifier padding      { "padding" };

    static juce::String     root         { "root" };

}


MagicBuilder::MagicBuilder (juce::Component& parentToUse)
  : parent (parentToUse)
{
}

Stylesheet& MagicBuilder::getStylesheet()
{
    return stylesheet;
}

juce::ValueTree& MagicBuilder::getGuiTree()
{
    return config;
}

void MagicBuilder::updateAll()
{
    updateStylesheet();
    updateComponents();
    updateLayout();
}

void MagicBuilder::updateStylesheet()
{
    auto stylesNode = config.getOrCreateChildWithName (IDs::styles, &undo);
    if (stylesNode.getNumChildren() == 0)
        stylesNode.appendChild (Stylesheet::createDefaultStyle(), &undo);

    auto selectedName = stylesNode.getProperty (IDs::selected, {}).toString();
    if (selectedName.isNotEmpty())
    {
        auto style = stylesNode.getChildWithProperty (IDs::name, selectedName);
        stylesheet.setStyle (style);
    }
    else
    {
        stylesheet.setStyle (stylesNode.getChild (0));
    }
}

void MagicBuilder::restoreGUI (const juce::ValueTree& gui)
{
    if (gui.isValid() == false)
        return;

    config = gui;

    updateAll();
}

void MagicBuilder::updateComponents()
{
    createDefaultGUITree (true);

    auto rootNode = config.getOrCreateChildWithName (IDs::div, &undo);
    root = restoreNode (parent, rootNode);
}

void MagicBuilder::updateLayout()
{
    if (root.get() != nullptr)
        root->setBounds (parent.getLocalBounds());
}

void MagicBuilder::registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel)
{
    if (lookAndFeels.find (name) != lookAndFeels.cend())
    {
        // You tried to register more than one LookAndFeel with the same name!
        // That cannot work, the second LookAndFeel will be ignored
        jassertfalse;
        return;
    }

    lookAndFeels [name] = std::move (lookAndFeel);
}

void MagicBuilder::registerJUCELookAndFeels()
{
    registerLookAndFeel ("LookAndFeel_V1", std::make_unique<juce::LookAndFeel_V1>());
    registerLookAndFeel ("LookAndFeel_V2", std::make_unique<juce::LookAndFeel_V2>());
    registerLookAndFeel ("LookAndFeel_V3", std::make_unique<juce::LookAndFeel_V3>());
    registerLookAndFeel ("LookAndFeel_V4", std::make_unique<juce::LookAndFeel_V4>());
}


template <class AppType>
MagicGUIBuilder<AppType>::MagicGUIBuilder (juce::Component& parentToUse, AppType& appToUse, MagicProcessorState* magicStateToUse)
  : MagicBuilder (parentToUse),
    app (appToUse),
    magicState (magicStateToUse)
{
    config = juce::ValueTree (IDs::magic);

    updateStylesheet();
}

template <class AppType>
void MagicGUIBuilder<AppType>::registerFactory (juce::String type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory)
{
    if (factories.find (type) != factories.cend())
    {
        // You tried to add two factories with the same type name!
        // That cannot work, the second factory will be ignored.
        jassertfalse;
        return;
    }

    factories [type] = factory;
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::registerJUCEFactories()
{
    registerFactory (IDs::slider.toString(),
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
                     });

    registerFactory (IDs::comboBox.toString(),
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::ComboBox>();
                     });

    registerFactory (IDs::textButton.toString(),
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::TextButton>();
                     });

    registerFactory (IDs::toggleButton.toString(),
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         auto text = config.getProperty (IDs::caption, "Active");
                         return std::make_unique<juce::ToggleButton>(text);
                     });
}

template <class AppType>
std::unique_ptr<Decorator> MagicGUIBuilder<AppType>::restoreNode (juce::Component& component, const juce::ValueTree& node)
{
    if (node.getType() == IDs::div)
    {
        auto item = std::make_unique<Container>();
        for (auto childNode : node)
        {
            item->addChildItem (restoreNode (*item, childNode));
        }

        item->configureDecorator (stylesheet, node);

        stylesheet.configureFlexBox (item->flexBox, node);

        component.addAndMakeVisible (item.get());
        return item;
    }

    auto factory = factories [node.getType().toString()];

    if (factory == nullptr)
    {
        DBG ("No factory for: " << node.getType().toString());
    }

    auto item = std::make_unique<Decorator> (factory ? factory (node, app) : nullptr);
    component.addAndMakeVisible (item.get());

    item->configureDecorator (stylesheet, node);

    stylesheet.configureFlexBoxItem (item->flexItem, node);

    if (magicState != nullptr)
    {
        auto parameter = node.getProperty (IDs::parameter, juce::String()).toString();
        if (parameter.isNotEmpty())
            item->connectToState (parameter, magicState->getValueTreeState());
    }

    return item;
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::createDefaultFromParameters (juce::ValueTree& node, const juce::AudioProcessorParameterGroup& tree)
{
    for (const auto& sub : tree.getSubgroups (false))
    {
        auto child = juce::ValueTree (IDs::div);
        child.setProperty (IDs::caption, sub->getName(), nullptr);
        child.setProperty (IDs::styleClass, "group", nullptr);
        createDefaultFromParameters (child, *sub);
        node.appendChild (child, nullptr);
    }

    for (const auto& param : tree.getParameters (false))
    {
        auto child = juce::ValueTree (IDs::slider);
        if (dynamic_cast<juce::AudioParameterChoice*>(param) != nullptr)
            child = juce::ValueTree (IDs::comboBox);
        else if (dynamic_cast<juce::AudioParameterBool*>(param) != nullptr)
            child = juce::ValueTree (IDs::toggleButton);

        child.setProperty (IDs::caption, param->getName (64), nullptr);
        if (const auto* parameterWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
            child.setProperty (IDs::parameter, parameterWithID->paramID, nullptr);

        node.appendChild (child, nullptr);
    }
}

template <>
void MagicGUIBuilder<juce::AudioProcessor>::createDefaultGUITree (bool keepExisting)
{
    if (keepExisting && config.getChildWithName (IDs::div).isValid())
        return;

    auto rootNode = config.getOrCreateChildWithName (IDs::div, &undo);
    rootNode.setProperty (IDs::id, IDs::root, &undo);

    if (magicState != nullptr)
        createDefaultFromParameters (rootNode, magicState->getProcessor().getParameterTree());
}


} // namespace foleys
