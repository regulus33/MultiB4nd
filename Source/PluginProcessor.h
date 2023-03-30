/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

/*
 DSP roadmap
 1. Figure out how to split audio into 3 bands
 2. create params to control where this split happens
 3. prove that splitting between 3 bands results in no artifacts
 4. create audio params for the 3 compressor bands
 5. add 2 remaining comps
 6. add ability to mute solo and bypass indiv comps
 7. add input and output gain to offset changes in output level
 8. cleanup
 
 Linkwitz - Riley
 */

#include <JuceHeader.h>

namespace Params
{
    enum Names
    {
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,
        
        Threshold_Low_Band,
        Threshold_Mid_Band,
        Threshold_High_Band,
        
        Attack_Low_Band,
        Attack_Mid_Band,
        Attack_High_Band,
        
        Release_Low_Band,
        Release_Mid_Band,
        Release_High_Band,
        
        Ratio_Low_Band,
        Ratio_Mid_Band,
        Ratio_High_Band,
        
        Bypassed_Low_Band,
        Bypassed_Mid_Band,
        Bypassed_High_Band,
    };
    
    //https://stackoverflow.com/questions/17712001/how-is-meyers-implementation-of-a-singleton-actually-a-singleton
    
    inline const std::map<Names, juce::String>& GetParams()
    {
        // the static keyword guarantees that only one of this will exist per program
        static std::map<Names, juce::String> params = {
            {Low_Mid_Crossover_Freq, "Low-Mid Crossover Freq"},
            {Mid_High_Crossover_Freq, "Mid-High Crossover Freq"},
            {Threshold_Low_Band, "Threshold Low Band"},
            {Threshold_Mid_Band, "Threshold Mid Band"},
            {Threshold_High_Band, "Threshold High Band"},
            {Attack_Low_Band, "Attack Low Band"},
            {Attack_Mid_Band, "Attack Mid Band"},
            {Attack_High_Band, "Attack High Band"},
            {Release_Low_Band, "Release Low Band"},
            {Release_Mid_Band, "Release Mid Band"},
            {Release_High_Band, "Release High Band"},
            {Ratio_Low_Band, "Ratio Low Band"},
            {Ratio_Mid_Band, "Ratio Mid Band"},
            {Ratio_High_Band, "Ratio High Band"},
            {Bypassed_Low_Band, "Bypassed Low Band"},
            {Bypassed_Mid_Band, "Bypassed Mid Band"},
            {Bypassed_High_Band, "Bypassed High Band"}
        };
        
        return params;
    }
}

struct CompressorBand
{
    juce::AudioParameterFloat* attack { nullptr };
    juce::AudioParameterFloat* release { nullptr };
    juce::AudioParameterFloat* threshold { nullptr };
    juce::AudioParameterChoice* ratio { nullptr };
    juce::AudioParameterBool* bypassed { nullptr };
    
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        compressor.prepare(spec);
    }
    
    void updateCompressorSettings()
    {
        compressor.setAttack(attack->get());
        compressor.setRelease(release->get());
        compressor.setThreshold(threshold->get());
        compressor.setRatio(
                            ratio->getCurrentChoiceName().getFloatValue());
    }
    
    void process(juce::AudioBuffer<float>& buffer)
    {
        // Create our own block to manipulate. Blocks are basically the narrowed down version of the buffer.
        // focused -> channels, range etc.
        auto block = juce::dsp::AudioBlock<float>(buffer);
        
        // this replaces the audio in the buffer I think its just more memory efficient
        // context has like everything important in it like sample rate buffer size etc.
        // is the block plus the "context" which makes sense
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        
        // Bypass the whole processBlock code (anything we would do is not done)
        context.isBypassed = bypassed->get();
        
        // We are just passing our context pointer to compressor and since this context is
        // already being fed through the output somehow, the compression is applied
        compressor.process(context);
    }
private:
    juce::dsp::Compressor<float> compressor;
};

//==============================================================================
/**
 */
class SimpleMBCompAudioProcessor  : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
, public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    SimpleMBCompAudioProcessor();
    ~SimpleMBCompAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParameterLayout();
    APVTS apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
private:
    //    juce::dsp::Compressor<float> compressor;
    //
    //    // these are just pointers to help optimize the lookup for parameter values on each processBlock
    //    juce::AudioParameterFloat* attack { nullptr };
    //    juce::AudioParameterFloat* release { nullptr };
    //    juce::AudioParameterFloat* threshold { nullptr };
    //    juce::AudioParameterChoice* ratio { nullptr };
    //    juce::AudioParameterBool* bypassed { nullptr };
    
    // See the data class or "Struct" declared at top of file
    CompressorBand compressor;
    //
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
