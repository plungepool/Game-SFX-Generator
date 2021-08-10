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
class GameSFXGeneratorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GameSFXGeneratorAudioProcessorEditor (GameSFXGeneratorAudioProcessor&);
    ~GameSFXGeneratorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void static setPlaybackToggle(bool state);
    bool getPlaybackToggleState();

    void static enableLoadFileButton(bool state);
    void static enableLoadRandomSampleButton(bool state);
    void static enablePlaybackButton(bool state);
    void static enableExportButton(bool state);
    void static enableRandSampleButton(bool state);
    void static enableRandVibButton(bool state);
    void static enableRandPitchEnvButton(bool state);

private:
    juce::TextButton static loadFileButton;
    juce::TextButton static loadRandomSampleButton;
    juce::ToggleButton static playbackButton;
    juce::TextButton static exportFileButton;

    juce::TextButton static randomizeSampleGroup;
    juce::TextButton static randomizeVibratoGroup;
    juce::TextButton static randomizePitchEnvGroup;

    //Debug
    juce::DrawableText sampleDebugText;
    juce::DrawableText vibDebugText;
    juce::DrawableText pitchenvDebugText;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GameSFXGeneratorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GameSFXGeneratorAudioProcessorEditor)
};
