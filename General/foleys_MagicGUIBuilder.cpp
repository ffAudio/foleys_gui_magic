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
    static juce::Identifier styles       { "Styles" };
    static juce::Identifier style        { "Style" };
    static juce::Identifier div          { "Div" };
    static juce::Identifier slider       { "Slider" };
    static juce::Identifier textButton   { "TextButton" };
    static juce::Identifier toggleButton { "ToggleButton" };
    static juce::Identifier comboBox     { "ComboBox" };
    static juce::Identifier plot         { "Plot" };

    static juce::Identifier caption      { "caption" };
    static juce::Identifier lookAndFeel  { "lookAndFeel" };
    static juce::Identifier parameter    { "parameter" };
    static juce::Identifier source       { "source" };

    static juce::Identifier name         { "name" };
    static juce::Identifier selected     { "selected" };
    static juce::Identifier id           { "id" };
    static juce::Identifier styleClass   { "class" };

    static juce::Identifier backgroundColour  { "background-color" };
    static juce::Identifier borderColour      { "border-color" };
    static juce::Identifier border       { "border" };
    static juce::Identifier margin       { "margin" };
    static juce::Identifier padding      { "padding" };

    static juce::Identifier display      { "display" };
    static juce::String     contents     { "contents" };
    static juce::String     flexbox      { "flexbox" };

    static juce::String     root         { "root" };

}


MagicBuilder::MagicBuilder (juce::Component& parentToUse)
  : parent (parentToUse)
{
}

MagicBuilder::~MagicBuilder()
{
    config.removeListener (this);
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

    config.addListener (this);
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
    stylesheet.registerLookAndFeel (name, std::move (lookAndFeel));
}

void MagicBuilder::registerJUCELookAndFeels()
{
    stylesheet.registerLookAndFeel ("LookAndFeel_V1", std::make_unique<juce::LookAndFeel_V1>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V2", std::make_unique<juce::LookAndFeel_V2>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V3", std::make_unique<juce::LookAndFeel_V3>());
    stylesheet.registerLookAndFeel ("LookAndFeel_V4", std::make_unique<juce::LookAndFeel_V4>());
    stylesheet.registerLookAndFeel ("FoleysFinest", std::make_unique<LookAndFeel>());
}

void MagicBuilder::setColourTranslation (juce::Identifier type, std::vector<std::pair<juce::String, int>> mapping)
{
    if (colourTranslations.find (type) != colourTranslations.cend())
    {
        // You tried to add two colour mappings for the same type name!
        // That cannot work, the second mapping will be ignored.
        jassertfalse;
        return;
    }

    colourTranslations [type] = mapping;
}

void MagicBuilder::valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&)
{
    updateAll();
}

void MagicBuilder::valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&)
{
    updateAll();
}

void MagicBuilder::valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int)
{
    updateAll();
}

void MagicBuilder::valueTreeChildOrderChanged (juce::ValueTree&, int, int)
{
    updateAll();
}

void MagicBuilder::valueTreeParentChanged (juce::ValueTree&)
{
    updateAll();
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
void MagicGUIBuilder<AppType>::registerFactory (juce::Identifier type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory)
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
    registerFactory (IDs::slider,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
                     });

    setColourTranslation (IDs::slider,
    {
        { "slider-background", juce::Slider::backgroundColourId },
        { "slider-thumb", juce::Slider::thumbColourId },
        { "slider-track", juce::Slider::trackColourId },
        { "rotary-fill", juce::Slider::rotarySliderFillColourId },
        { "rotary-outline", juce::Slider::rotarySliderOutlineColourId },
        { "slider-text", juce::Slider::textBoxTextColourId },
        { "slider-text-background", juce::Slider::textBoxBackgroundColourId },
        { "slider-text-highlight", juce::Slider::textBoxHighlightColourId },
        { "slider-text-outline", juce::Slider::textBoxOutlineColourId }
    });

    registerFactory (IDs::comboBox,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::ComboBox>();
                     });

    setColourTranslation (IDs::comboBox,
                          {
                              { "combo-background", juce::ComboBox::backgroundColourId },
                              { "combo-text", juce::ComboBox::textColourId },
                              { "combo-outline", juce::ComboBox::outlineColourId },
                              { "combo-button", juce::ComboBox::buttonColourId },
                              { "combo-arrow", juce::ComboBox::arrowColourId },
                              { "combo-focused-outline", juce::ComboBox::focusedOutlineColourId }
                          });

    registerFactory (IDs::textButton,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         return std::make_unique<juce::TextButton>();
                     });

    setColourTranslation (IDs::textButton,
                          {
                              { "button-color", juce::TextButton::buttonColourId },
                              { "button-on-color", juce::TextButton::buttonOnColourId },
                              { "button-off-text", juce::TextButton::textColourOffId },
                              { "button-on-text", juce::TextButton::textColourOnId }
                          });

    registerFactory (IDs::toggleButton,
                     [] (const juce::ValueTree& config, auto& app)
                     {
                         auto text = config.getProperty (IDs::caption, "Active");
                         return std::make_unique<juce::ToggleButton>(text);
                     });

    setColourTranslation (IDs::toggleButton,
                          {
                              { "toggle-text", juce::ToggleButton::textColourId },
                              { "toggle-tick", juce::ToggleButton::tickColourId },
                              { "toggle-tick-disabled", juce::ToggleButton::tickDisabledColourId }
                          });

    registerFactory (IDs::plot,
                     [&] (const juce::ValueTree& config, auto& app)
                     {
                         auto item = std::make_unique<MagicPlotComponent>();
                         if (magicState && config.hasProperty (IDs::source))
                         {
                             auto* source = magicState->getPlotSource (config.getProperty (IDs::source).toString());
                             item->setPlotSource (source);
                         }
                         return std::move (item);
                     });

    setColourTranslation (IDs::plot,
                          {
                              { "plot-color", MagicPlotComponent::plotColourId },
                              { "plot-fill-color", MagicPlotComponent::plotFillColourId }
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

        auto display = stylesheet.getProperty (IDs::display, node).toString();
        if (display == IDs::contents)
        {
            item->layout = Container::Layout::Contents;
        }
        else
        {
            item->layout = Container::Layout::FlexBox;
            stylesheet.configureFlexBox (item->flexBox, node);
        }

        component.addAndMakeVisible (item.get());
        return item;
    }

    auto factory = factories [node.getType()];

    if (factory == nullptr)
    {
        DBG ("No factory for: " << node.getType().toString());
    }

    auto item = std::make_unique<Decorator> (factory ? factory (node, app) : nullptr);
    component.addAndMakeVisible (item.get());

    item->configureDecorator (stylesheet, node);

    stylesheet.configureFlexBoxItem (item->flexItem, node);

    const auto translation = colourTranslations.find (node.getType());
    if (translation != colourTranslations.end() && item->getWrappedComponent() != nullptr)
    {
        for (auto& pair : translation->second)
        {
            auto colour = stylesheet.getProperty (pair.first, node).toString();
            if (colour.isNotEmpty())
                item->getWrappedComponent()->setColour (pair.second, stylesheet.parseColour (colour));
        }
    }

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
