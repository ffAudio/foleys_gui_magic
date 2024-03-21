//
// Created by Daniel Walz on 04.02.24.
//

#pragma once

namespace foleys::dsp
{

/**
 * The MagicDspBuilder will create a DspProgram from an XML description
 */
class MagicDspBuilder
{
public:
    using DspFactory = std::function<std::unique_ptr<DspNode> (MagicDspBuilder& builder, const juce::ValueTree& node)>;

    explicit MagicDspBuilder (MagicGUIState& magicState);

    void registerBuiltinFactories();

    void registerDspFactory (const juce::Identifier& name, DspFactory&& factory);

    std::unique_ptr<DspProgram> createProgram (const juce::ValueTree& tree);

    std::unique_ptr<DspNode> createNode (const juce::ValueTree& node);

    MagicGUIState& getMagicState();

    [[nodiscard]] int nextUID();
    void              setNextUID (int uid);

    std::map<juce::Identifier, DspFactory>::iterator begin() {return factories.begin();}
    std::map<juce::Identifier, DspFactory>::iterator end() {return factories.end();}

private:
    MagicGUIState& magicState;

    std::map<juce::Identifier, DspFactory> factories;
    int                                    lastUID = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MagicDspBuilder)
};

}  // namespace foleys::dsp
