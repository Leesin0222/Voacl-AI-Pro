#include "VocalAIProPlugin.h"
#include "VocalAIProEditor.h"
#include "AIPitchTuner.h"
#include "VocalEffects.h"

//==============================================================================
VocalAIProPlugin::VocalAIProPlugin()
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      parameters (*this, nullptr, "VocalAIProParameters", createParameterLayout())
{
    // Initialize AI Pitch Tuning Parameters
    pitchCorrectionParam = parameters.getRawParameterValue("pitchCorrection");
    pitchSpeedParam = parameters.getRawParameterValue("pitchSpeed");
    pitchCorrectionEnabledParam = parameters.getRawParameterValue("pitchCorrectionEnabled");
    
    // Initialize Vocal Effects Parameters
    reverbAmountParam = parameters.getRawParameterValue("reverbAmount");
    delayTimeParam = parameters.getRawParameterValue("delayTime");
    delayFeedbackParam = parameters.getRawParameterValue("delayFeedback");
    harmonyAmountParam = parameters.getRawParameterValue("harmonyAmount");
    harmonyVoicesParam = parameters.getRawParameterValue("harmonyVoices");
    
    // Initialize Master Parameters
    inputGainParam = parameters.getRawParameterValue("inputGain");
    outputGainParam = parameters.getRawParameterValue("outputGain");
    bypassParam = parameters.getRawParameterValue("bypass");
    
    // Initialize AI Components
    aiPitchTuner = std::make_unique<AIPitchTuner>();
    vocalEffects = std::make_unique<VocalEffects>();
    
    // Add parameter listener
    parameters.addParameterListener("pitchCorrection", this);
    parameters.addParameterListener("pitchSpeed", this);
    parameters.addParameterListener("pitchCorrectionEnabled", this);
    parameters.addParameterListener("reverbAmount", this);
    parameters.addParameterListener("delayTime", this);
    parameters.addParameterListener("delayFeedback", this);
    parameters.addParameterListener("harmonyAmount", this);
    parameters.addParameterListener("harmonyVoices", this);
    parameters.addParameterListener("inputGain", this);
    parameters.addParameterListener("outputGain", this);
    parameters.addParameterListener("bypass", this);
}

VocalAIProPlugin::~VocalAIProPlugin()
{
    // std::unique_ptr automatically handles cleanup
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout VocalAIProPlugin::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // AI Pitch Tuning Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitchCorrection", "Pitch Correction", 
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pitchSpeed", "Pitch Speed", 
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "pitchCorrectionEnabled", "Pitch Correction Enabled", true));
    
    // Vocal Effects Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbAmount", "Reverb Amount", 
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayTime", "Delay Time", 
        juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f), 250.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayFeedback", "Delay Feedback", 
        juce::NormalisableRange<float>(0.0f, 95.0f, 0.1f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "harmonyAmount", "Harmony Amount", 
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "harmonyVoices", "Harmony Voices", 1, 8, 2));
    
    // Master Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "inputGain", "Input Gain", 
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "outputGain", "Output Gain", 
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));
    
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String VocalAIProPlugin::getName() const
{
    return JucePlugin_Name;
}

bool VocalAIProPlugin::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocalAIProPlugin::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocalAIProPlugin::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocalAIProPlugin::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalAIProPlugin::getNumPrograms()
{
    return 1;
}

int VocalAIProPlugin::getCurrentProgram()
{
    return 0;
}

void VocalAIProPlugin::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String VocalAIProPlugin::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void VocalAIProPlugin::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void VocalAIProPlugin::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    try
    {
        // Validate parameters
        if (sampleRate <= 0.0 || samplesPerBlock <= 0)
        {
            jassertfalse; // Debug assertion
            return;
        }
        
        currentSampleRate = sampleRate;
        currentBlockSize = samplesPerBlock;
        
        // Prepare DSP components
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
        
        inputGain.prepare(spec);
        outputGain.prepare(spec);
        dryWetMixer.prepare(spec);
        
        // Initialize parameter smoothers
        inputGainSmoother.reset(sampleRate, 0.05); // 50ms smoothing
        outputGainSmoother.reset(sampleRate, 0.05);
        reverbAmountSmoother.reset(sampleRate, 0.1); // 100ms smoothing
        delayTimeSmoother.reset(sampleRate, 0.2); // 200ms smoothing
        
        // Prepare AI components with error checking
        if (aiPitchTuner)
        {
            aiPitchTuner->prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
        }
        else
        {
            jassertfalse; // AI Pitch Tuner not initialized
        }
        
        if (vocalEffects)
        {
            vocalEffects->prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
        }
        else
        {
            jassertfalse; // Vocal Effects not initialized
        }
        
        isInitialized = true;
        updateParameters();
    }
    catch (const std::exception& e)
    {
        // Log error and reset to safe state
        DBG("Error in prepareToPlay: " << e.what());
        isInitialized = false;
    }
}

