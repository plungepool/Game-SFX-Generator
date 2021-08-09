/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Faust/FaustMainProcessor.h"

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
    audioFormatManager.registerBasicFormats();
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

    transportSource.prepareToPlay(512, sampleRate);
    // Could load whole file with transportSource.getTotalLength()
    // but is it a good idea? 512 samples at a time should be enough
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

    audioWriter.release();
    transportSource.releaseResources();
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

    //Playback Sample
    juce::AudioSourceChannelInfo audioSourceChannelInfo(buffer);
    if (readerSource.get() == nullptr)
    {
        audioSourceChannelInfo.clearActiveBufferRegion();
        return;
    }
    transportSource.getNextAudioBlock(audioSourceChannelInfo);

    //Faust Processing
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

    if (exportInProgress){
        if (audioWriter != nullptr) {
            audioWriter->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
        }
    }

    if (transportSource.getCurrentPosition() >= (static_cast<double>(randomizedValueSusTime) + fUI->getParamValue("Env_Attack") + fUI->getParamValue("Env_Decay"))) {
        setGate(false);
    }
    
    disablePlaybackButtonIfEnvelopeClosed(buffer);
    disablePlaybackButtonIfStreamFinished(buffer);
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

//Files
void GameSFXGeneratorAudioProcessor::loadFilePrompt() 
{
    juce::FileChooser fileChooser("Choose a .wav file", 
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory), 
        "* .wav", true, false
        );

    if (fileChooser.browseForFileToOpen()) {
        auto currentFile = fileChooser.getResult();
        auto* audioReader = audioFormatManager.createReaderFor(currentFile);

        if (audioReader != nullptr) {
            std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(audioReader, true));
            transportSource.setSource(tempSource.get(), 0, nullptr, audioReader->sampleRate);
            readerSource.reset(tempSource.release());
            setGate(false);
            setTransport(false);
            resetTransportPosition();
            GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
            sampleRateDebug = getSampleRate();
        }
    }
}

void GameSFXGeneratorAudioProcessor::exportFilePrompt()
{
    GameSFXGeneratorAudioProcessorEditor::enableLoadFileButton(false);
    GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(false);
    GameSFXGeneratorAudioProcessorEditor::enableExportButton(false);
    GameSFXGeneratorAudioProcessorEditor::enableRandSampleButton(false);
    GameSFXGeneratorAudioProcessorEditor::enableRandVibButton(false);
    GameSFXGeneratorAudioProcessorEditor::enableRandPitchEnvButton(false);

    juce::FileChooser fileChooser("Choose a location",
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
        "* .wav", true, false
    );

    if (fileChooser.browseForFileToSave(true)) {
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(true);

        setGate(true);
        setTransport(true);

        exportInProgress = true;

        juce::File file(fileChooser.getResult());
        file.deleteFile();

        audioWriter.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file),
            44100,
            2,
            24,
            {},
            0));
    }
    else {
        GameSFXGeneratorAudioProcessorEditor::enableLoadFileButton(true);
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableExportButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandSampleButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandVibButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandPitchEnvButton(true);
    }
}

//Playback
void GameSFXGeneratorAudioProcessor::setTransport(bool gate) 
{
    if (gate) {
        transportSource.start();
    }
    else {
        transportSource.stop();
    }
}

void GameSFXGeneratorAudioProcessor::resetTransportPosition() {
    transportSource.setPosition(0.0);
}

void GameSFXGeneratorAudioProcessor::disablePlaybackButtonIfEnvelopeClosed(juce::AudioBuffer<float>& buffer) {
    double atkTime = fUI->getParamValue("Env_Attack");
    double dcyTime = fUI->getParamValue("Env_Decay");
    double susTime = randomizedValueSusTime;
    double relTime = fUI->getParamValue("Env_Release");
    if (transportSource.getCurrentPosition() >= ((atkTime + dcyTime + susTime + relTime) + 0.2)) {
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(false);
        if (exportInProgress) {
            audioWriter.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file),
                44100,
                2,
                24,
                {},
                0));
            exportInProgress = false;
        }
        buffer.clear();
        setGate(false);
        setTransport(false);
        resetTransportPosition();
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableLoadFileButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableExportButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandSampleButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandVibButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandPitchEnvButton(true);
    }
    else {
        return;
    }
}

