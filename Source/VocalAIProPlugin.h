#pragma once

#include <JuceHeader.h>
#include <memory>

//==============================================================================
/**
    AI-Powered Vocal Effects Suite Plugin
    Features:
    - AI Pitch Tuning
    - Vocal Effects (Reverb, Delay, Harmony)
    - Real-time Processing
    - Preset System
*/
class VocalAIProPlugin : public juce::AudioProcessor
{
public:
    //==============================================================================
    VocalAIProPlugin();
    ~VocalAIProPlugin() override;

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
    // Parameter Management
    juce::AudioProcessorValueTreeState& getValueTreeState() { return parameters; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // AI Pitch Tuning Parameters
    juce::AudioParameterFloat* pitchCorrectionParam;
    juce::AudioParameterFloat* pitchSpeedParam;
    juce::AudioParameterBool* pitchCorrectionEnabledParam;
    
    // Vocal Effects Parameters
    juce::AudioParameterFloat* reverbAmountParam;
    juce::AudioParameterFloat* delayTimeParam;
    juce::AudioParameterFloat* delayFeedbackParam;
    juce::AudioParameterFloat* harmonyAmountParam;
    juce::AudioParameterInt* harmonyVoicesParam;
    
    // Master Parameters
    juce::AudioParameterFloat* inputGainParam;
    juce::AudioParameterFloat* outputGainParam;
    juce::AudioParameterBool* bypassParam;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    
    // AI Processing Components
    std::unique_ptr<class AIPitchTuner> aiPitchTuner;
    std::unique_ptr<class VocalEffects> vocalEffects;
    
    // DSP Components
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    // Parameter Smoothing
    juce::LinearSmoothedValue<float> inputGainSmoother;
    juce::LinearSmoothedValue<float> outputGainSmoother;
    juce::LinearSmoothedValue<float> reverbAmountSmoother;
    juce::LinearSmoothedValue<float> delayTimeSmoother;
    
    // Processing State
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool isInitialized = false;
    
    // Getter functions for editor access
    AIPitchTuner* getAIPitchTuner() const { return aiPitchTuner.get(); }
    VocalEffects* getVocalEffects() const { return vocalEffects.get(); }

private:
    //==============================================================================
    // Parameter Update Handling
    void updateParameters();
    void parameterChanged(const juce::String& parameterID, float newValue);
    
    // Parameter Validation
    bool validateParameter(const juce::String& parameterID, float value);
    float sanitizeParameter(float value, float minVal, float maxVal);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAIProPlugin)
};
