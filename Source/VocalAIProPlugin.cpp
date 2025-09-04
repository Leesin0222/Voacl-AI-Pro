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
    
    // Initialize presets after parameters are ready
    initializePresets();
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
    return 13; // 8 GOD presets + 5 basic presets
}

int VocalAIProPlugin::getCurrentProgram()
{
    return currentProgram;
}

void VocalAIProPlugin::setCurrentProgram (int index)
{
    // Enhanced validation with bounds checking
    if (index >= 0 && index < getNumPrograms() && index < static_cast<int>(presets.size()))
    {
        currentProgram = index;
        applyPreset(index);
    }
    else
    {
        // Fallback to default preset if invalid index
        currentProgram = 0;
        applyPreset(0);
    }
}

const juce::String VocalAIProPlugin::getProgramName (int index)
{
    if (index >= 0 && index < getNumPrograms() && index < static_cast<int>(presetNames.size()))
    {
        juce::String name = presetNames[index];
        // Add custom indicator if current program is custom
        if (index == currentProgram && isCustomPreset)
        {
            name += " (Custom)";
        }
        return name;
    }
    return "Invalid Preset";
}

void VocalAIProPlugin::changeProgramName (int index, const juce::String& newName)
{
    if (index >= 0 && index < getNumPrograms() && index < static_cast<int>(presetNames.size()))
    {
        presetNames[index] = newName;
    }
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
    
    // Add preset information to state
    state.setProperty("currentProgram", currentProgram, nullptr);
    
    // Save preset names
    juce::ValueTree presetNamesTree("PresetNames");
    for (int i = 0; i < static_cast<int>(presetNames.size()); ++i)
    {
        presetNamesTree.setProperty("preset" + juce::String(i), presetNames[i], nullptr);
    }
    state.addChild(presetNamesTree, -1, nullptr);
    
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VocalAIProPlugin::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            auto state = juce::ValueTree::fromXml (*xmlState);
            
            // Restore preset information
            if (state.hasProperty("currentProgram"))
            {
                currentProgram = state.getProperty("currentProgram", 0);
            }
            
            // Restore preset names
            auto presetNamesTree = state.getChildWithName("PresetNames");
            if (presetNamesTree.isValid())
            {
                for (int i = 0; i < static_cast<int>(presetNames.size()); ++i)
                {
                    juce::String presetName = presetNamesTree.getProperty("preset" + juce::String(i), presetNames[i]);
                    if (presetName.isNotEmpty())
                    {
                        presetNames[i] = presetName;
                    }
                }
            }
            
            parameters.replaceState (state);
        }
    }
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
    
    // Check if current settings match any preset
    checkForCustomPreset();
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
// Preset Management Implementation
void VocalAIProPlugin::initializePresets()
{
    presets.resize(13);
    presetNames.resize(13);
    
    // GOD Presets - Professional Vocal Presets
    presetNames[0] = "GOD - Perfect Vocal";
    presets[0] = {85.0f, 70.0f, true, 25.0f, 200.0f, 20.0f, 15.0f, 2, 2.0f, 0.0f, false};
    
    presetNames[1] = "GOD - Studio Master";
    presets[1] = {95.0f, 80.0f, true, 35.0f, 300.0f, 25.0f, 20.0f, 3, 1.5f, -1.0f, false};
    
    presetNames[2] = "GOD - Pop Hit Maker";
    presets[2] = {90.0f, 75.0f, true, 40.0f, 250.0f, 30.0f, 45.0f, 4, 3.0f, 0.5f, false};
    
    presetNames[3] = "GOD - Rock Legend";
    presets[3] = {60.0f, 50.0f, true, 60.0f, 500.0f, 40.0f, 30.0f, 3, 4.0f, 1.0f, false};
    
    presetNames[4] = "GOD - R&B Smooth";
    presets[4] = {80.0f, 60.0f, true, 50.0f, 400.0f, 35.0f, 55.0f, 5, 2.5f, 0.0f, false};
    
    presetNames[5] = "GOD - Trap Vocal";
    presets[5] = {70.0f, 65.0f, true, 70.0f, 150.0f, 40.0f, 25.0f, 2, 3.0f, 1.0f, false};
    
    presetNames[6] = "GOD - Live Performance";
    presets[6] = {75.0f, 85.0f, true, 30.0f, 200.0f, 15.0f, 10.0f, 2, 1.0f, 0.0f, false};
    
    presetNames[7] = "GOD - Creative Chaos";
    presets[7] = {50.0f, 40.0f, true, 60.0f, 600.0f, 40.0f, 50.0f, 4, 2.0f, 0.5f, false};
    
    // Basic Presets
    presetNames[8] = "Default";
    presets[8] = {50.0f, 50.0f, true, 0.0f, 250.0f, 30.0f, 0.0f, 2, 0.0f, 0.0f, false};
    
    presetNames[9] = "Vocal Doubling";
    presets[9] = {30.0f, 40.0f, true, 15.0f, 20.0f, 10.0f, 0.0f, 2, 1.0f, 0.0f, false};
    
    presetNames[10] = "Harmony";
    presets[10] = {40.0f, 60.0f, true, 20.0f, 100.0f, 15.0f, 80.0f, 4, 0.5f, 0.0f, false};
    
    presetNames[11] = "Reverb";
    presets[11] = {20.0f, 30.0f, true, 80.0f, 50.0f, 5.0f, 0.0f, 2, 0.0f, 0.0f, false};
    
    presetNames[12] = "Delay";
    presets[12] = {25.0f, 35.0f, true, 10.0f, 500.0f, 60.0f, 0.0f, 2, 0.0f, 0.0f, false};
}

