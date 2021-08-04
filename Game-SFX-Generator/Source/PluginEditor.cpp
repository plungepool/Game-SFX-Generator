/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GameSFXGeneratorAudioProcessorEditor::GameSFXGeneratorAudioProcessorEditor (GameSFXGeneratorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    loadFileButton.setButtonText("Open File...");
    loadFileButton.onClick = [this] 
    {
        audioProcessor.loadFilePrompt();
        audioProcessor.randomizePitch();
        audioProcessor.randomizeADSR();
        audioProcessor.randomizePitchEnvGroup();
        audioProcessor.randomizeVibratoGroup();
        audioProcessor.randomizedSampleId = NULL;
        sampleDebugText.setText(audioProcessor.sampleDebug());
        vibDebugText.setText(audioProcessor.vibDebug());
        pitchenvDebugText.setText(audioProcessor.pitchenvDebug());
        repaint(0, 0, getWidth(), getHeight());
    };
    addAndMakeVisible(loadFileButton);

    playbackButton.setButtonText("PLAY");
    playbackButton.onClick = [this] 
    { 
        audioProcessor.setGate(playbackButton.getToggleState());
        audioProcessor.setPlayback(playbackButton.getToggleState());
    };
    addAndMakeVisible(playbackButton);

    //RANDOM SAMPLE GROUP
    randomizeSampleGroup.setButtonText("Randomize Sample");
    randomizeSampleGroup.onClick = [this]
    {
        audioProcessor.randomizeSampleGroup();

        sampleDebugText.setText(audioProcessor.sampleDebug());
        repaint(0, 0, getWidth(), getHeight());
    };
    addAndMakeVisible(randomizeSampleGroup);

    //RANDOM VIBRATO GROUP
    randomizeVibratoGroup.setButtonText("Randomize Vibrato");
    randomizeVibratoGroup.onClick = [this]
    {
        audioProcessor.randomizeVibratoGroup();

        vibDebugText.setText(audioProcessor.vibDebug());
        repaint(0, 0, getWidth(), getHeight());
    };
    addAndMakeVisible(randomizeVibratoGroup);

    //RANDOM PITCH ENV GROUP
    randomizePitchEnvGroup.setButtonText("Randomize Pitch Env");
    randomizePitchEnvGroup.onClick = [this]
    {
        audioProcessor.randomizePitchEnvGroup();

        pitchenvDebugText.setText(audioProcessor.pitchenvDebug());
        repaint(0, 0, getWidth(), getHeight());
    };
    addAndMakeVisible(randomizePitchEnvGroup);

    setSize (400, 300);
}

GameSFXGeneratorAudioProcessorEditor::~GameSFXGeneratorAudioProcessorEditor()
{
}

//==============================================================================
void GameSFXGeneratorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    g.drawFittedText (sampleDebugText.getText(), getLocalBounds(), juce::Justification::bottomLeft, 1);
    g.drawFittedText(vibDebugText.getText(), getLocalBounds(), juce::Justification::centredBottom, 1);
    g.drawFittedText(pitchenvDebugText.getText(), getLocalBounds(), juce::Justification::bottomRight, 1);

    loadFileButton.setBounds(0, 0, 100, 50);
    playbackButton.setBounds(getWidth()-100, 25, 100, 100);
    randomizeSampleGroup.setBounds(50, 100, 100, 100);
    randomizeVibratoGroup.setBounds(150, 100, 100, 100);
    randomizePitchEnvGroup.setBounds(250, 100, 100, 100);
}

void GameSFXGeneratorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

juce::ToggleButton GameSFXGeneratorAudioProcessorEditor::playbackButton;

void GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(bool state) {
    playbackButton.setToggleState(state, juce::dontSendNotification);
}

bool GameSFXGeneratorAudioProcessorEditor::getPlaybackToggleState() {
    return playbackButton.getToggleState();
}

void GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(bool state) {
    if (state) {
        playbackButton.setEnabled(true);
    }
    else {
        playbackButton.setEnabled(false);
    }
}