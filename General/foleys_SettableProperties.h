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
 A SettableProperty is a value that can be selected by the designer and will be
 set for the Component each time the ValueTree is loaded.
 */
struct SettableProperty
{
    enum PropertyType
    {
        Text,           /*< Plain text, e.g. for buttons */
        Number,         /*< A number, e.g. line width */
        Colour,         /*< Show the colour selector and palette names */
        Justification,  /*< Offer a list of the different justification options */
        Toggle,         /*< Show a toggle for bool properties */
        Choice,         /*< Shows choices provided */
        Parameter,      /*< Shows available parameters as choice */
        LevelSource,    /*< Shows available LevelSources as choice */
        PlotSource,     /*< Shows available PlotSources as choice */
        Trigger,        /*< Shows available Triggers as choice */
        Property,       /*< Shows available properties in the state tree to connect to */
        AssetFile       /*< Shows embedded asset files to choose from (BinaryData) */
    };

    SettableProperty (const juce::Identifier& n, PropertyType t, juce::var defaultToUse)
    : name (n), type (t), defaultValue (defaultToUse) {}
    virtual ~SettableProperty() = default;

    virtual void set (juce::Component*, juce::var) const = 0;
    const juce::Identifier name;
    const PropertyType     type;
    const juce::var        defaultValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableProperty)
};

struct SettableChoiceProperty : public SettableProperty
{
    SettableChoiceProperty (const juce::Identifier& n,
                            std::function<void(juce::Component*, juce::var, const juce::NamedValueSet&)> s,
                            juce::var d,
                            PropertyType t)
    : SettableProperty (n, t, d), setter (s) {}

    SettableChoiceProperty (const juce::Identifier& n,
                            std::function<void(juce::Component*, juce::var, const juce::NamedValueSet&)> s,
                            juce::String d,
                            juce::NamedValueSet m)
    : SettableProperty (n, SettableProperty::Choice, d), setter (s), options (m) {}

    void set (juce::Component* c, juce::var v) const override
    {
        if (setter)
        {
            if (v.isVoid())
                setter (c, defaultValue.toString(), options);
            else
                setter (c, v, options);
        }
    }

    std::function<void(juce::Component*, juce::var, const juce::NamedValueSet&)> setter;
    juce::NamedValueSet                                                          options;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableChoiceProperty)
};

struct SettableTextProperty : public SettableProperty
{
    SettableTextProperty (const juce::Identifier& n,
                          juce::var d,
                          std::function<void(juce::Component*, juce::var)> s,
                          PropertyType t = SettableProperty::Text)
      : SettableProperty (n, t, d), setter (s) {}

    void set (juce::Component* c, juce::var v) const override
    {
        if (setter && v.isVoid() == false)
            setter (c, v);
    }

    std::function<void(juce::Component*, juce::var)> setter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableTextProperty)
};

struct SettableBoolProperty : public SettableProperty
{
    SettableBoolProperty (const juce::Identifier& n,
                          juce::var d,
                          std::function<void(juce::Component*, juce::var)> s,
                          PropertyType t = SettableProperty::Toggle)
      : SettableProperty (n, t, d), setter (s) {}

    void set (juce::Component* c, juce::var v) const override
    {
        if (setter && v.isVoid() == false)
            setter (c, v);
    }

    std::function<void(juce::Component*, juce::var)> setter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableBoolProperty)
};

struct SettableNumberProperty : public SettableProperty
{
    SettableNumberProperty (const juce::Identifier& n,
                            juce::var d,
                            std::function<void(juce::Component*, juce::var)> s)
      : SettableProperty (n, SettableProperty::Number, d), setter (s) {}

    void set (juce::Component* c, juce::var v) const override
    {
        if (setter && v.isVoid() == false)
            setter (c, v);
    }

    std::function<void(juce::Component*, juce::var)> setter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableNumberProperty)
};

struct SettableValueProperty : public SettableProperty
{
    SettableValueProperty (const juce::Identifier& n,
                            juce::var d,
                            std::function<void(juce::Component*, juce::var)> s)
      : SettableProperty (n, SettableProperty::Property, d), setter (s) {}

    void set (juce::Component* c, juce::var v) const override
    {
        if (setter && v.isVoid() == false)
            setter (c, v);
    }

    std::function<void(juce::Component*, juce::var)> setter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettableValueProperty)
};

} // namespace foleys
