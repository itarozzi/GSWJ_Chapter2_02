/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"



class NumericalLabel : public Label
{
public:
    void editorShown(TextEditor* editor) {
        editor->setInputRestrictions(0,"1234567890");
    }
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public Component,
                             public Button::Listener,
                             public Slider::Listener,
                             public Label::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked(Button*);
    void sliderValueChanged(Slider*);
    void labelTextChanged(Label*);

private:
    TextButton button1;     // Step [1]
    NumericalLabel label1;
    Slider slider1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};



#endif  // MAINCOMPONENT_H_INCLUDED
