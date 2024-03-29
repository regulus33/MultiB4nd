/*
  ==============================================================================

    Utils.h
    Created: 14 Apr 2023 7:42:44pm
    Author:  zack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


/*
WARNING WARNING WARNING
Templates are a kind of code generation technique, and the actual code is generated by the compiler at compile-time based on the specific type(s) used to instantiate the template. Therefore, the entire implementation of a template function should be visible to the compiler at the point of instantiation.
 WARNING WARNING WARNING
 */


enum FFTOrder
{
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};


# define MIN_FREQUENCY 20.f
# define MAX_FREQUENCY 20000.f

# define NEGATIVE_INFINITY -72.f
# define MAX_DECIBALS 12.f

# define MIN_THRESHOLD -60.f

/*!
@brief Helper function to create an AudioProcessorValueTreeState attachment.
@tparam Attachment Type of the attachment to be created.
@tparam APVTS Type of the AudioProcessorValueTreeState.
@tparam Params Type of the parameter map.
@tparam ParamName Type of the parameter name.
@tparam SliderType Type of the slider to be attached to the parameter.
@param attachment [out] Pointer to the unique_ptr that holds the attachment.
@param apvts Reference to the AudioProcessorValueTreeState.
@param params Map of parameters.
@param name Name of the parameter to be attached.
@param slider Slider to be attached to the parameter.
*/
template<
typename Attachment,
typename APVTS,
typename Params,
typename ParamName,
typename SliderType
>
void makeAttachment(std::unique_ptr<Attachment>& attachment,
                    APVTS& apvts,
                    const Params& params,
                    const ParamName& name,
                    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts, params.at(name), slider);
}


/*!
 @brief Gets parameters from the Params enum. Just gets the value from a key in a hashmap
 @tparam APVTS APVTS
 @tparam Params Just the params hashmap
 @tparam Name Usually a string
 @param APVts& apvts The APVTS
 @param Params& params A reference to the params hash.
 @param Name&  name A reference to the string name of the param
 @return juce::RangedAudioParameter& A reference to the audio parameter associated with Name.
 */
template<
typename APVTS,
typename Params,
typename Name
>
juce::RangedAudioParameter& getParam(APVTS& apvts, const Params& params, const Name& name)
{
    auto param = apvts.getParameter(params.at(name));
    jassert(param != nullptr);
    
    return *param;
}

/*!
 @brief Just returns true if number is over 1000. Works for any number type.
 @param T& value The number to check
 @return bool
 */
template<typename T>
bool truncateKiloValue(T& value)
{
   if(value > static_cast<T>(999))
   {
       value /= static_cast<T>(1000);
       return true;
   }
   else
   {
       return false;
   }
}


juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);


/*!
 @brief Add
 @tparam Labels Mostly ends up being RotarySliderWithLabels. Must have a labels member.
 @tparam Type of the suffix. String `"dB"` for example
 */
template<
typename Labels,
typename ParamType,
typename SuffixType
>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix)
{
    labels.clear();
    labels.add({0.f, getValString(param, true, suffix)});
    labels.add({1.f, getValString(param, false, suffix)});
}

juce::Rectangle<int> drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds);

