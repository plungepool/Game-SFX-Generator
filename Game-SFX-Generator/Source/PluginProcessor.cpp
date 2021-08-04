/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Faust/FaustMainProcessor.h"
#include "SQLite/sqlite3.h"

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
            setPlayback(false);
            GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        }
    }
}

int GameSFXGeneratorAudioProcessor::getNumberOfSamplesInDatabase() {
    //return SQL count query
    return 1;
}

std::string GameSFXGeneratorAudioProcessor::getSampleNameFromDatabaseById(int id) {
    std::string currentFilePath;
    std::string id_string = "SELECT filename FROM files WHERE id = " + std::to_string(id);
    char const* querySelectById = id_string.c_str();

    sqlite3* connection = nullptr;
    int result = sqlite3_open("C:/Users/cyclpsrock/Dropbox/Programming/Project Repos/Game-SFX-Generator/Game-SFX-Generator/Source/SQLite/audioDB/audio.db", &connection);
    //":memory:" instead of path to create an in-memory database
    
    sqlite3_stmt* query = nullptr;
    result = sqlite3_prepare_v2(connection, querySelectById, -1, &query, nullptr);

    if (SQLITE_OK != result) {
        //error handling
    }

    while (SQLITE_ROW == sqlite3_step(query)) {
        currentFilePath = std::string(reinterpret_cast<const char*>(sqlite3_column_text(query, 0)));
    }

    sqlite3_finalize(query);
    sqlite3_close(connection);

    return currentFilePath;
}

//Playback
void GameSFXGeneratorAudioProcessor::setPlayback(bool gate) 
{
    if (gate) {
        transportSource.start();
    }
    else {
        transportSource.stop();
        transportSource.setPosition(0.0);
    }
}

void GameSFXGeneratorAudioProcessor::disablePlaybackButtonIfEnvelopeClosed(juce::AudioBuffer<float>& buffer) {
    double atkTime = fUI->getParamValue("Env_Attack");
    double dcyTime = fUI->getParamValue("Env_Decay");
    if (transportSource.getCurrentPosition() >= (atkTime + dcyTime)) {
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(false);
        buffer.clear();
        setGate(false);
        setPlayback(false);
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(true);
    }
    else {
        return;
    }
}

void GameSFXGeneratorAudioProcessor::disablePlaybackButtonIfStreamFinished(juce::AudioBuffer<float>& buffer) {
    if (transportSource.hasStreamFinished()) {
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(false);
        buffer.clear();
        setGate(false);
        setPlayback(false);
        GameSFXGeneratorAudioProcessorEditor::setPlaybackToggle(false);
        GameSFXGeneratorAudioProcessorEditor::enablePlaybackButton(true);
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
    //TODO
    int min = 1;
    int max = getNumberOfSamplesInDatabase();

    //get sample from binary resources
    char const* binarySampleName = getSampleNameFromDatabaseById(50).c_str();
    int dataSize;
    auto* binarySample = BinaryData::getNamedResource(binarySampleName, dataSize);

    //fileDebug = binarySampleName;

    //assign data to playback buffer
    //Need this to test if database works
}

void GameSFXGeneratorAudioProcessor::randomizePitch()
{
    float res = 100;
    int min = -48 * res;
    int max = 48 * res;
    float randomizedValue = randomInt(min, max);
    fUI->setParamValue("PitchShift", randomizedValue / res);
}

void GameSFXGeneratorAudioProcessor::randomizeADSR()
{
    float resAtk = 100;
    int minAtk = 0;
    int maxAtk = (transportSource.getTotalLength() / getSampleRate()) * resAtk;
    float randomizedValueAtk = randomInt(minAtk, maxAtk);
    fUI->setParamValue("Env_Attack", randomizedValueAtk / resAtk);

    float resDcy = 100;
    int minDcy = 0;
    int maxDcy = ((transportSource.getTotalLength() / getSampleRate()) * resDcy) - randomizedValueAtk;
    float randomizedValueDcy = randomInt(minDcy, maxDcy);
    if (randomizedValueDcy < 1) {
        randomizedValueDcy = 1;
    }
    fUI->setParamValue("Env_Decay", randomizedValueDcy / resDcy);
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
    std::string samplerateDebug = std::to_string(getSampleRate());
    std::string samplelengthDebug = std::to_string(transportSource.getTotalLength());

    std::string sampleDebugText = getSampleNameFromDatabaseById(1) + "\n" +
                                    "Pitch: " + pitchDebug.substr(0, 5) + " semi\n" +
                                    "Atk: " + envatkDebug.substr(0, 4) + " sec\n" +
                                    "Dcy: " + envdcyDebug.substr(0, 4) + " sec\n" +
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