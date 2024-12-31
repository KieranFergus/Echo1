/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
Echo1AudioProcessorEditor::Echo1AudioProcessorEditor (Echo1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();
    
    for ( auto* slider : sliders ){
        slider->setSliderStyle(juce::Slider::LinearVertical);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
        slider->setPopupDisplayEnabled(false, false, this);
    }
    
    //=============================================================
    
    dryWetSlider.setValue(0.05);
    dryWetSlider.setRange(0.05, 1.0);
    dryWetSlider.setSkewFactor(0.4);
    
    dryWetSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(dryWetSlider);
    dryWetSlider.addListener(this);
    
    //=============================================================
    
    decayTimeSlider.setValue(0.00);
    decayTimeSlider.setRange(0.00, 0.9);
    decayTimeSlider.setSkewFactor(0.3);
    
    decayTimeSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(decayTimeSlider);
    decayTimeSlider.addListener(this);
    
    //=============================================================
    
    roomSizeSlider.setValue(0.1);
    roomSizeSlider.setRange(0.1, 1.0);
    roomSizeSlider.setSkewFactor(0.47);
    
    roomSizeSlider.setLookAndFeel(customLookAndFeel.get());
    addAndMakeVisible(roomSizeSlider);
    roomSizeSlider.addListener(this);
    
    //==================== Restore State ==========================
    
    dryWetSlider.setValue(*audioProcessor.getDryWet());
    decayTimeSlider.setValue(*audioProcessor.getDecayTime());
    roomSizeSlider.setValue(*audioProcessor.getRoomSize());
    
    
    setSize(600, 400);
    startTimerHz(30);
   
}

Echo1AudioProcessorEditor::~Echo1AudioProcessorEditor()
{
//    dryWetSlider.removeListener(this);
//    decayTimeSlider.removeListener(this);
//    roomSizeSlider.removeListener(this);
    
    for ( auto* slider : sliders ) {
        slider->removeListener(this);
        slider->setLookAndFeel(nullptr);
    }
    stopTimer();
    
//    dryWetSlider.setLookAndFeel(nullptr);
//    decayTimeSlider.setLookAndFeel(nullptr);
//    roomSizeSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void Echo1AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &dryWetSlider) {
        audioProcessor.setDryWet( dryWetSlider.getValue() );
    } else if (slider == &decayTimeSlider) {
        audioProcessor.setDecayTime( decayTimeSlider.getValue() );
    } else if (slider == &roomSizeSlider) {
        audioProcessor.setRoomSize( roomSizeSlider.getValue() );
    }
    repaint();
    
}


void Echo1AudioProcessorEditor::timerCallback()
{
    repaint();
}

