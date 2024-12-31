/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Echo1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Echo1AudioProcessor();
    ~Echo1AudioProcessor() override;

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
    
    
    
    
    //============================= Getters and Setters ============================
    
    float* getDryWet() { return &dryWet; }
    void setDryWet(float val)
    {
        dryWet = juce::jlimit(0.01f, 1.0f, val);
    }
    
    float* getDecayTime() { return &decayTime; }
    void setDecayTime(float val)
    {
        decayTime = juce::jlimit(0.01f, 1.0f, val);
    }
    
    float* getRoomSize() { return &roomSize; }
    void setRoomSize(float val)
    {
        roomSize = juce::jlimit(0.01f, 1.0f, val);
    }
    
    float* getVolume() { return &volume; }
    void setVolume(float val)
    {
        volume = juce::jlimit(0.0f, 1.0f, val);
        
    }
    
    


private:
    //==============================================================================
    float dryWet = 0.0f;
    float decayTime = 0.0f;
    float roomSize = 0.0f;
    float volume = 0.0f; // will be radius of plusing volume circle
    //float feedback = 0.5f;
    
    juce::Reverb reverb;
    juce::dsp::DelayLine<float> delayLine;
    
    

    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Echo1AudioProcessor)
};
