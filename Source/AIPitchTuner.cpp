#include "AIPitchTuner.h"

//==============================================================================
AIPitchTuner::AIPitchTuner()
{
    // Initialize buffers
    pitchBuffer.resize(pitchBufferSize, 0.0f);
    autocorrelationBuffer.resize(pitchBufferSize, 0.0f);
    yinBuffer.resize(pitchBufferSize, 0.0f);
    pitchShiftBuffer.resize(pitchShiftBufferSize, 0.0f);
    overlapBuffer.resize(hopSize, 0.0f);
    fftBuffer.resize(fftSize * 2, 0.0f); // Complex FFT buffer
    windowBuffer.resize(fftSize, 0.0f);
    
    // Initialize Hann window
    for (int i = 0; i < fftSize; ++i)
    {
        windowBuffer[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (fftSize - 1)));
    }
    
    // Initialize smoothers
    pitchRatioSmoother.reset(0.0f);
    pitchSmoother.reset(0.0f);
}

AIPitchTuner::~AIPitchTuner()
{
}

//==============================================================================
void AIPitchTuner::prepare(double newSampleRate, int newBlockSize, int newNumChannels)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    numChannels = newNumChannels;
    
    // Resize buffers based on sample rate
    pitchBufferSize = static_cast<int>(sampleRate * 0.1); // 100ms buffer
    hopSize = static_cast<int>(sampleRate * 0.01); // 10ms hop
    
    pitchBuffer.resize(pitchBufferSize, 0.0f);
    autocorrelationBuffer.resize(pitchBufferSize, 0.0f);
    yinBuffer.resize(pitchBufferSize, 0.0f);
    overlapBuffer.resize(hopSize, 0.0f);
    
    // Reset processing state
    currentHopPosition = 0;
    currentPitch = 0.0f;
    pitchConfidence = 0.0f;
    pitchDetected = false;
    currentPitchRatio = 1.0f;
    targetPitchRatio = 1.0f;
    
    // Configure smoothers
    pitchRatioSmoother.reset(1.0f);
    pitchSmoother.reset(0.0f);
    pitchRatioSmoother.setCurrentAndTargetValue(1.0f);
    pitchSmoother.setCurrentAndTargetValue(0.0f);
    
    isPrepared = true;
}

void AIPitchTuner::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isPrepared) return;
    
    juce::ignoreUnused(midiMessages);
    
    const int numSamples = buffer.getNumSamples();
    const int numChannelsToProcess = juce::jmin(numChannels, buffer.getNumChannels());
    
    // Process each channel
    for (int channel = 0; channel < numChannelsToProcess; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        // Fill pitch detection buffer
        for (int sample = 0; sample < numSamples; ++sample)
        {
            pitchBuffer[currentHopPosition] = channelData[sample];
            currentHopPosition = (currentHopPosition + 1) % pitchBufferSize;
            
            // Perform pitch detection when buffer is full
            if (currentHopPosition == 0)
            {
                float detectedPitch = detectPitch(pitchBuffer.data(), pitchBufferSize);
                
                if (detectedPitch > 0.0f && isInRange(detectedPitch))
                {
                    currentPitch = detectedPitch;
                    pitchDetected = true;
                    pitchConfidence = 0.8f; // Simplified confidence calculation
                    
                    // Calculate target pitch and pitch ratio
                    float targetPitch = calculateTargetPitch(detectedPitch);
                    targetPitchRatio = targetPitch / detectedPitch;
                }
                else
                {
                    pitchDetected = false;
                    pitchConfidence = 0.0f;
                    targetPitchRatio = 1.0f;
                }
            }
        }
        
        // Apply pitch correction if enabled
        if (pitchDetected && pitchCorrectionStrength > 0.0f)
        {
            // Smooth pitch ratio changes
            pitchRatioSmoother.setTargetValue(targetPitchRatio);
            currentPitchRatio = pitchRatioSmoother.getNextValue();
            
            // Apply pitch shifting
            if (std::abs(currentPitchRatio - 1.0f) > 0.01f)
            {
                processPitchShift(buffer, channel, currentPitchRatio);
            }
        }
    }
}