void VocalAIProPlugin::applyPreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;
    
    const PresetData& preset = presets[presetIndex];
    
    // Apply preset values to parameters
    *pitchCorrectionParam = preset.pitchCorrection;
    *pitchSpeedParam = preset.pitchSpeed;
    *pitchCorrectionEnabledParam = preset.pitchCorrectionEnabled ? 1.0f : 0.0f;
    *reverbAmountParam = preset.reverbAmount;
    *delayTimeParam = preset.delayTime;
    *delayFeedbackParam = preset.delayFeedback;
    *harmonyAmountParam = preset.harmonyAmount;
    *harmonyVoicesParam = static_cast<float>(preset.harmonyVoices);
    *inputGainParam = preset.inputGain;
    *outputGainParam = preset.outputGain;
    *bypassParam = preset.bypass ? 1.0f : 0.0f;
    
    // Mark as not custom preset
    isCustomPreset = false;
    
    // Update parameters
    updateParameters();
}

void VocalAIProPlugin::saveCurrentPreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;
    
    PresetData& preset = presets[presetIndex];
    
    // Save current parameter values to preset
    preset.pitchCorrection = *pitchCorrectionParam;
    preset.pitchSpeed = *pitchSpeedParam;
    preset.pitchCorrectionEnabled = *pitchCorrectionEnabledParam > 0.5f;
    preset.reverbAmount = *reverbAmountParam;
    preset.delayTime = *delayTimeParam;
    preset.delayFeedback = *delayFeedbackParam;
    preset.harmonyAmount = *harmonyAmountParam;
    preset.harmonyVoices = static_cast<int>(*harmonyVoicesParam);
    preset.inputGain = *inputGainParam;
    preset.outputGain = *outputGainParam;
    preset.bypass = *bypassParam > 0.5f;
}

void VocalAIProPlugin::loadPreset(int presetIndex)
{
    applyPreset(presetIndex);
}

void VocalAIProPlugin::checkForCustomPreset()
{
    if (currentProgram < 0 || currentProgram >= static_cast<int>(presets.size()))
        return;
    
    const PresetData& currentPreset = presets[currentProgram];
    
    // Check if current parameter values match the preset with tolerance
    const float tolerance = 0.05f; // 5% tolerance for float comparisons
    const float timeTolerance = 0.5f; // 0.5ms tolerance for delay time
    
    bool matchesPreset = (
        std::abs(*pitchCorrectionParam - currentPreset.pitchCorrection) < tolerance &&
        std::abs(*pitchSpeedParam - currentPreset.pitchSpeed) < tolerance &&
        (*pitchCorrectionEnabledParam > 0.5f) == currentPreset.pitchCorrectionEnabled &&
        std::abs(*reverbAmountParam - currentPreset.reverbAmount) < tolerance &&
        std::abs(*delayTimeParam - currentPreset.delayTime) < timeTolerance &&
        std::abs(*delayFeedbackParam - currentPreset.delayFeedback) < tolerance &&
        std::abs(*harmonyAmountParam - currentPreset.harmonyAmount) < tolerance &&
        static_cast<int>(*harmonyVoicesParam) == currentPreset.harmonyVoices &&
        std::abs(*inputGainParam - currentPreset.inputGain) < tolerance &&
        std::abs(*outputGainParam - currentPreset.outputGain) < tolerance &&
        (*bypassParam > 0.5f) == currentPreset.bypass
    );
    
    isCustomPreset = !matchesPreset;
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalAIProPlugin();
}
