#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    Vocal Effects Processing Module
    
    Features:
    - Reverb with multiple algorithms
    - Delay with feedback and filtering
    - Harmony generation
    - Vocal doubling
    - Formant shifting
*/
class VocalEffects
{
public:
    //==============================================================================
    VocalEffects();
    ~VocalEffects();
    
    //==============================================================================
    void prepare(double sampleRate, int blockSize, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void reset();
    
    //==============================================================================
    // Reverb Controls
    void setReverbAmount(float amount);        // 0.0 to 1.0
    void setReverbSize(float size);            // 0.0 to 1.0
    void setReverbDamping(float damping);      // 0.0 to 1.0
    void setReverbWidth(float width);          // 0.0 to 1.0
    
    //==============================================================================
    // Delay Controls
    void setDelayTime(float timeMs);           // 0 to 2000 ms
    void setDelayFeedback(float feedback);     // 0.0 to 0.95
    void setDelayMix(float mix);               // 0.0 to 1.0
    void setDelayFilter(float cutoff);         // 20 to 20000 Hz
    
    //==============================================================================
    // Harmony Controls
    void setHarmonyAmount(float amount);       // 0.0 to 1.0
    void setHarmonyVoices(int voices);         // 1 to 8
    void setHarmonyInterval(int semitones);    // -12 to +12
    void setHarmonyDetune(float detune);       // -50 to +50 cents
    
    //==============================================================================
    // Vocal Doubling Controls
    void setDoublingAmount(float amount);      // 0.0 to 1.0
    void setDoublingDelay(float delayMs);      // 0 to 100 ms
    void setDoublingDetune(float detune);      // -50 to +50 cents
    
    //==============================================================================
    // Formant Controls
    void setFormantShift(float shift);         // -1.0 to +1.0
    void setFormantAmount(float amount);       // 0.0 to 1.0
    
    // Advanced Audio Processing
    void applyNaturalPitchVariation(float* samples, int numSamples, float amount);
    void applyDynamicEQ(float* samples, int numSamples, double sampleRate);
    void applyVocalEnhancement(float* samples, int numSamples, double sampleRate);
    
    //==============================================================================
    // Effect Parameters
    float reverbAmount = 0.0f;
    float reverbSize = 0.5f;
    float reverbDamping = 0.5f;
    float reverbWidth = 1.0f;
    
    float delayTime = 250.0f;
    float delayFeedback = 0.3f;
    float delayMix = 0.5f;
    float delayFilter = 8000.0f;
    
    float harmonyAmount = 0.0f;
    int harmonyVoices = 2;
    int harmonyInterval = 3; // Minor third
    float harmonyDetune = 0.0f;
    
    float doublingAmount = 0.0f;
    float doublingDelay = 20.0f;
    float doublingDetune = 5.0f;
    
    float formantShift = 0.0f;
    float formantAmount = 0.0f;

private:
    //==============================================================================
    // Processing State
    double sampleRate = 44100.0;
    int blockSize = 512;
    int numChannels = 2;
    bool isPrepared = false;
    
    //==============================================================================
    // Reverb Processing
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    juce::dsp::DryWetMixer<float> reverbMixer;
    
    //==============================================================================
    // Delay Processing
    struct DelayLine
    {
        std::vector<float> buffer;
        int writePosition = 0;
        int bufferSize = 0;
        
        void prepare(int size)
        {
            bufferSize = size;
            buffer.resize(bufferSize, 0.0f);
            writePosition = 0;
        }
        
        void process(float* samples, int numSamples, float delayTime, float feedback, float mix, float filterCutoff, double sampleRate)
        {
            const float delaySamples = delayTime * 0.001f * static_cast<float>(sampleRate); // Convert ms to samples
            int readPosition = (writePosition - static_cast<int>(delaySamples) + bufferSize) % bufferSize;
            
            for (int i = 0; i < numSamples; ++i)
            {
                float delayedSample = buffer[readPosition];
                
                // Apply simple low-pass filter to delay feedback
                static float filterState = 0.0f;
                const float filterCoeff = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * filterCutoff / static_cast<float>(sampleRate));
                filterState += filterCoeff * (delayedSample - filterState);
                delayedSample = filterState;
                
                // Mix with input
                float output = samples[i] + delayedSample * feedback;
                
                // Write to delay buffer
                buffer[writePosition] = output;
                
                // Update positions
                writePosition = (writePosition + 1) % bufferSize;
                readPosition = (readPosition + 1) % bufferSize;
                
                // Apply mix
                samples[i] = samples[i] * (1.0f - mix) + output * mix;
            }
        }
    };
    
    DelayLine delayLine;
    
    //==============================================================================
    // Harmony Processing
    struct HarmonyGenerator
    {
        std::vector<float> pitchShiftBuffers[8]; // Up to 8 harmony voices
        std::vector<float> delayBuffers[8];
        int bufferSizes[8];
        int writePositions[8];
        
        void prepare(int numVoices, int bufferSize)
        {
            for (int i = 0; i < numVoices; ++i)
            {
                pitchShiftBuffers[i].resize(bufferSize, 0.0f);
                delayBuffers[i].resize(bufferSize, 0.0f);
                bufferSizes[i] = bufferSize;
                writePositions[i] = 0;
            }
        }
        