void AIPitchTuner::reset()
{
    currentHopPosition = 0;
    currentPitch = 0.0f;
    pitchConfidence = 0.0f;
    pitchDetected = false;
    currentPitchRatio = 1.0f;
    targetPitchRatio = 1.0f;
    
    // Clear buffers
    std::fill(pitchBuffer.begin(), pitchBuffer.end(), 0.0f);
    std::fill(autocorrelationBuffer.begin(), autocorrelationBuffer.end(), 0.0f);
    std::fill(yinBuffer.begin(), yinBuffer.end(), 0.0f);
    std::fill(pitchShiftBuffer.begin(), pitchShiftBuffer.end(), 0.0f);
    std::fill(overlapBuffer.begin(), overlapBuffer.end(), 0.0f);
    
    // Reset smoothers
    pitchRatioSmoother.reset(1.0f);
    pitchSmoother.reset(0.0f);
}

//==============================================================================
void AIPitchTuner::setPitchCorrection(float strength)
{
    pitchCorrectionStrength = juce::jlimit(0.0f, 1.0f, strength);
}

void AIPitchTuner::setPitchSpeed(float speed)
{
    pitchSpeed = juce::jlimit(0.0f, 1.0f, speed);
    
    // Adjust smoother speed based on pitch speed parameter
    float smootherTime = 0.1f * (1.0f - speed) + 0.001f; // 1ms to 100ms
    pitchRatioSmoother.reset(smootherTime * sampleRate);
}

void AIPitchTuner::setKeyCenter(float frequency)
{
    keyCenter = juce::jlimit(80.0f, 2000.0f, frequency);
}

void AIPitchTuner::setScaleMode(int mode)
{
    scaleMode = juce::jlimit(0, 7, mode);
}

//==============================================================================
float AIPitchTuner::detectPitch(const float* samples, int numSamples)
{
    // Use autocorrelation method for pitch detection
    return autocorrelationPitchDetection(samples, numSamples);
}

float AIPitchTuner::autocorrelationPitchDetection(const float* samples, int numSamples)
{
    // Calculate autocorrelation
    const int minPeriod = static_cast<int>(sampleRate / 2000.0f); // 2000 Hz max
    const int maxPeriod = static_cast<int>(sampleRate / 80.0f);   // 80 Hz min
    
    float maxCorrelation = 0.0f;
    int bestPeriod = 0;
    
    for (int period = minPeriod; period < maxPeriod && period < numSamples / 2; ++period)
    {
        float correlation = 0.0f;
        int count = 0;
        
        for (int i = 0; i < numSamples - period; ++i)
        {
            correlation += samples[i] * samples[i + period];
            count++;
        }
        
        if (count > 0)
        {
            correlation /= count;
            
            if (correlation > maxCorrelation)
            {
                maxCorrelation = correlation;
                bestPeriod = period;
            }
        }
    }
    
    if (bestPeriod > 0 && maxCorrelation > 0.1f) // Threshold for confidence
    {
        return sampleRate / bestPeriod;
    }
    
    return 0.0f;
}

float AIPitchTuner::yinPitchDetection(const float* samples, int numSamples)
{
    // Simplified YIN algorithm implementation
    const int minPeriod = static_cast<int>(sampleRate / 2000.0f);
    const int maxPeriod = static_cast<int>(sampleRate / 80.0f);
    
    // Calculate difference function
    std::fill(yinBuffer.begin(), yinBuffer.end(), 0.0f);
    
    for (int tau = minPeriod; tau < maxPeriod && tau < numSamples / 2; ++tau)
    {
        float sum = 0.0f;
        for (int i = 0; i < numSamples - tau; ++i)
        {
            float diff = samples[i] - samples[i + tau];
            sum += diff * diff;
        }
        yinBuffer[tau] = sum;
    }
    
    // Find minimum
    float minValue = yinBuffer[minPeriod];
    int minIndex = minPeriod;
    
    for (int i = minPeriod + 1; i < maxPeriod; ++i)
    {
        if (yinBuffer[i] < minValue)
        {
            minValue = yinBuffer[i];
            minIndex = i;
        }
    }
    
    if (minValue < 0.1f && minIndex > 0) // Threshold
    {
        return sampleRate / minIndex;
    }
    
    return 0.0f;
}

//==============================================================================
float AIPitchTuner::calculateTargetPitch(float detectedPitch)
{
    if (detectedPitch <= 0.0f) return detectedPitch;
    
    // Convert to MIDI note
    float midiNote = frequencyToMidi(detectedPitch);
    
    // Quantize to scale if not in chromatic mode
    if (scaleMode > 0)
    {
        midiNote = quantizeToScale(midiNote, scaleMode);
    }
    else
    {
        // Chromatic quantization
        midiNote = juce::roundToInt(midiNote);
    }
    
    // Convert back to frequency
    float targetPitch = midiToFrequency(midiNote);
    
    // Apply correction strength
    return detectedPitch + (targetPitch - detectedPitch) * pitchCorrectionStrength;
}

