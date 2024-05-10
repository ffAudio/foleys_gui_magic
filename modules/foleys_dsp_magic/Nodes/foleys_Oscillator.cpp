//
// Created by Daniel Walz on 25.02.24.
//

#include "foleys_Oscillator.h"

#include <cmath>

namespace foleys::dsp
{

static constexpr size_t wavetablesize = 8192;

Oscillator::Oscillator (DspProgram& program, const juce::ValueTree& config) : DspNode (program, config)
{
    addAudioOutput (TRANS ("Audio Out"));
    addParameterOutput (TRANS ("Signal Out"));

    addParameterInput (TRANS ("Signal Type"));
    addParameterInput (TRANS ("Frequency"));
    addParameterInput (TRANS ("Amplitude"));
    addParameterInput (TRANS ("Offset"));

    // make sure to repeat the first sample at the end for interpolation
    wavetable.resize (wavetablesize + 1, 0);
}

void Oscillator::setWaveType (WaveType type)
{
    if (type == currentWaveType)
        return;

    switch (type)
    {
        case WaveType::None: std::fill (wavetable.begin(), wavetable.end(), 0.0f); break;
        case WaveType::Sine:
            for (size_t i = 0; i < wavetablesize; ++i)
                wavetable[i] = std::sin (static_cast<float> (i) * juce::MathConstants<float>::twoPi / wavetablesize);
            break;
        case WaveType::Saw:
            for (size_t i = 0; i < wavetablesize; ++i)
                wavetable[i] = std::lerp (-1.0f, 1.0f, static_cast<float> (i) / wavetablesize);
            break;
        case WaveType::Square:
            std::fill (wavetable.begin(), std::next (wavetable.begin(), wavetablesize / 2), -1.0f);
            std::fill (std::next (wavetable.begin(), wavetablesize / 2), wavetable.end(), 1.0f);
            break;
        default: break;
    }

    wavetable.back() = wavetable.front();
    currentWaveType  = type;
}

void Oscillator::prepare (juce::dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;

    buffer.setSize (static_cast<int> (spec.numChannels), static_cast<int> (spec.maximumBlockSize));
}

void Oscillator::process (int numSamples)
{
    buffer.clear();
    auto block = juce::dsp::AudioBlock<float> (buffer).getSubBlock (0, numSamples);

    auto* signalType = getConnectedOutput (ConnectionType::Parameter, 0);
    auto* frequency  = getConnectedOutput (ConnectionType::Parameter, 1);
    auto* amplitude  = getConnectedOutput (ConnectionType::Parameter, 2);
    auto* dcOffset   = getConnectedOutput (ConnectionType::Parameter, 3);

    if (signalType)
        setWaveType (static_cast<WaveType> (signalType->getStaticValue()));

    if (!frequency || currentWaveType == WaveType::None)
    {
        getOutput (ConnectionType::Audio, 0)->setStaticValue (0.0f);
        getOutput (ConnectionType::Parameter, 0)->setStaticValue (0.0f);

        return;
    }

    auto* ptr = block.getChannelPointer (0);
    if (frequency->isStatic())
    {
        auto freq = frequency->normalize (frequency->getStaticValue()) * 20000.0;
        auto inc  = std::clamp (static_cast<float> (freq / sampleRate), 0.0f, 1.0f);
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            auto pos = phase * wavetablesize;
            auto p0  = static_cast<size_t> (pos);
            ptr[i]   = std::lerp (wavetable[p0], wavetable[p0 + 1], pos - static_cast<float> (p0));

            phase += inc;
            if (phase >= 1.0f)
                phase -= 1.0f;
        }
    }
    else
    {
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            auto pos = phase * wavetablesize;
            auto p0  = static_cast<size_t> (pos);
            ptr[i]   = std::lerp (wavetable[p0], wavetable[p0 + 1], pos - static_cast<float> (p0));

            auto freq = frequency->normalize (frequency->getValueAt (i)) * 20000.0;
            phase += std::clamp (static_cast<float> (freq / sampleRate), 0.0f, 1.0f);
            if (phase >= 1.0f)
                phase -= 1.0f;
        }
    }

    if (amplitude)
        amplitude->multiply (block, 0.0f, 1.0f);

    if (dcOffset)
        dcOffset->add (block, -1.0f, 1.0f);

    for (size_t ch = 1; ch < block.getNumChannels(); ++ch)
        block.getSingleChannelBlock (ch).copyFrom (block.getSingleChannelBlock (0));

    auto* audioOutput = getOutput (ConnectionType::Audio, 0);
    audioOutput->setAudioBlock (block);

    auto* parameterOutput = getOutput (ConnectionType::Parameter, 0);
    parameterOutput->setAudioBlock (block);
}

}  // namespace foleys::dsp
