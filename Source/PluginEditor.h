/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        // Background track
        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(x + width / 2 - 2, y, 4, height, 15.0f);

        
        
        // Slider thumb
        g.setColour(juce::Colours::snow);
        g.fillRoundedRectangle(x + width / 2.f - 15.5f, sliderPos - 8.f, 30.f, 15.f, 5.f);
        g.setColour(juce::Colours::silver);
        g.drawRoundedRectangle(x + width / 2.f - 15.5f, sliderPos - 8.f, 30.f, 15.f, 5.f, 1.0f);
    }
};

class Echo1AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener,
                                   public juce::Timer
{
public:
    Echo1AudioProcessorEditor (Echo1AudioProcessor&);
    ~Echo1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;
    
    void timerCallback() override;
    
    float getVolume() { return *audioProcessor.getVolume(); }
    
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Echo1AudioProcessor& audioProcessor;
    
    //=========================== Rectangle stuff ==============================
    
    juce::Rectangle<float> leftRect;
    
        juce::Rectangle<float> topLabelRect;
            juce::Rectangle<float> topLeftLabelRect;
            juce::Rectangle<float> topMiddleLabelRect;
            juce::Rectangle<float> topRightLabelRect;
        juce::Rectangle<float> sliderRect;
            juce::Rectangle<float> dryWetRect;
            juce::Rectangle<float> decayTimeRect;
            juce::Rectangle<float> roomSizeRect;
        juce::Rectangle<float> bottomLabelRect;
            juce::Rectangle<float> bottomLeftLabelRect;
            juce::Rectangle<float> bottomMiddleLabelRect;
            juce::Rectangle<float> bottomRightLabelRect;
    
    juce::Line<float> bigBorder;
    
    juce::Rectangle<float> rightRect;
    
        juce::Rectangle<float> verbWindow;
        float cornerRadius = 15.0f;
    
    //============================ Slider stuff ===============================
    
    juce::Slider dryWetSlider;
    juce::Slider decayTimeSlider;
    juce::Slider roomSizeSlider;
    
    
    
    std::vector<juce::Slider*> sliders = {&dryWetSlider, &decayTimeSlider, &roomSizeSlider};
    
    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    
    //====================== Volume helper variable ===========================
    
    float volumeLevel;
    
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Echo1AudioProcessorEditor)
};
