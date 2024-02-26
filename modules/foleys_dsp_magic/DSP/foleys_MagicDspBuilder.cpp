//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_MagicDspBuilder.h"

#include "foleys_BuiltinNodes.h"

namespace foleys::dsp
{

MagicDspBuilder::MagicDspBuilder (MagicGUIState& magicState) : m_magicState (magicState)
{
    registerBuiltinFactories();
}


void MagicDspBuilder::registerBuiltinFactories()
{
    // BuiltinNodes
    registerDspFactory (AudioInput::ID, &AudioInput::factory);
    registerDspFactory (AudioOutput::ID, &AudioOutput::factory);
    registerDspFactory (MidiInput::ID, &MidiInput::factory);
    registerDspFactory (MidiOutput::ID, &MidiOutput::factory);
}

void MagicDspBuilder::registerDspFactory (const juce::Identifier& name, DspFactory&& factory)
{
    if (m_factories.find (name) != m_factories.cend())
    {
        DBG ("Factory with name " << name << " already exists! - skipping");
        jassertfalse;
        return;
    }

    m_factories[name] = factory;
}

std::unique_ptr<DspProgram> MagicDspBuilder::createProgram (const juce::ValueTree& tree)
{
    return std::make_unique<DspProgram> (*this, tree);
}

MagicGUIState& MagicDspBuilder::getMagicState()
{
    return m_magicState;
}

}  // namespace foleys::dsp
