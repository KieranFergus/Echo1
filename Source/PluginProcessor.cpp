/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Echo1AudioProcessor::Echo1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

Echo1AudioProcessor::~Echo1AudioProcessor()
{
}




//==============================================================================
const juce::String Echo1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Echo1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Echo1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Echo1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Echo1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Echo1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Echo1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Echo1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Echo1AudioProcessor::getProgramName (int index)
{
    return {};
}

void Echo1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Echo1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int maxDelayInSamples = static_cast<int>(2.0 * sampleRate); // Maximum 2 seconds delay

        // Resize the delay line to match the number of input channels
        delayLine.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumInputChannels()) });
        delayLine.setMaximumDelayInSamples(maxDelayInSamples);
}

void Echo1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Echo1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Echo1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    jassert(totalNumInputChannels == 2 && totalNumOutputChannels == 2); // Ensure stereo

    // Configure reverb parameters
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = *getRoomSize();
    reverbParams.damping = *getDryWet() / 2.f;
    reverbParams.wetLevel = *getDryWet();
    reverbParams.dryLevel = 1.0f - *getDryWet();
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    // Calculate and set delay time
    float decayTime = juce::jlimit(0.1f, 0.8f, *getDecayTime());
    float delayTime = juce::jmap(decayTime, 0.1f, 1.0f, 50.0f, 500.0f); // Map decay time to delay time
    delayLine.setDelay(getSampleRate() * (delayTime / 1000.0f));
    
    // Process the delay with feedback
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float delayedSample = delayLine.popSample(channel); // Get delayed output
            float inputSample = channelData[sample] + delayedSample * *getDecayTime(); // Add feedback to input
            delayLine.pushSample(channel, inputSample); // Feed back into delay line
            channelData[sample] = (1.0f - *getDryWet()) * channelData[sample] + *getDryWet() * delayedSample; // Dry/wet mix
        }
    }
    
    // Process reverb (optional)
    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);
    reverb.processStereo(leftChannel, rightChannel, buffer.getNumSamples());
    
    float rmsLevel = 0.0f;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getReadPointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                rmsLevel += channelData[sample] * channelData[sample]; // Sum of squares
            }
        }
    

    // Normalize by the total number of samples
    rmsLevel = std::sqrt(rmsLevel / (buffer.getNumSamples() * buffer.getNumChannels()));
    // Store the calculated volume level
    setVolume(juce::jlimit(0.0f, 1.0f, rmsLevel));
    //DBG("Volume: " << volume << "      ");
}


//==============================================================================
bool Echo1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Echo1AudioProcessor::createEditor()
{
    return new Echo1AudioProcessorEditor (*this);
}

//==============================================================================
void Echo1AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Create a ValueTree to hold the state
    juce::ValueTree state("PARAMETERS");

    // Add parameters as properties
    state.setProperty("dryWet", dryWet, nullptr);
    state.setProperty("decayTime", decayTime, nullptr);
    state.setProperty("roomSize", roomSize, nullptr);
    
    // Write the ValueTree to a stream
    juce::MemoryOutputStream stream(destData, true);
    state.writeToStream(stream);
}

void Echo1AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Read the ValueTree from the data
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    juce::ValueTree state = juce::ValueTree::readFromStream(stream);

    if (state.isValid())
    {
        dryWet = state.getProperty("dryWet", 0.05f);
        decayTime = state.getProperty("decayTime", 0.0f);
        roomSize = state.getProperty("roomSize", 0.1f);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Echo1AudioProcessor();
}
