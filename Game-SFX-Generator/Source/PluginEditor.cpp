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

    exportFileButton.setButtonText("Export as Wav");
    exportFileButton.onClick = [this]
    {
        audioProcessor.exportFilePrompt();
    };
    addAndMakeVisible(exportFileButton);

    playbackButton.setButtonText("PLAY");
    playbackButton.onClick = [this] 
    { 
        audioProcessor.setGate(playbackButton.getToggleState());
        audioProcessor.setPlayback(playbackButton.getToggleState());
        enableLoadFileButton(!playbackButton.getToggleState());
        enableExportButton(!playbackButton.getToggleState());
        enableRandSampleButton(!playbackButton.getToggleState());
        enableRandVibButton(!playbackButton.getToggleState());
        enableRandPitchEnvButton(!playbackButton.getToggleState());
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
    exportFileButton.setBounds(getWidth() - 100, 0, 100, 50);
    playbackButton.setBounds((getWidth()/2) - 25, 25, 100, 100);
    randomizeSampleGroup.setBounds(50, 100, 100, 100);
    randomizeVibratoGroup.setBounds(150, 100, 100, 100);
    randomizePitchEnvGroup.setBounds(250, 100, 100, 100);
}

void GameSFXGeneratorAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(bool state) {
    playbackButton.setToggleState(state, juce::dontSendNotification);
}

bool GameSFXGeneratorAudioProcessorEditor::getPlaybackToggleState() {
    return playbackButton.getToggleState();
}

juce::TextButton GameSFXGeneratorAudioProcessorEditor::loadFileButton;

void GameSFXGeneratorAudioProcessorEditor::enableLoadFileButton(bool state) {
    if (state) {
        loadFileButton.setEnabled(true);
    }
    else {
        loadFileButton.setEnabled(false);
    }
}

juce::ToggleButton GameSFXGeneratorAudioProcessorEditor::playbackButton;

void GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(bool state) {
    if (state) {
        playbackButton.setEnabled(true);
    }
    else {
        playbackButton.setEnabled(false);
    }
}

juce::TextButton GameSFXGeneratorAudioProcessorEditor::exportFileButton;

void GameSFXGeneratorAudioProcessorEditor::enableExportButton(bool state) {
    if (state) {
        exportFileButton.setEnabled(true);
    }
    else {
        exportFileButton.setEnabled(false);
    }
}

juce::TextButton GameSFXGeneratorAudioProcessorEditor::randomizeSampleGroup;

void GameSFXGeneratorAudioProcessorEditor::enableRandSampleButton(bool state) {
    if (state) {
        randomizeSampleGroup.setEnabled(true);
    }
    else {
        randomizeSampleGroup.setEnabled(false);
    }
}

juce::TextButton GameSFXGeneratorAudioProcessorEditor::randomizeVibratoGroup;

void GameSFXGeneratorAudioProcessorEditor::enableRandVibButton(bool state) {
    if (state) {
        randomizeVibratoGroup.setEnabled(true);
    }
    else {
        randomizeVibratoGroup.setEnabled(false);
    }
}

juce::TextButton GameSFXGeneratorAudioProcessorEditor::randomizePitchEnvGroup;

void GameSFXGeneratorAudioProcessorEditor::enableRandPitchEnvButton(bool state) {
    if (state) {
        randomizePitchEnvGroup.setEnabled(true);
    }
    else {
        randomizePitchEnvGroup.setEnabled(false);
    }
}