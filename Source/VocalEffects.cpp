#include "VocalEffects.h"

//==============================================================================
VocalEffects::VocalEffects()
{
    // Initialize reverb parameters
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.3f;
    reverbParams.dryLevel = 0.4f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
}

VocalEffects::~VocalEffects()
{
}

//==============================================================================
void VocalEffects::prepare(double newSampleRate, int newBlockSize, int newNumChannels)
{
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    numChannels = newNumChannels;
    
    // Prepare reverb
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = static_cast<juce::uint32>(numChannels);
    
    reverb.prepare(spec);
    reverbMixer.prepare(spec);
    reverbMixer.setWetMixProportion(0.0f);
    
    // Prepare delay line
    int delayBufferSize = static_cast<int>(sampleRate * 2.0); // 2 seconds max delay
    delayLine.prepare(delayBufferSize);
    
    // Prepare harmony generator
    int harmonyBufferSize = static_cast<int>(sampleRate * 0.1); // 100ms buffer
    harmonyGenerator.prepare(8, harmonyBufferSize);
    
    // Prepare vocal doubler
    int doublerBufferSize = static_cast<int>(sampleRate * 0.1); // 100ms buffer
    vocalDoubler.prepare(doublerBufferSize);
    
    // Prepare formant shifter
    int formantBufferSize = static_cast<int>(sampleRate * 0.1); // 100ms buffer
    formantShifter.prepare(formantBufferSize);
    
    isPrepared = true;
}

void VocalEffects::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!isPrepared) return;
    
    // Validate input buffer
    if (buffer.getNumSamples() <= 0 || buffer.getNumChannels() <= 0) {
        return;
    }
    
    juce::ignoreUnused(midiMessages);
    
    const int numSamples = buffer.getNumSamples();
    const int numChannelsToProcess = juce::jmin(numChannels, buffer.getNumChannels());
    
    // Additional safety check
    if (numSamples <= 0 || numChannelsToProcess <= 0) {
        return;
    }
    
    // Process each channel
    for (int channel = 0; channel < numChannelsToProcess; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        // Apply vocal doubling first (for natural sound)
        if (doublingAmount > 0.0f)
        {
            vocalDoubler.process(channelData, numSamples, doublingAmount, doublingDelay, doublingDetune, sampleRate);
        }
        
        // Apply harmony generation
        if (harmonyAmount > 0.0f && harmonyVoices > 1)
        {
            harmonyGenerator.process(channelData, numSamples, harmonyVoices, harmonyAmount, harmonyInterval, harmonyDetune);
        }
        
        // Apply formant shifting
        if (formantAmount > 0.0f)
        {
            formantShifter.process(channelData, numSamples, formantShift, formantAmount);
        }
        
        // Apply delay
        if (delayMix > 0.0f)
        {
            delayLine.process(channelData, numSamples, delayTime, delayFeedback, delayMix, delayFilter, sampleRate);
        }
    }
    
    // Apply reverb (stereo processing)
    if (reverbAmount > 0.0f)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        
        // Update reverb parameters
        reverbParams.roomSize = reverbSize;
        reverbParams.damping = reverbDamping;
        reverbParams.wetLevel = reverbAmount;
        reverbParams.dryLevel = 1.0f - reverbAmount;
        reverbParams.width = reverbWidth;
        
        reverb.setParameters(reverbParams);
        reverb.process(context);
    }
}

void VocalEffects::reset()
{
    if (!isPrepared) return;
    
    // Reset reverb
    reverb.reset();
    reverbMixer.reset();
    
    // Reset delay line
    std::fill(delayLine.buffer.begin(), delayLine.buffer.end(), 0.0f);
    delayLine.writePosition = 0;
    
    // Reset harmony generator
    for (int i = 0; i < 8; ++i)
    {
        std::fill(harmonyGenerator.pitchShiftBuffers[i].begin(), harmonyGenerator.pitchShiftBuffers[i].end(), 0.0f);
        std::fill(harmonyGenerator.delayBuffers[i].begin(), harmonyGenerator.delayBuffers[i].end(), 0.0f);
        harmonyGenerator.writePositions[i] = 0;
    }
    
    // Reset vocal doubler
    std::fill(vocalDoubler.delayBuffer.begin(), vocalDoubler.delayBuffer.end(), 0.0f);
    vocalDoubler.writePosition = 0;
    
    // Reset formant shifter
    std::fill(formantShifter.formantBuffer.begin(), formantShifter.formantBuffer.end(), 0.0f);
}

//==============================================================================
void VocalEffects::setReverbAmount(float amount)
{
    // Validate and clamp parameter
    if (std::isnan(amount) || std::isinf(amount)) {
        amount = 0.0f;
    }
    reverbAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void VocalEffects::setReverbSize(float size)
{
    reverbSize = juce::jlimit(0.0f, 1.0f, size);
}

void VocalEffects::setReverbDamping(float damping)
{
    reverbDamping = juce::jlimit(0.0f, 1.0f, damping);
}

void VocalEffects::setReverbWidth(float width)
{
    reverbWidth = juce::jlimit(0.0f, 1.0f, width);
}

//==============================================================================
void VocalEffects::setDelayTime(float timeMs)
{
    delayTime = juce::jlimit(0.0f, 2000.0f, timeMs);
}

void VocalEffects::setDelayFeedback(float feedback)
{
    delayFeedback = juce::jlimit(0.0f, 0.95f, feedback);
}

void VocalEffects::setDelayMix(float mix)
{
    delayMix = juce::jlimit(0.0f, 1.0f, mix);
}

void VocalEffects::setDelayFilter(float cutoff)
{
    delayFilter = juce::jlimit(20.0f, 20000.0f, cutoff);
}

//==============================================================================
void VocalEffects::setHarmonyAmount(float amount)
{
    harmonyAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void VocalEffects::setHarmonyVoices(int voices)
{
    harmonyVoices = juce::jlimit(1, 8, voices);
}

void VocalEffects::setHarmonyInterval(int semitones)
{
    harmonyInterval = juce::jlimit(-12, 12, semitones);
}

void VocalEffects::setHarmonyDetune(float detune)
{
    harmonyDetune = juce::jlimit(-50.0f, 50.0f, detune);
}

//==============================================================================
void VocalEffects::setDoublingAmount(float amount)
{
    doublingAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void VocalEffects::setDoublingDelay(float delayMs)
{
    doublingDelay = juce::jlimit(0.0f, 100.0f, delayMs);
}

void VocalEffects::setDoublingDetune(float detune)
{
    doublingDetune = juce::jlimit(-50.0f, 50.0f, detune);
}

//==============================================================================
void VocalEffects::setFormantShift(float shift)
{
    formantShift = juce::jlimit(-1.0f, 1.0f, shift);
}

void VocalEffects::setFormantAmount(float amount)
{
    formantAmount = juce::jlimit(0.0f, 1.0f, amount);
}
