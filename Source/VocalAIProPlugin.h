#pragma once

#include <JuceHeader.h>
#include <memory>

// Forward declarations
class AIPitchTuner;
class VocalEffects;

//==============================================================================
/**
    AI-Powered Vocal Effects Suite Plugin
    Features:
    - AI Pitch Tuning
    - Vocal Effects (Reverb, Delay, Harmony)
    - Real-time Processing
    - Preset System
*/
class VocalAIProPlugin : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
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
    
    //==============================================================================
    // Preset Management
    void initializePresets();
    void applyPreset(int presetIndex);
    void saveCurrentPreset(int presetIndex);
    void loadPreset(int presetIndex);
    void checkForCustomPreset();
    
    // Preset data structure
    struct PresetData
    {
        float pitchCorrection = 50.0f;
        float pitchSpeed = 50.0f;
        bool pitchCorrectionEnabled = true;
        float reverbAmount = 0.0f;
        float delayTime = 250.0f;
        float delayFeedback = 30.0f;
        float harmonyAmount = 0.0f;
        int harmonyVoices = 2;
        float inputGain = 0.0f;
        float outputGain = 0.0f;
        bool bypass = false;
    };
    
    std::vector<PresetData> presets;
    std::vector<juce::String> presetNames;
    int currentProgram = 0;
    bool isCustomPreset = false;
    
    // AI Pitch Tuning Parameters
    std::atomic<float>* pitchCorrectionParam;
    std::atomic<float>* pitchSpeedParam;
    std::atomic<float>* pitchCorrectionEnabledParam;
    
    // Vocal Effects Parameters
    std::atomic<float>* reverbAmountParam;
    std::atomic<float>* delayTimeParam;
    std::atomic<float>* delayFeedbackParam;
    std::atomic<float>* harmonyAmountParam;
    std::atomic<float>* harmonyVoicesParam;
    
    // Master Parameters
    std::atomic<float>* inputGainParam;
    std::atomic<float>* outputGainParam;
    std::atomic<float>* bypassParam;
    
    // Getter functions for editor access
    AIPitchTuner* getAIPitchTuner() const { return aiPitchTuner.get(); }
    VocalEffects* getVocalEffects() const { return vocalEffects.get(); }
    
    // Spectrum Analysis
    void updateSpectrum(const juce::AudioBuffer<float>& buffer);
    const std::vector<float>& getSpectrumMagnitudes() const { return spectrumMagnitudes; }

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
    
    // Spectrum Analysis
    std::unique_ptr<juce::dsp::FFT> fft;
    std::vector<float> fftData;
    std::vector<float> window;
    std::vector<float> spectrumMagnitudes;
    
    // Parameter Smoothing
    juce::LinearSmoothedValue<float> inputGainSmoother;
    juce::LinearSmoothedValue<float> outputGainSmoother;
    juce::LinearSmoothedValue<float> reverbAmountSmoother;
    juce::LinearSmoothedValue<float> delayTimeSmoother;
    
    // Processing State
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool isInitialized = false;
    
    //==============================================================================
    // AudioProcessorValueTreeState::Listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    //==============================================================================
    // Parameter Update Handling
    void updateParameters();
    
    // Parameter Validation
    bool validateParameter(const juce::String& parameterID, float value);
    float sanitizeParameter(float value, float minVal, float maxVal);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAIProPlugin)
};
