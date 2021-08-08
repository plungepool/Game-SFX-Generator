/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <random>

//==============================================================================
/**
*/
class dsp;
class MapUI;

class GameSFXGeneratorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GameSFXGeneratorAudioProcessor();
    ~GameSFXGeneratorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    //Files
    void loadFilePrompt();
    void exportFilePrompt();
    int randomizedSampleId;

    //Playback
    void setPlayback(bool gate);
    void disablePlaybackButtonIfEnvelopeClosed(juce::AudioBuffer<float>& buffer);
    void disablePlaybackButtonIfStreamFinished(juce::AudioBuffer<float>& buffer);

    //Faust Setters
    void setGate(bool gate);

    void randomizeSampleGroup();
    void randomizeSample();
    void randomizePitch();
    void randomizeADSR();

    void randomizeVibratoGroup();
    void randomizeVibRateEnvAttack();
    void randomizeVibRateEnvDecay();
    void randomizeVibRateEnvSustain();
    void randomizeVibRateEnvDepth();
    void randomizeVibRate();
    void randomizeVibDutyCycle();
    void randomizeVibShape();
    void randomizeVibDepth();
    void randomizeVibDelay();

    void randomizePitchEnvGroup();
    void randomizePitchEnvAttack();
    void randomizePitchEnvDecay();
    void randomizePitchEnvSustain();
    void randomizePitchEnvGateSource();
    void randomizePitchEnvDepth();
    void randomizePitchEnvDelay();

    void randomizeAll();

    //Debug
    std::string static fileDebug;
    std::string sampleDebug();
    int sampleRateDebug;
    std::string vibDebug();
    std::string pitchenvDebug();

private:
    //Audio I/O
    juce::AudioFormatManager audioFormatManager;
    juce::WavAudioFormat wavFormat;
    std::unique_ptr <juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioFormatWriter> audioWriter;
    juce::AudioTransportSource transportSource;

    juce::File file;
    bool exportInProgress = false;

    //Faust I/O
    MapUI* fUI;
    dsp* fDSP;
    float** inputs;
    float** outputs;

    //Random Number Functions
    int randomInt(float min, float max);
    int randomGaussian(float mean, float stdDev);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GameSFXGeneratorAudioProcessor)
};