void VocalAIProPlugin::releaseResources()
{
    isInitialized = false;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocalAIProPlugin::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VocalAIProPlugin::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update parameters
    updateParameters();
    
    // Bypass check
    if (bypassParam->load() > 0.5f)
    {
        return;
    }
    
    // Create processing context
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Apply input gain with safety check
    if (inputGainSmoother.getCurrentValue() < 10.0f) // Prevent excessive gain
    {
        inputGain.process(context);
    }
    
    // Store dry signal for dry/wet mixing
    auto dryBlock = block;
    
    // AI Pitch Tuning
    if (aiPitchTuner && pitchCorrectionEnabledParam->load() > 0.5f)
    {
        aiPitchTuner->processBlock(buffer, midiMessages);
    }
    
    // Vocal Effects
    if (vocalEffects)
    {
        vocalEffects->processBlock(buffer, midiMessages);
    }
    
    // Apply output gain with safety check
    if (outputGainSmoother.getCurrentValue() < 10.0f) // Prevent excessive gain
    {
        outputGain.process(context);
    }
    
    // Apply dry/wet mix (safe 50% wet to prevent feedback)
    dryWetMixer.setWetMixProportion(0.5f);
    dryWetMixer.mixWetSamples(block);
}

//==============================================================================
bool VocalAIProPlugin::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VocalAIProPlugin::createEditor()
{
    return new VocalAIProEditor (*this);
}

//==============================================================================
void VocalAIProPlugin::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VocalAIProPlugin::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
void VocalAIProPlugin::updateParameters()
{
    if (!isInitialized) return;
    
    // Update smoothed parameters for anti-aliasing with safety limits
    float inputGainDb = juce::jlimit(-24.0f, 12.0f, inputGainParam->load());
    float outputGainDb = juce::jlimit(-24.0f, 12.0f, outputGainParam->load());
    inputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(inputGainDb));
    outputGainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputGainDb));
    reverbAmountSmoother.setTargetValue(reverbAmountParam->load() / 100.0f);
    delayTimeSmoother.setTargetValue(delayTimeParam->load());
    
    // Update input/output gain with smoothing
    inputGain.setGainLinear(inputGainSmoother.getNextValue());
    outputGain.setGainLinear(outputGainSmoother.getNextValue());
    
    // Update AI components
    if (aiPitchTuner)
    {
        aiPitchTuner->setPitchCorrection(pitchCorrectionParam->load() / 100.0f);
        aiPitchTuner->setPitchSpeed(pitchSpeedParam->load() / 100.0f);
    }
    
    if (vocalEffects)
    {
        vocalEffects->setReverbAmount(reverbAmountSmoother.getNextValue());
        vocalEffects->setDelayTime(delayTimeSmoother.getNextValue());
        vocalEffects->setDelayFeedback(delayFeedbackParam->load() / 100.0f);
        vocalEffects->setHarmonyAmount(harmonyAmountParam->load() / 100.0f);
        vocalEffects->setHarmonyVoices(static_cast<int>(harmonyVoicesParam->load()));
    }
}

void VocalAIProPlugin::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(parameterID, newValue);
    // Parameters are updated in updateParameters() for efficiency
}

bool VocalAIProPlugin::validateParameter(const juce::String& parameterID, float value)
{
    // Check for NaN or infinity
    if (std::isnan(value) || std::isinf(value)) {
        return false;
    }
    
    // Parameter-specific validation
    if (parameterID == "pitchCorrection" || parameterID == "pitchSpeed" || 
        parameterID == "reverbAmount" || parameterID == "harmonyAmount") {
        return value >= 0.0f && value <= 100.0f;
    }
    
    if (parameterID == "delayTime") {
        return value >= 0.0f && value <= 2000.0f;
    }
    
    if (parameterID == "delayFeedback") {
        return value >= 0.0f && value <= 80.0f; // Reduced max to prevent feedback
    }
    
    if (parameterID == "harmonyVoices") {
        return value >= 1.0f && value <= 8.0f;
    }
    
    if (parameterID == "inputGain" || parameterID == "outputGain") {
        return value >= -24.0f && value <= 12.0f; // Reduced max to prevent clipping
    }
    
    return true; // Default validation passed
}

float VocalAIProPlugin::sanitizeParameter(float value, float minVal, float maxVal)
{
    // Handle NaN and infinity
    if (std::isnan(value) || std::isinf(value)) {
        return (minVal + maxVal) * 0.5f; // Return middle value
    }
    
    // Clamp to valid range
    return juce::jlimit(minVal, maxVal, value);
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalAIProPlugin();
}
