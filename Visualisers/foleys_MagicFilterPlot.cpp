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

MagicFilterPlot::MagicFilterPlot()
{
    frequencies.resize (300);
    for (size_t i = 0; i < frequencies.size(); ++i)
        frequencies [i] = 20.0 * std::pow (2.0, i / 30.0);

    magnitudes.resize (frequencies.size());
}

void MagicFilterPlot::setIIRCoefficients (juce::dsp::IIR::Coefficients<float>::Ptr coefficients, float maxDBToDisplay)
{
    if (sampleRate < 20.0)
        return;

    const juce::ScopedWriteLock writeLock (plotLock);

    maxDB = maxDBToDisplay;
    coefficients->getMagnitudeForFrequencyArray (frequencies.data(),
                                                 magnitudes.data(),
                                                 frequencies.size(),
                                                 sampleRate);

    resetLastDataFlag();
}

void MagicFilterPlot::setIIRCoefficients (float gain, std::vector<juce::dsp::IIR::Coefficients<float>::Ptr> coefficients, float maxDBToDisplay)
{
    if (sampleRate < 20.0)
        return;

    const juce::ScopedWriteLock writeLock (plotLock);

    std::vector<double> buffer (frequencies.size(), 0.0);

    maxDB = maxDBToDisplay;
    std::fill (magnitudes.begin(), magnitudes.end(), gain);

    for (auto ptr : coefficients)
    {
        ptr->getMagnitudeForFrequencyArray (frequencies.data(),
                                            buffer.data(),
                                            frequencies.size(),
                                            sampleRate);
        juce::FloatVectorOperations::multiply (magnitudes.data(), buffer.data(), int (magnitudes.size()));
    }

    resetLastDataFlag();
}

void MagicFilterPlot::pushSamples (const juce::AudioBuffer<float>&){}

void MagicFilterPlot::createPlotPaths (juce::Path& path, juce::Path& filledPath, juce::Rectangle<float> bounds, MagicPlotComponent&)
{
    const juce::ScopedReadLock readLock (plotLock);

    const auto yFactor = 2.0f * bounds.getHeight() / juce::Decibels::decibelsToGain (maxDB);
    const auto xFactor = static_cast<double> (bounds.getWidth()) / frequencies.size();

    path.clear();
    path.startNewSubPath (bounds.getX(), float (magnitudes [0] > 0 ? bounds.getCentreY() - yFactor * std::log (magnitudes [0]) / std::log (2) : bounds.getBottom()));
    for (size_t i=1; i < frequencies.size(); ++i)
        path.lineTo (float (bounds.getX() + i * xFactor),
                     float (magnitudes [i] > 0 ? bounds.getCentreY() - yFactor * std::log (magnitudes [i]) / std::log (2) : bounds.getBottom()));

    filledPath = path;
    filledPath.lineTo (bounds.getBottomRight());
    filledPath.lineTo (bounds.getBottomLeft());
    filledPath.closeSubPath();
}

void MagicFilterPlot::prepareToPlay (double sampleRateToUse, int)
{
    sampleRate = sampleRateToUse;
}

} // namespace foleys
