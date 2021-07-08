/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "Faust/FaustMainProcessor.h"
#include "Faust/FaustEffectTest.h"

//==============================================================================
GameSFXGeneratorAudioProcessor::GameSFXGeneratorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GameSFXGeneratorAudioProcessor::~GameSFXGeneratorAudioProcessor()
{
}

//==============================================================================
const juce::String GameSFXGeneratorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GameSFXGeneratorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GameSFXGeneratorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GameSFXGeneratorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GameSFXGeneratorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GameSFXGeneratorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GameSFXGeneratorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GameSFXGeneratorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GameSFXGeneratorAudioProcessor::getProgramName (int index)
{
    return {};
}

void GameSFXGeneratorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GameSFXGeneratorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fDSP = new mydsp();
    fDSP->init(sampleRate);
    fUI = new MapUI();
    fDSP->buildUserInterface(fUI);
    inputs = new float* [2];
    outputs = new float* [2];
    for (int channel = 0; channel < 2; ++channel) {
        inputs[channel] = new float[samplesPerBlock];
        outputs[channel] = new float[samplesPerBlock];
    }
}

void GameSFXGeneratorAudioProcessor::releaseResources()
{
    delete fDSP;
    delete fUI;
    for (int channel = 0; channel < 2; ++channel) {
        delete[] inputs[channel];
        delete[] outputs[channel];
    }
    delete[] inputs;
    delete[] outputs;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GameSFXGeneratorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GameSFXGeneratorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            inputs[channel][i] = *buffer.getWritePointer(channel, i);
        }
    }

    fDSP->compute(buffer.getNumSamples(), inputs, outputs);

    for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            *buffer.getWritePointer(channel, i) = outputs[channel][i];
        }
    }
}

//==============================================================================
bool GameSFXGeneratorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GameSFXGeneratorAudioProcessor::createEditor()
{
    return new GameSFXGeneratorAudioProcessorEditor (*this);
}

//==============================================================================
void GameSFXGeneratorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GameSFXGeneratorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GameSFXGeneratorAudioProcessor();
}

//==============================================================================
void GameSFXGeneratorAudioProcessor::setGate(bool gate)
{
    if (gate) {
        fUI->setParamValue("adsr_gate", 1);
    }
    else {
        fUI->setParamValue("adsr_gate", 0);
    }
}
