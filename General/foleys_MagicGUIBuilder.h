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

#pragma once

namespace foleys
{

/**
 The MagicGUIBuilder is responsible to recreate the GUI from a single ValueTree.
 You can add your own factories to the builder to allow additional components.
 The template parameter AppType specifies a type, that each components gets a reference of,
 it is intended to be eithe AudioProcessor, JUCEApplication or your own MainWindow, it's up to you.
 */
template <class AppType>
class MagicGUIBuilder
{
public:

    MagicGUIBuilder (juce::Component& parent, AppType& app);

    /**
     Loads a gui from a previously stored ValueTree.
     */
    void restoreGUI (const juce::ValueTree& gui);

    /**
     Recalculates the layout of all components
     */
    void updateLayout ();

    void registerFactory (juce::String type, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)> factory);

    void registerJUCEFactories();

    void registerLookAndFeel (juce::String name, std::unique_ptr<juce::LookAndFeel> lookAndFeel);

    void registerJUCELookAndFeels();

    std::unique_ptr<Decorator> restoreNode (juce::Component& component, const juce::ValueTree& node);

private:

    juce::Component& parent;

    AppType&         app;

    std::map<juce::String, std::unique_ptr<juce::LookAndFeel>> lookAndFeels;

    std::map<juce::String, std::function<std::unique_ptr<juce::Component>(const juce::ValueTree&, AppType&)>> factories;

    juce::UndoManager undo;
    juce::ValueTree   config;
    Stylesheet stylesheet;

    std::unique_ptr<Decorator> root;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicGUIBuilder)
};

} // namespace foleys