void GameSFXGeneratorAudioProcessor::disablePlaybackButtonIfStreamFinished(juce::AudioBuffer<float>& buffer) {
    if (transportSource.hasStreamFinished()) {
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(false);
        if (exportInProgress) {
            audioWriter.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file),
                44100,
                2,
                24,
                {},
                0));
            exportInProgress = false;
        }
        buffer.clear();
        setGate(false);
        setTransport(false);
        resetTransportPosition();
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(true);\
        GameSFXGeneratorAudioProcessorEditor::enableLoadFileButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableExportButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandSampleButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandVibButton(true);
        GameSFXGeneratorAudioProcessorEditor::enableRandPitchEnvButton(true);
    }
    else {
        return;
    }
}

//Faust Setters
void GameSFXGeneratorAudioProcessor::setGate(bool gate)
{
    if (gate) {
        fUI->setParamValue("PLAY", 1);
    }
    else {
        fUI->setParamValue("PLAY", 0);
    }
}

void GameSFXGeneratorAudioProcessor::randomizeSampleGroup()
{
    randomizeSample();
    randomizePitch();
    randomizeADSR();
}

void GameSFXGeneratorAudioProcessor::randomizeSample()
{
    //randomizing code
    int min = 1;
    int max = BinaryData::namedResourceListSize;
    randomizedSampleId = randomInt(min, max);

    //get sample from binary resources
    int dataSize = 0;
    char const* binarySample = BinaryData::getNamedResource(BinaryData::namedResourceList[randomizedSampleId], dataSize);

    //create reader
    juce::MemoryInputStream* inputStream = new juce::MemoryInputStream(binarySample, dataSize, false);
    juce::AudioFormatReader* audioReader = wavFormat.createReaderFor(inputStream, true);

    //assign data to playback buffer
    if (audioReader != nullptr) {
        std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(audioReader, true));
        transportSource.setSource(tempSource.get(), 0, nullptr, audioReader->sampleRate);
        readerSource.reset(tempSource.release());
        setGate(false);
        setTransport(false);
        resetTransportPosition();
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        sampleRateDebug = getSampleRate();
    }
}

void GameSFXGeneratorAudioProcessor::randomizePitch()
{
    int pitchStyle = randomInt(0, 2);

    //Delay lines pitch shift
    if (!pitchStyle) {
        float res = 100;
        int mean = 0 * res;
        int stdDev = 15 * res;
        float randomizedValue = randomGaussian(mean, stdDev);
        fUI->setParamValue("PitchShift", randomizedValue / res);
    }

    //Linear interpolation/varispeed pitch shift
    else {
        fUI->setParamValue("PitchShift", 0);

        int min = getSampleRate() / 4;
        int max = getSampleRate() * 4;
        if ((transportSource.getTotalLength()/getSampleRate()) < 1.0f) {
            max = max * (transportSource.getTotalLength() / getSampleRate());
        }
        int randomizedValue = randomInt(min, max);

        int dataSize = 0;
        char const* binarySample = BinaryData::getNamedResource(BinaryData::namedResourceList[randomizedSampleId], dataSize);
        juce::MemoryInputStream* inputStream = new juce::MemoryInputStream(binarySample, dataSize, false);
        juce::AudioFormatReader* audioReader = wavFormat.createReaderFor(inputStream, true);
        if (audioReader != nullptr) {
            std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(audioReader, true));
            transportSource.setSource(tempSource.get(), 0, nullptr, randomizedValue);
            readerSource.reset(tempSource.release());
            setGate(false);
            setTransport(false);
            resetTransportPosition();
            GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
            sampleRateDebug = randomizedValue;
        }
    }
}

void GameSFXGeneratorAudioProcessor::randomizeADSR()
{
    float resAtk = 100;
    int minAtk = 0;
    int maxAtk = (transportSource.getTotalLength() / getSampleRate()) * resAtk;
    float randomizedValueAtk = randomNegBinomial(maxAtk, 0.75);
    fUI->setParamValue("Env_Attack", randomizedValueAtk / resAtk);

    float resDcy = 100;
    int minDcy = 0;
    int maxDcy = ((transportSource.getTotalLength() / getSampleRate()) * resDcy) - randomizedValueAtk;
    float randomizedValueDcy = randomInt(minDcy, maxDcy);
    if (randomizedValueDcy < 1) {
        randomizedValueDcy = 1;
    }
    fUI->setParamValue("Env_Decay", randomizedValueDcy / resDcy);

    float resSusTime = 100;
    int minSusTime = 0;
    int maxSusTime = ((transportSource.getTotalLength() / getSampleRate()) * resSusTime) - randomizedValueAtk - randomizedValueDcy;
    randomizedValueSusTime = randomInt(minSusTime, maxSusTime);
    if (randomizedValueSusTime < 1) {
        randomizedValueSusTime = 1;
    }
    randomizedValueSusTime = randomizedValueSusTime / resSusTime;

    float resRel = 100;
    int minRel = 0;
    int maxRel = ((transportSource.getTotalLength() / getSampleRate()) * resRel) - randomizedValueAtk - randomizedValueDcy - randomizedValueSusTime;
    float randomizedValueRel = randomInt(minRel, maxRel);
    if (randomizedValueRel < 1) {
        randomizedValueRel = 1;
    }
    fUI->setParamValue("Env_Release", randomizedValueRel / resRel);

    float resSusRatio = 1000;
    int minSusRatio = 1;
    int maxSusRatio = 1000;
    float randomizeSusRatio = randomInt(minSusRatio, maxSusRatio);
    fUI->setParamValue("Env_Sustain", randomizeSusRatio / resSusRatio);
}

