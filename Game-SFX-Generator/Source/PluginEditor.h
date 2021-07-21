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

private:
    juce::TextButton loadFileButton;
    juce::ToggleButton static playbackButton;

    juce::TextButton randomizeSampleGroup;
    juce::TextButton randomizeVibratoGroup;
    juce::TextButton randomizePitchEnvGroup;

    //Debug
    juce::DrawableText sampleDebugText;
    juce::DrawableText vibDebugText;
    juce::DrawableText pitchenvDebugText;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GameSFXGeneratorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GameSFXGeneratorAudioProcessorEditor)
};
