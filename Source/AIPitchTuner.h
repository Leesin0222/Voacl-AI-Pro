#pragma once

#include <JuceHeader.h>
#include <vector>
#include <complex>
#include <memory>

//==============================================================================
/**
    AI-Powered Pitch Tuning System
    
    Features:
    - Real-time pitch detection using autocorrelation
    - Smooth pitch correction with configurable strength
    - Natural-sounding pitch shifting
    - Low latency processing
*/
class AIPitchTuner
{
public:
    //==============================================================================
    AIPitchTuner();
    ~AIPitchTuner();
    
    //==============================================================================
    void prepare(double sampleRate, int blockSize, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void reset();
    
    //==============================================================================
    // Parameter Controls
    void setPitchCorrection(float strength); // 0.0 to 1.0
    void setPitchSpeed(float speed);         // 0.0 to 1.0
    void setKeyCenter(float frequency);      // Hz
    void setScaleMode(int mode);             // 0=Chromatic, 1=Major, 2=Minor, etc.
    
    //==============================================================================
    // Analysis Results
    float getCurrentPitch() const { return currentPitch; }
    float getPitchConfidence() const { return pitchConfidence; }
    bool isPitchDetected() const { return pitchDetected; }

private:
    //==============================================================================
    // Pitch Detection
    float detectPitch(const float* samples, int numSamples);
    float autocorrelationPitchDetection(const float* samples, int numSamples);
    float yinPitchDetection(const float* samples, int numSamples);
    
    // Pitch Correction
    float calculateTargetPitch(float detectedPitch);
    void applyPitchCorrection(juce::AudioBuffer<float>& buffer, int channel, float targetPitch);
    
    // Pitch Shifting (Improved FFT-based)
    void pitchShift(float* samples, int numSamples, float pitchRatio);
    void processPitchShift(juce::AudioBuffer<float>& buffer, int channel, float pitchRatio);
    void processPitchShiftFFT(float* samples, int numSamples, float pitchRatio);
    
    //==============================================================================
    // Processing State
    double sampleRate = 44100.0;
    int blockSize = 512;
    int numChannels = 2;
    bool isPrepared = false;
    
    // Pitch Detection State
    std::vector<float> pitchBuffer;
    std::vector<float> autocorrelationBuffer;
    std::vector<float> yinBuffer;
    int pitchBufferSize = 2048;
    int hopSize = 256;
    int currentHopPosition = 0;
    
    // Current Analysis Results
    float currentPitch = 0.0f;
    float pitchConfidence = 0.0f;
    bool pitchDetected = false;
    
    // Pitch Correction Parameters
    float pitchCorrectionStrength = 0.5f;
    float pitchSpeed = 0.5f;
    float keyCenter = 440.0f; // A4
    int scaleMode = 0; // Chromatic
    
    // Pitch Shifting State
    std::vector<float> pitchShiftBuffer;
    std::vector<float> overlapBuffer;
    std::vector<float> fftBuffer;
    std::vector<float> windowBuffer;
    std::vector<float> tempBuffer; // Reusable temporary buffer
    int pitchShiftBufferSize = 4096;
    int fftSize = 2048;
    float currentPitchRatio = 1.0f;
    float targetPitchRatio = 1.0f;
    
    // Smoothing
    juce::LinearSmoothedValue<float> pitchRatioSmoother;
    juce::LinearSmoothedValue<float> pitchSmoother;
    
    //==============================================================================
    // Helper Functions
    float frequencyToMidi(float frequency);
    float midiToFrequency(float midiNote);
    float quantizeToScale(float midiNote, int scaleMode);
    bool isInRange(float frequency, float minFreq = 80.0f, float maxFreq = 2000.0f);
    
    // Window Functions
    void applyHannWindow(float* samples, int numSamples);
    void applyBlackmanWindow(float* samples, int numSamples);
    
    // Mathematical Functions
    float calculateRMS(const float* samples, int numSamples);
    float calculateAutocorrelation(const float* samples, int numSamples, int lag);
    float findPeakInRange(const float* data, int start, int end);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AIPitchTuner)
};