void GameSFXGeneratorAudioProcessor::randomizeVibratoGroup()
{
    randomizeVibRateEnvAttack();
    randomizeVibRateEnvDecay();
    randomizeVibRateEnvSustain();
    randomizeVibRateEnvDepth();
    randomizeVibRate();
    randomizeVibDutyCycle();
    randomizeVibShape();
    randomizeVibDepth();
    randomizeVibDelay();
}

void GameSFXGeneratorAudioProcessor::randomizeVibRateEnvAttack()
{
    float res = 100;
    int min = 0;
    int max = (transportSource.getTotalLength() / getSampleRate()) * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("VibRateEnv_Attack", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibRateEnvDecay()
{
    float res = 100;
    int min = 0;
    int max = (transportSource.getTotalLength() / getSampleRate()) * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("VibRateEnv_Decay", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibRateEnvSustain()
{
    float res = 100;
    int min = 0;
    int max = 1 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("VibRateEnv_Sustain", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibRateEnvDepth()
{
    float res = 10;
    int min = 0;
    int max = 10 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("VibRateEnv_Depth", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibRate()
{
    float res = 10;
    int min = 0;
    int max = 25 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("Vib_Freq", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibDutyCycle()
{
    float res = 4;
    int min = 0;
    int max = 1 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("Vib_Duty", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibShape()
{
    int minSubset = 0;
    int maxSubset = 3;
    float randomizedValueSubset = randomInt(minSubset, maxSubset);
    fUI->setParamValue("Vib_LFOShape_Sync", randomizedValueSubset);
    fUI->setParamValue("Vib_LFOShape_Unsync", randomizedValueSubset);

    int min = 0;
    int max = 2;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("Vib_LFOShape", randomizedValue);
}

void GameSFXGeneratorAudioProcessor::randomizeVibDepth()
{
    float res = 10;
    int min = 0;
    int max = 20 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("Vib_Depth", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeVibDelay()
{
    int min = 0;
    int max = transportSource.getTotalLength();
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("Vib_Delay", randomizedValue);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvGroup()
{
    randomizePitchEnvAttack();
    randomizePitchEnvDecay();
    randomizePitchEnvSustain();
    randomizePitchEnvGateSource();
    randomizePitchEnvDepth();
    randomizePitchEnvDelay();
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvAttack()
{
    float res = 100;
    int min = 0;
    int max = (transportSource.getTotalLength() / getSampleRate()) * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_Attack", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvDecay()
{
    float res = 100;
    int min = 0;
    int max = (transportSource.getTotalLength() / getSampleRate()) * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_Decay", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvSustain()
{
    float res = 100;
    int min = 0;
    int max = 1 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_Sustain", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvGateSource()
{
    int min = 0;
    int max = 2;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_AdsrGate", randomizedValue);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvDepth()
{
    float res = 10;
    int min = -20 * res;
    int max = 20 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_Depth", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizePitchEnvDelay()
{
    int min = 0;
    int max = transportSource.getTotalLength();
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchEnv_Delay", randomizedValue);
}

void GameSFXGeneratorAudioProcessor::randomizeAll()
{
    randomizeSampleGroup();

    randomizeVibratoGroup();

    randomizePitchEnvGroup();
}

//Debug
std::string GameSFXGeneratorAudioProcessor::sampleDebug() {
    std::string pitchDebug = std::to_string(fUI->getParamValue("PitchShift"));
    std::string envatkDebug = std::to_string(fUI->getParamValue("Env_Attack"));
    std::string envdcyDebug = std::to_string(fUI->getParamValue("Env_Decay"));
    std::string envsusDebug = std::to_string(randomizedValueSusTime);
    std::string envsusTimeDebug = std::to_string(fUI->getParamValue("Env_Sustain"));
    std::string envrelDebug = std::to_string(fUI->getParamValue("Env_Release"));
    std::string samplerateDebug = std::to_string(sampleRateDebug);
    std::string samplelengthDebug = std::to_string(transportSource.getTotalLength());

    std::string sampleDebugText = std::string(BinaryData::namedResourceList[randomizedSampleId]) + "\n" +
                                    "# of samples: " + std::to_string(BinaryData::namedResourceListSize) + "\n" +     
                                    "Pitch: " + pitchDebug.substr(0, 5) + " semi\n" +
                                    "A:" + envatkDebug.substr(0, 4) + "s/" +
                                    "D:" + envdcyDebug.substr(0, 4) + "s\n" +
                                    "S:" + envsusTimeDebug.substr(0, 4) + "s " + envsusDebug.substr(1, 3) + "%/" +
                                    "R:" + envrelDebug.substr(0, 4) + "s\n" +
                                    "Sample Rate: " + samplerateDebug.substr(0, 5) + "\n" +
                                    "Length: " + samplelengthDebug;

    return sampleDebugText;
}

std::string GameSFXGeneratorAudioProcessor::vibDebug()
{
    std::string vibrateDebug = std::to_string(fUI->getParamValue("Vib_Freq"));
    std::string vibdutyDebug = std::to_string(fUI->getParamValue("Vib_Duty"));
    std::string vibshapeDebug = (std::to_string(fUI->getParamValue("Vib_LFOShape"))).substr(0, 1) + "." +
                                (std::to_string(fUI->getParamValue("Vib_LFOShape_Sync"))).substr(0, 1);
    std::string vibdepthDebug = std::to_string(fUI->getParamValue("Vib_Depth"));
    std::string vibdelayDebug = std::to_string(fUI->getParamValue("Vib_Delay") / getSampleRate());
    std::string ratemodDebug =  (std::to_string(fUI->getParamValue("VibRateEnv_Attack"))).substr(0, 3) + "s/" +
                                (std::to_string(fUI->getParamValue("VibRateEnv_Decay"))).substr(0, 3) + "s/" +
                                (std::to_string(fUI->getParamValue("VibRateEnv_Sustain"))).substr(0, 3) + "/" +
                                (std::to_string(fUI->getParamValue("VibRateEnv_Depth"))).substr(0, 3) + "d";

    std::string vibDebugText =  "Rate: " + vibrateDebug.substr(0,4) + " hz\n" +
                                "Duty: " + vibdutyDebug.substr(0, 4) + "%\n" +
                                "Shape: " + vibshapeDebug + "\n" +
                                "Depth: " + vibdepthDebug.substr(0, 4) + "\n" +
                                "Delay: " + vibdelayDebug.substr(0, 4) + " sec\n"
                                "RateMod: " + ratemodDebug + "\n";

    return vibDebugText;
}

std::string GameSFXGeneratorAudioProcessor::pitchenvDebug()
{
    std::string peatkDebug = std::to_string(fUI->getParamValue("PitchEnv_Attack"));
    std::string pedcyDebug = std::to_string(fUI->getParamValue("PitchEnv_Decay"));
    std::string pesusDebug = std::to_string(fUI->getParamValue("PitchEnv_Sustain"));
    std::string pedepthDebug = std::to_string(fUI->getParamValue("PitchEnv_Depth"));
    std::string pedelayDebug = std::to_string(fUI->getParamValue("PitchEnv_Delay") / getSampleRate());
    std::string pegateDebug = std::to_string(fUI->getParamValue("PitchEnv_AdsrGate"));

    std::string pitchenvDebugText = "Atk: " + peatkDebug.substr(0, 4) + " sec\n" +
                                    "Dcy: " + pedcyDebug.substr(0, 4) + " sec\n" +
                                    "Sus: " + pesusDebug.substr(0, 4) + "\n" +
                                    "Depth: " + pedepthDebug.substr(0, 4) + "\n" +
                                    "Delay: " + pedelayDebug.substr(0, 4) + " sec\n" +
                                    "Gate: " + pegateDebug.substr(0, 1) + "\n";

    return pitchenvDebugText;
}

//Random Number Functions
int GameSFXGeneratorAudioProcessor::randomInt(float min, float max)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(min, max);

    return dist(mt);
}

int GameSFXGeneratorAudioProcessor::randomGaussian(float mean, float stdDev) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<double> dist(mean, stdDev);

    return dist(mt);
}

int GameSFXGeneratorAudioProcessor::randomNegBinomial(float max, float ratio) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::negative_binomial_distribution<long> dist(max, ratio);
    //not sure how this type of distribution works exactly yet
    return dist(mt);
}