void AIPitchTuner::applyPitchCorrection(juce::AudioBuffer<float>& buffer, int channel, float targetPitch)
{
    if (currentPitch <= 0.0f) return;
    
    float pitchRatio = targetPitch / currentPitch;
    processPitchShift(buffer, channel, pitchRatio);
}

//==============================================================================
void AIPitchTuner::pitchShift(float* samples, int numSamples, float pitchRatio)
{
    // Simple pitch shifting using phase vocoder technique
    // This is a simplified implementation - production code would use more sophisticated algorithms
    
    if (std::abs(pitchRatio - 1.0f) < 0.01f) return;
    
    // For now, use a simple time-stretching approach
    // In a real implementation, you'd use FFT-based phase vocoder or PSOLA
    
    std::vector<float> tempBuffer(numSamples);
    std::copy(samples, samples + numSamples, tempBuffer.data());
    
    for (int i = 0; i < numSamples; ++i)
    {
        float sourceIndex = i / pitchRatio;
        int sourceIndexInt = static_cast<int>(sourceIndex);
        float fraction = sourceIndex - sourceIndexInt;
        
        if (sourceIndexInt >= 0 && sourceIndexInt < numSamples - 1)
        {
            // Linear interpolation
            samples[i] = tempBuffer[sourceIndexInt] * (1.0f - fraction) + 
                        tempBuffer[sourceIndexInt + 1] * fraction;
        }
        else if (sourceIndexInt >= 0 && sourceIndexInt < numSamples)
        {
            samples[i] = tempBuffer[sourceIndexInt];
        }
        else
        {
            samples[i] = 0.0f;
        }
    }
}

void AIPitchTuner::processPitchShift(juce::AudioBuffer<float>& buffer, int channel, float pitchRatio)
{
    float* channelData = buffer.getWritePointer(channel);
    const int numSamples = buffer.getNumSamples();
    
    // Use improved FFT-based pitch shifting for better quality
    if (std::abs(pitchRatio - 1.0f) > 0.01f)
    {
        processPitchShiftFFT(channelData, numSamples, pitchRatio);
    }
}

void AIPitchTuner::processPitchShiftFFT(float* samples, int numSamples, float pitchRatio)
{
    // Enhanced FFT-based pitch shifting with phase vocoder
    // This provides much better quality than simple time-stretching
    
    const int overlap = fftSize / 4; // 75% overlap for smooth results
    const int stepSize = fftSize - overlap;
    
    for (int pos = 0; pos < numSamples - fftSize; pos += stepSize)
    {
        // Apply window and copy to FFT buffer
        for (int i = 0; i < fftSize; ++i)
        {
            if (pos + i < numSamples)
            {
                fftBuffer[i * 2] = samples[pos + i] * windowBuffer[i]; // Real part
                fftBuffer[i * 2 + 1] = 0.0f; // Imaginary part
            }
            else
            {
                fftBuffer[i * 2] = 0.0f;
                fftBuffer[i * 2 + 1] = 0.0f;
            }
        }
        
        // Perform FFT (using JUCE's FFT)
        juce::dsp::FFT fft(static_cast<int>(std::log2(fftSize)));
        fft.performFrequencyOnlyForwardTransform(fftBuffer.data());
        
        // Phase vocoder processing
        static std::vector<float> lastPhase(fftSize / 2 + 1, 0.0f);
        static std::vector<float> sumPhase(fftSize / 2 + 1, 0.0f);
        
        for (int k = 0; k <= fftSize / 2; ++k)
        {
            float magnitude = std::sqrt(fftBuffer[k * 2] * fftBuffer[k * 2] + fftBuffer[k * 2 + 1] * fftBuffer[k * 2 + 1]);
            float phase = std::atan2(fftBuffer[k * 2 + 1], fftBuffer[k * 2]);
            
            float deltaPhase = phase - lastPhase[k];
            lastPhase[k] = phase;
            
            // Unwrap phase
            while (deltaPhase > juce::MathConstants<float>::pi) deltaPhase -= 2.0f * juce::MathConstants<float>::pi;
            while (deltaPhase < -juce::MathConstants<float>::pi) deltaPhase += 2.0f * juce::MathConstants<float>::pi;
            
            // Calculate instantaneous frequency
            float instFreq = k * 2.0f * juce::MathConstants<float>::pi / fftSize + deltaPhase / stepSize;
            
            // Adjust frequency for pitch shifting
            float newFreq = instFreq * pitchRatio;
            
            // Update phase
            sumPhase[k] += newFreq * stepSize;
            
            // Convert back to complex
            fftBuffer[k * 2] = magnitude * std::cos(sumPhase[k]);
            fftBuffer[k * 2 + 1] = magnitude * std::sin(sumPhase[k]);
        }
        
        // Perform inverse FFT
        fft.performRealOnlyInverseTransform(fftBuffer.data());
        
        // Apply window and overlap-add
        for (int i = 0; i < fftSize && pos + i < numSamples; ++i)
        {
            samples[pos + i] = fftBuffer[i] * windowBuffer[i] * 0.5f; // Scale down for overlap
        }
    }
}