        void process(float* samples, int numSamples, int numVoices, float amount, int interval, float detune)
        {
            for (int voice = 0; voice < numVoices; ++voice)
            {
                float voicePitchRatio = 1.0f;
                
                // Calculate pitch ratio for this voice
                if (voice == 0)
                {
                    voicePitchRatio = 1.0f; // Root note
                }
                else
                {
                    float semitones = interval * voice;
                    float detuneCents = detune * voice;
                    voicePitchRatio = std::pow(2.0f, (semitones + detuneCents / 100.0f) / 12.0f);
                }
                
                // Simple pitch shifting (in production, use more sophisticated algorithms)
                for (int i = 0; i < numSamples; ++i)
                {
                    float sourceIndex = i / voicePitchRatio;
                    int sourceIndexInt = static_cast<int>(sourceIndex);
                    float fraction = sourceIndex - sourceIndexInt;
                    
                    float harmonySample = 0.0f;
                    if (sourceIndexInt >= 0 && sourceIndexInt < numSamples - 1)
                    {
                        harmonySample = samples[sourceIndexInt] * (1.0f - fraction) + 
                                      samples[sourceIndexInt + 1] * fraction;
                    }
                    else if (sourceIndexInt >= 0 && sourceIndexInt < numSamples)
                    {
                        harmonySample = samples[sourceIndexInt];
                    }
                    
                    // Add to output with voice-specific gain
                    float voiceGain = amount / numVoices;
                    samples[i] += harmonySample * voiceGain;
                }
            }
        }
    };
    
    HarmonyGenerator harmonyGenerator;
    
    //==============================================================================
    // Vocal Doubling Processing
    struct VocalDoubler
    {
        std::vector<float> delayBuffer;
        int writePosition = 0;
        int bufferSize = 0;
        
        void prepare(int size)
        {
            bufferSize = size;
            delayBuffer.resize(bufferSize, 0.0f);
            writePosition = 0;
        }
        
        void process(float* samples, int numSamples, float amount, float delayMs, float detune, double sampleRate)
        {
            const float delaySamples = delayMs * 0.001f * static_cast<float>(sampleRate);
            int readPosition = (writePosition - static_cast<int>(delaySamples) + bufferSize) % bufferSize;
            
            for (int i = 0; i < numSamples; ++i)
            {
                float delayedSample = delayBuffer[readPosition];
                
                // Apply slight pitch variation for doubling effect
                if (detune != 0.0f)
                {
                    float pitchRatio = 1.0f + (detune / 100.0f) * 0.01f; // Small pitch variation
                    // Simple pitch shifting for doubling
                    float sourceIndex = i / pitchRatio;
                    int sourceIndexInt = static_cast<int>(sourceIndex);
                    float fraction = sourceIndex - sourceIndexInt;
                    
                    if (sourceIndexInt >= 0 && sourceIndexInt < numSamples - 1)
                    {
                        delayedSample = delayBuffer[readPosition] * (1.0f - fraction) + 
                                      delayBuffer[(readPosition + 1) % bufferSize] * fraction;
                    }
                }
                
                // Mix with input
                samples[i] = samples[i] + delayedSample * amount;
                
                // Write to delay buffer
                delayBuffer[writePosition] = samples[i];
                
                // Update positions
                writePosition = (writePosition + 1) % bufferSize;
                readPosition = (readPosition + 1) % bufferSize;
            }
        }
    };
    
    VocalDoubler vocalDoubler;
    
    //==============================================================================
    // Formant Processing
    struct FormantShifter
    {
        std::vector<float> formantBuffer;
        int bufferSize = 0;
        
        void prepare(int size)
        {
            bufferSize = size;
            formantBuffer.resize(bufferSize, 0.0f);
        }
        
        void process(float* samples, int numSamples, float shift, float amount)
        {
            if (std::abs(shift) < 0.01f || amount < 0.01f) return;
            
            // Simple formant shifting using pitch shifting with different ratios
            // In production, use more sophisticated formant preservation techniques
            
            for (int i = 0; i < numSamples; ++i)
            {
                float formantRatio = 1.0f + shift * 0.5f; // Scale the shift
                float sourceIndex = i / formantRatio;
                int sourceIndexInt = static_cast<int>(sourceIndex);
                float fraction = sourceIndex - sourceIndexInt;
                
                float formantSample = 0.0f;
                if (sourceIndexInt >= 0 && sourceIndexInt < numSamples - 1)
                {
                    formantSample = samples[sourceIndexInt] * (1.0f - fraction) + 
                                  samples[sourceIndexInt + 1] * fraction;
                }
                else if (sourceIndexInt >= 0 && sourceIndexInt < numSamples)
                {
                    formantSample = samples[sourceIndexInt];
                }
                
                // Mix with original
                samples[i] = samples[i] * (1.0f - amount) + formantSample * amount;
            }
        }
    };
    
    FormantShifter formantShifter;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEffects)
};