//==============================================================================
void Echo1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::snow);
    
    auto lighterpurple = juce::Colours::violet.interpolatedWith(juce::Colours::white, 0.2f);
    
    
    //============================ Labels =====================================
    
    g.setColour(lighterpurple);
    g.drawFittedText("Wet", topLeftLabelRect.toNearestInt(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Long", topMiddleLabelRect.toNearestInt(), juce::Justification::centredBottom, 1);
    g.drawFittedText("Hall", topRightLabelRect.toNearestInt(), juce::Justification::centredBottom, 1);
    
    g.setColour(juce::Colours::skyblue);
    g.drawFittedText("Dry", bottomLeftLabelRect.toNearestInt(), juce::Justification::centredTop, 1);
    g.drawFittedText("Short", bottomMiddleLabelRect.toNearestInt(), juce::Justification::centredTop, 1);
    g.drawFittedText("Room", bottomRightLabelRect.toNearestInt(), juce::Justification::centredTop, 1);
    
    //========================== verbWindow ============================
   
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(verbWindow, cornerRadius);
    
    
            //============== Background Color (dryWet) =================
    
    auto blendedColor2 = juce::Colours::white.interpolatedWith(lighterpurple, dryWetSlider.getValue()); // 50% mix
    
    g.setColour(blendedColor2);
    
    g.fillRoundedRectangle(verbWindow, cornerRadius);
    
    g.setColour(juce::Colours::silver);
    g.drawRoundedRectangle(verbWindow, cornerRadius, 1.0f);
    
    
    //================= Reverb Circle (roomSize, decayTime) ===================
    
    
    //auto circleColor = juce::Colours::darkslateblue;
    float radius = roomSizeSlider.getValue() * 120.f;
    float centerX = 400.f;
    float centerY = 200.f;
    float thickness = 1.5f - decayTimeSlider.getValue();
    

    
    
    g.setColour(juce::Colours::violet);
    g.drawEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f, thickness);
    
    
    g.drawEllipse(centerX - radius - decayTimeSlider.getValue() * 5.f * 2.25f,
                  centerY - radius - decayTimeSlider.getValue() * 5.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 10.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 10.f * 2.25f,
                  thickness / 1.5f);
    
    g.drawEllipse(centerX - radius - decayTimeSlider.getValue() * 10.f * 2.25f,
                  centerY - radius - decayTimeSlider.getValue() * 10.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 20.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 20.f * 2.25f,
                  thickness / 2.0f);
    
    g.drawEllipse(centerX - radius - decayTimeSlider.getValue() * 15.f * 2.25f,
                  centerY - radius - decayTimeSlider.getValue() * 15.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 30.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 30.f * 2.25f,
                  thickness / 2.5f);
    
    g.drawEllipse(centerX - radius - decayTimeSlider.getValue() * 20.f * 2.25f,
                  centerY - radius - decayTimeSlider.getValue() * 20.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 40.f * 2.25f,
                  (radius * 2.0f) + decayTimeSlider.getValue() * 40.f * 2.25f,
                  thickness / 3.0f);
    
    //======================= Volume Circle (volume) =========================
   
    // Calculate the inner circle radius based on volumeLevel
    float volumeRadius = *audioProcessor.getVolume() * radius; // Scaled by volume level

    
    juce::ColourGradient gradient(lighterpurple,
                                  centerX,
                                  centerY,  // Center color
                                  blendedColor2,
                                  centerX,
                                  centerY + volumeRadius,
                                  true); // Edge color
    gradient.addColour(0.1, juce::Colours::violet); // Optional: Add an

    // Apply the gradient
    g.setGradientFill(gradient);
    g.fillEllipse(centerX - volumeRadius, centerY - volumeRadius, volumeRadius * 2.0f, volumeRadius * 2.0f);

}

void Echo1AudioProcessorEditor::resized()
{
    //=========================== Rectangle Stuff ==============================
    auto bounds = getLocalBounds().toFloat();
    
    leftRect = bounds.removeFromLeft( bounds.getWidth() / 3 );
        topLabelRect = leftRect.removeFromTop( leftRect.getHeight() / 8 );
            topLeftLabelRect = topLabelRect.removeFromLeft( topLabelRect.getWidth() / 3 );
            topMiddleLabelRect = topLabelRect.removeFromLeft( topLabelRect.getWidth() / 2 );
            topRightLabelRect = topLabelRect;
        bottomLabelRect = leftRect.removeFromBottom( leftRect.getHeight() / 6 );
            bottomLeftLabelRect = bottomLabelRect.removeFromLeft( bottomLabelRect.getWidth() / 3 );
            bottomMiddleLabelRect = bottomLabelRect.removeFromLeft( bottomLabelRect.getWidth() / 2 ) ;
            bottomRightLabelRect = bottomLabelRect;
    
        sliderRect = leftRect;
            dryWetRect = leftRect.removeFromLeft( leftRect.getWidth() / 3 );
            decayTimeRect = leftRect.removeFromLeft( leftRect.getWidth() / 2 );
            roomSizeRect = leftRect;
    
    bigBorder.setStart( leftRect.getRight(),
                        leftRect.getBottom() );
    bigBorder.setEnd( leftRect.getRight(),
                      leftRect.getY() );
    
    rightRect = bounds;
        verbWindow = rightRect;
        verbWindow.reduce( verbWindow.getWidth() / 20.f,
                           verbWindow.getHeight() / 20.f );
    
    //============================ Slider Stuff ===============================
    
    dryWetSlider.setBounds(dryWetRect.toNearestInt());
    decayTimeSlider.setBounds(decayTimeRect.toNearestInt());
    roomSizeSlider.setBounds(roomSizeRect.toNearestInt());
}
