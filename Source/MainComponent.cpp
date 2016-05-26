/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
:button1("Click me"),
 slider1(Slider::LinearHorizontal, Slider::NoTextBox)

{
    slider1.setRange(0.0, 100.0);


    // Aggiungere i listener -> questa classe deve implementare le funzioni del listener
    slider1.addListener(this);
    button1.addListener(this);
    label1.addListener(this);

    label1.setEditable(true);

    //slider1.setSliderStyle(Slider::LinearBar);
    slider1.setValue(100,sendNotification);

    addAndMakeVisible(&button1);
    addAndMakeVisible(&label1);
    addAndMakeVisible(&slider1);
    setSize (200, 100);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xfffffFff));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
//    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    button1.setBoundsRelative(0.05, 0.05, 0.9, 0.25);
    slider1.setBoundsRelative(0.05, 0.65, 0.9, 0.25);
    label1.setBoundsRelative(0.05, 0.35, 0.9, 0.25);
}

void MainContentComponent::buttonClicked(Button* button){
    if (&button1 == button) {
        label1.setText("0", sendNotification);
    }

}

void MainContentComponent::sliderValueChanged(Slider* slider){
    if (&slider1 == slider) {
        int value = slider1.getValue();
        label1.setText(String(value), sendNotification);

        if (value < 30) {
            slider1.setColour(Slider::thumbColourId, Colour(0xff00ff00));
        } else if (value < 70) {
            slider1.setColour(Slider::thumbColourId, Colours::yellow);
        } else {
            slider1.setColour(Slider::thumbColourId, Colour(255,0,0));
        }

    }
}

void MainContentComponent::labelTextChanged(Label* label) {
    if (&label1 == label) {
        slider1.setValue(label1.getText().getDoubleValue(), dontSendNotification);
    }
}
