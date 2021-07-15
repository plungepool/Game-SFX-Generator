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
    loadFileButton.onClick = [this] {
        audioProcessor.loadFilePrompt();
    };
    addAndMakeVisible(loadFileButton);

    playbackButton.setButtonText("PLAY");
    playbackButton.onClick = [this] { 
        audioProcessor.setPlayback(playbackButton.getToggleState());
        audioProcessor.setGate(playbackButton.getToggleState()); 
    };
    addAndMakeVisible(playbackButton);

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
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    loadFileButton.setBounds(0, 0, 100, 50);
    playbackButton.setBounds(getWidth()-100, getHeight()-100, 100, 100);
}

void GameSFXGeneratorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