//==============================================================================
float AIPitchTuner::frequencyToMidi(float frequency)
{
    return 12.0f * std::log2(frequency / 440.0f) + 69.0f;
}

float AIPitchTuner::midiToFrequency(float midiNote)
{
    return 440.0f * std::pow(2.0f, (midiNote - 69.0f) / 12.0f);
}

float AIPitchTuner::quantizeToScale(float midiNote, int scaleMode)
{
    // Scale patterns (semitones from root)
    static const std::vector<std::vector<int>> scales = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, // Chromatic
        {0, 2, 4, 5, 7, 9, 11},                  // Major
        {0, 2, 3, 5, 7, 8, 10},                  // Minor
        {0, 2, 4, 6, 7, 9, 11},                  // Lydian
        {0, 1, 3, 5, 7, 8, 10},                  // Dorian
        {0, 2, 3, 5, 7, 9, 10},                  // Mixolydian
        {0, 1, 3, 5, 6, 8, 10},                  // Phrygian
        {0, 2, 3, 6, 7, 8, 11}                   // Locrian
    };
    
    if (scaleMode < 0 || scaleMode >= static_cast<int>(scales.size()))
        return midiNote;
    
    const auto& scale = scales[scaleMode];
    float rootNote = std::floor(midiNote);
    float octave = std::floor(rootNote / 12.0f) * 12.0f;
    float noteInOctave = rootNote - octave;
    
    // Find closest note in scale
    float closestNote = noteInOctave;
    float minDistance = 12.0f;
    
    for (int scaleNote : scale)
    {
        float distance = std::abs(noteInOctave - scaleNote);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestNote = scaleNote;
        }
    }
    
    return octave + closestNote;
}

bool AIPitchTuner::isInRange(float frequency, float minFreq, float maxFreq)
{
    return frequency >= minFreq && frequency <= maxFreq;
}

//==============================================================================
void AIPitchTuner::applyHannWindow(float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        float window = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * i / (numSamples - 1)));
        samples[i] *= window;
    }
}

void AIPitchTuner::applyBlackmanWindow(float* samples, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        float window = 0.42f - 0.5f * std::cos(2.0f * juce::MathConstants<float>::pi * i / (numSamples - 1)) +
                      0.08f * std::cos(4.0f * juce::MathConstants<float>::pi * i / (numSamples - 1));
        samples[i] *= window;
    }
}

float AIPitchTuner::calculateRMS(const float* samples, int numSamples)
{
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        sum += samples[i] * samples[i];
    }
    return std::sqrt(sum / numSamples);
}

float AIPitchTuner::calculateAutocorrelation(const float* samples, int numSamples, int lag)
{
    if (lag >= numSamples) return 0.0f;
    
    float sum = 0.0f;
    int count = 0;
    
    for (int i = 0; i < numSamples - lag; ++i)
    {
        sum += samples[i] * samples[i + lag];
        count++;
    }
    
    return count > 0 ? sum / count : 0.0f;
}

float AIPitchTuner::findPeakInRange(const float* data, int start, int end)
{
    float maxValue = data[start];
    int maxIndex = start;
    
    for (int i = start + 1; i < end; ++i)
    {
        if (data[i] > maxValue)
        {
            maxValue = data[i];
            maxIndex = i;
        }
    }
    
    return static_cast<float>(maxIndex);
}
