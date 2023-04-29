/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>
#include "DSP/CompressorBand.h"
#include "DSP/SingleChannelSampleFifo.h"
#include <array>

/*!
 @class SimpleMBCompAudioProcessor
 @brief The main audio processing class for the Simple Multiband Compressor plugin.
 This class is responsible for splitting the audio into 3 frequency bands using Linkwitz-Riley filters, applying
 separate compressors to each band, and then summing the bands together to produce a compressed signal. The
 parameters for the filters and compressors can be adjusted through the user interface created by the
 SimpleMBCompAudioProcessorEditor class. The AudioProcessorValueTreeState (APVTS) object is used to manage the
 plugin's parameters and their values.
 @see SimpleMBCompAudioProcessorEditor
 @see CompressorBand
 @see SingleChannelSampleFifo
 @see juce::AudioProcessor
 @see juce::AudioProcessorValueTreeState
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
    // This needs to be public so UI components can access it in PluginEditor class
    APVTS apvts { *this, nullptr, "Parameters", createParameterLayout() };
    
    using BlockType = juce::AudioBuffer<float>;
    SingleChannelSampleFifo<BlockType> leftChannelFifo { Channel::Left };
    SingleChannelSampleFifo<BlockType> rightChannelFifo { Channel::Right };
    
    
private:
    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];
    
    juce::dsp::Compressor<float> compressor;
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    //     fc0  fc1
    Filter LP1, AP2,
    HP1, LP2,
    HP2;
    
    juce::AudioParameterFloat* lowMidCrossover { nullptr };
    juce::AudioParameterFloat* midHighCrossover { nullptr };
    
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam { nullptr };
    juce::AudioParameterFloat* outputGainParam { nullptr };
    
    
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    void updateState();
    void splitBands(juce::AudioBuffer<float>& inputBuffer);
    
    juce::dsp::Oscillator<float> osc;
    juce::dsp::Gain<float> gain;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleMBCompAudioProcessor)
};
