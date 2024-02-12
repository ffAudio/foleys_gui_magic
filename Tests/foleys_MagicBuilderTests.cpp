/*
 ==============================================================================
   Copyright (c) 2023 Foleys Finest Audio - Daniel Walz
   All rights reserved.

    **BSD 3-Clause License**

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

 ==============================================================================

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

#include <catch2/catch_test_macros.hpp>
#include <foleys_gui_magic/foleys_gui_magic.h>


TEST_CASE ("GUI builder test", "[gui]")
{
    const juce::ScopedJuceInitialiser_GUI init;

    class TestComponent : public juce::Component
    {
    public:
        TestComponent()
        {
            auto tree = juce::ValueTree::fromXml ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                  "\n"
                                                  "<magic>\n"
                                                  "  <Styles>\n"
                                                  "    <Style name=\"default\">\n"
                                                  "      <Nodes/>\n"
                                                  "      <Classes>\n"
                                                  "        <hidden flex-grow=\"0\" active=\"HideEditor\">\n"
                                                  "          <media max-height=\"100000\" max-width=\"100000\"/>\n"
                                                  "        </hidden>\n"
                                                  "      </Classes>\n"
                                                  "      <Palettes>\n"
                                                  "        <default/>\n"
                                                  "      </Palettes>\n"
                                                  "    </Style>\n"
                                                  "  </Styles>\n"
                                                  "  <View>\n"
                                                  "    <View flex-direction=\"column\" caption=\"Hide Test\">\n"
                                                  "      <View>\n"
                                                  "        <View/>\n"
                                                  "        <ToggleButton id=\"hideEditor\" text=\"Hide Editor View\" property=\"HideEditor\"/>\n"
                                                  "        <View/>\n"
                                                  "      </View>\n"
                                                  "      <View>\n"
                                                  "        <View id=\"editor\" caption=\"Editor View\" class=\"hidden\"/>\n"
                                                  "        <View caption=\"Performance View\"/>\n"
                                                  "      </View>\n"
                                                  "    </View>\n"
                                                  "  </View>\n"
                                                  "</magic>");

            state.setGuiValueTree (tree);

            builder.registerJUCEFactories();
            builder.createGUI (*this);
        }

        void resized() override { builder.updateLayout(); }

        foleys::MagicGUIState   state;
        foleys::MagicGUIBuilder builder { state };
    };

    auto testComponent = std::make_unique<TestComponent>();

    auto value = testComponent->state.getPropertyAsValue ("HideEditor");
    value.setValue (false);

    auto flags = juce::ComponentPeer::windowAppearsOnTaskbar | juce::ComponentPeer::windowHasTitleBar;
    testComponent->addToDesktop (flags);
    testComponent->setVisible (true);

    testComponent->setSize (800, 600);

    auto* editorItem = testComponent->builder.findGuiItemWithId ("editor");
    REQUIRE (editorItem);
    REQUIRE (editorItem->getWidth() > 200);
    REQUIRE (editorItem->getHeight() > 200);

    auto* buttonItem = testComponent->builder.findGuiItemWithId ("hideEditor");
    REQUIRE (buttonItem);

    auto* button = dynamic_cast<juce::ToggleButton*> (buttonItem->getWrappedComponent());
    REQUIRE (button);
    REQUIRE (button->getToggleState() == false);

    button->triggerClick();

    juce::MessageManager::getInstance()->runDispatchLoopUntil (100);

    REQUIRE (button->getToggleState() == true);

    // Horizontal layout with grow=0 from class hidden should make it zero wide
    REQUIRE (editorItem->getWidth() == 0);
}
