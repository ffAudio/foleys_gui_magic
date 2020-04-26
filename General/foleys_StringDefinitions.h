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

namespace IDs
{
    static juce::Identifier magic        { "magic" };
    static juce::Identifier resizable    { "resizable" };
    static juce::Identifier resizeCorner { "resize-corner" };
    static juce::Identifier styles       { "Styles" };
    static juce::Identifier style        { "Style" };
    static juce::Identifier view         { "View" };
    static juce::Identifier slider       { "Slider" };
    static juce::Identifier textButton   { "TextButton" };
    static juce::Identifier toggleButton { "ToggleButton" };
    static juce::Identifier label        { "Label" };
    static juce::Identifier comboBox     { "ComboBox" };
    static juce::Identifier meter        { "Meter" };
    static juce::Identifier plot         { "Plot" };
    static juce::Identifier xyDragComponent   { "XYDragComponent" };
    static juce::Identifier keyboardComponent { "KeyboardComponent" };
    static juce::Identifier webBrowser   { "WebBrowserComponent" };

    static juce::Identifier visibility   { "visibility" };
    static juce::Identifier caption      { "caption" };
    static juce::Identifier captionPlacement { "caption-placement" };
    static juce::Identifier captionColour { "caption-color" };
    static juce::Identifier captionSize  { "caption-size" };
    static juce::Identifier lookAndFeel  { "lookAndFeel" };
    static juce::Identifier tooltip      { "tooltip" };
    static juce::Identifier parameter    { "parameter" };
    static juce::Identifier parameterX   { "parameter-x" };
    static juce::Identifier parameterY   { "parameter-y" };
    static juce::Identifier source       { "source" };

    static juce::Identifier name         { "name" };
    static juce::Identifier selected     { "selected" };
    static juce::Identifier id           { "id" };
    static juce::Identifier styleClass   { "class" };
    static juce::Identifier active       { "active" };

    static juce::Identifier backgroundColour  { "background-color" };
    static juce::Identifier borderColour      { "border-color" };
    static juce::Identifier tabCaption   { "tab-caption" };
    static juce::Identifier tabColour    { "tab-color" };
    static juce::Identifier border       { "border" };
    static juce::Identifier margin       { "margin" };
    static juce::Identifier padding      { "padding" };
    static juce::Identifier radius       { "radius" };

    static juce::Identifier display      { "display" };
    static juce::Identifier repaintHz    { "repaint-hz" };
    static juce::String     contents     { "contents" };
    static juce::String     tabbed       { "tabbed" };
    static juce::String     flexbox      { "flexbox" };

    static juce::String     root         { "root" };

    static juce::Identifier nodes     { "Nodes"   };
    static juce::Identifier classes   { "Classes" };
    static juce::Identifier types     { "Types"   };

    static juce::String dragSelected  { "selected" };
    static juce::String dragParameter { "parameter:" };
    static juce::String dragSource    { "source:" };
    static juce::String dragPlot      { "plot:" };


    static juce::Identifier backgroundImage     { "background-image" };
    static juce::Identifier backgroundAlpha     { "background-alpha" };
    static juce::String     linearGradient      { "linear-gradient" };
    static juce::String     radialGradient      { "radial-gradient" };
    static juce::String     repeatingLinearGradient { "repeating-linear-gradient" };
    static juce::String     repeatingRadialGradient { "repeating-radial-gradient" };

    static juce::Identifier imagePlacement      { "image-placement" };
    static juce::String     imageCentred        { "centred" };
    static juce::String     imageFill           { "fill" };
    static juce::String     imageStretch        { "stretch" };

    static juce::Identifier backgroundGradient  { "background-gradient" };

    static juce::Identifier flexDirection       { "flex-direction" };
    static juce::String     flexDirRow          { "row" };
    static juce::String     flexDirRowReverse   { "row-reverse" };
    static juce::String     flexDirColumn       { "column" };
    static juce::String     flexDirColumnReverse { "column-reverse" };

    static juce::Identifier flexWrap            { "flex-wrap" };
    static juce::String     flexNoWrap          { "nowrap" };
    static juce::String     flexWrapNormal      { "wrap" };
    static juce::String     flexWrapReverse     { "wrap-reverse" };

    static juce::Identifier flexGrow            { "flex-grow" };
    static juce::Identifier flexShrink          { "flex-shrink" };

    static juce::Identifier flexAlignContent    { "flex-align-content" };
    static juce::Identifier flexAlignItems      { "flex-align-items" };
    static juce::Identifier flexJustifyContent  { "flex-justify-content" };
    static juce::Identifier flexAlignSelf       { "flex-align-self" };
    static juce::Identifier flexOrder           { "flex-order" };
    static juce::String     flexStretch         { "stretch" };
    static juce::String     flexStart           { "start" };
    static juce::String     flexEnd             { "end" };
    static juce::String     flexCenter          { "center" };
    static juce::String     flexSpaceBetween    { "space-between" };
    static juce::String     flexSpaceAround     { "space-around" };
    static juce::String     flexAuto            { "auto" };

    static juce::Identifier media       { "media" };
    static juce::Identifier recursive   { "recursive" };
    static juce::Identifier minWidth    { "min-width" };
    static juce::Identifier maxWidth    { "max-width" };
    static juce::Identifier minHeight   { "min-height" };
    static juce::Identifier maxHeight   { "max-height" };
    static juce::Identifier width       { "width" };
    static juce::Identifier height      { "height" };

    static juce::Identifier lastSize    { "last-size" };
}

} // namespace foleys
