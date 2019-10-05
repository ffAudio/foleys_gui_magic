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

    plotChanged = true;
    sendChangeMessage();
}

void MagicFilterPlot::pushSamples (const juce::AudioBuffer<float>&){}

void MagicFilterPlot::drawPlot (juce::Graphics& g, juce::Rectangle<float> bounds, MagicPlotComponent& component)
{
    if (plotChanged || lastBounds != bounds)
    {
        const juce::ScopedReadLock readLock (plotLock);

        const auto yFactor = 2.0f * bounds.getHeight() / juce::Decibels::decibelsToGain (maxDB);
        const auto xFactor = static_cast<double> (bounds.getWidth()) / frequencies.size();

        path.clear();
        path.startNewSubPath (bounds.getX(), float (magnitudes [0] > 0 ? bounds.getCentreY() - yFactor * std::log (magnitudes [0]) / std::log (2) : bounds.getBottom()));
        for (size_t i=1; i < frequencies.size(); ++i)
            path.lineTo (float (bounds.getX() + i * xFactor),
                         float (magnitudes [i] > 0 ? bounds.getCentreY() - yFactor * std::log (magnitudes [i]) / std::log (2) : bounds.getBottom()));

        lastBounds  = bounds;
        plotChanged = false;
    }

    g.setColour (component.findColour (MagicPlotComponent::plotColourId));
    g.strokePath (path, juce::PathStrokeType (2.0f));
}

void MagicFilterPlot::prepareToPlay (double sampleRateToUse, int)
{
    sampleRate = sampleRateToUse;
}

} // namespace foleys
