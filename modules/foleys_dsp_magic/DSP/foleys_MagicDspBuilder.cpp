//
// Created by Daniel Walz on 04.02.24.
//

#include "foleys_MagicDspBuilder.h"

#include "foleys_BuiltinNodes.h"

namespace foleys::dsp
{

MagicDspBuilder::MagicDspBuilder (MagicGUIState& state) : magicState (state)
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

    registerDspFactory (Noise::ID, &Noise::factory);
    registerDspFactory (Oscillator::ID, &Oscillator::factory);
    registerDspFactory (PlotOutput::ID, &PlotOutput::factory);
    registerDspFactory (Analyser::ID, &Analyser::factory);
}

void MagicDspBuilder::registerDspFactory (const juce::Identifier& name, DspFactory&& factory)
{
    if (factories.find (name) != factories.cend())
    {
        DBG ("Factory with name " << name << " already exists! - skipping");
        jassertfalse;
        return;
    }

    factories[name] = factory;
}

std::unique_ptr<DspProgram> MagicDspBuilder::createProgram (const juce::ValueTree& tree)
{
    return std::make_unique<DspProgram> (*this, tree);
}

std::unique_ptr<DspNode> MagicDspBuilder::createNode (DspProgram& program, const juce::ValueTree& node)
{
    auto factory = factories.find (node.getType());
    if (factory != factories.end())
    {
        auto item = factory->second (program, node);
        return item;
    }

    DBG ("No DSP factory for: " << node.getType().toString());
    return {};
}

bool MagicDspBuilder::canCreate (const juce::ValueTree& node) const
{
    auto factory = factories.find (node.getType());
    return (factory != factories.end());
}

MagicGUIState& MagicDspBuilder::getMagicState()
{
    return magicState;
}

int MagicDspBuilder::nextUID()
{
    return ++lastUID;
}

void MagicDspBuilder::setNextUID (int uid)
{
    lastUID = std::max (lastUID, uid);
}


}  // namespace foleys::dsp
