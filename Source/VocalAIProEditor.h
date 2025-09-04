#pragma once

#include <JuceHeader.h>
#include "VocalAIProPlugin.h"

//==============================================================================
/**
    VocalAI Pro Plugin Editor
    
    Modern, professional GUI with:
    - AI Pitch Tuning Controls
    - Vocal Effects Section
    - Real-time Visual Feedback
    - Preset Management
    - Responsive Design
*/
class VocalAIProEditor : public juce::AudioProcessorEditor,
                         public juce::Timer,
                         public juce::Button::Listener,
                         public juce::Slider::Listener,
                         public juce::ComboBox::Listener
{
public:
    //==============================================================================
    VocalAIProEditor (VocalAIProPlugin&);
    ~VocalAIProEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    //==============================================================================
    void buttonClicked (juce::Button* button) override;
    void sliderValueChanged (juce::Slider* slider) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;

private:
    //==============================================================================
    VocalAIProPlugin& audioProcessor;
    
    //==============================================================================
    // GUI Components - AI Pitch Tuning Section
    juce::GroupComponent aiPitchGroup;
    CustomKnob pitchCorrectionKnob;
    CustomKnob pitchSpeedKnob;
    juce::ToggleButton pitchCorrectionEnabledButton;
    juce::Label pitchCorrectionLabel;
    juce::Label pitchSpeedLabel;
    juce::Label pitchCorrectionEnabledLabel;
    
    //==============================================================================
    // GUI Components - Vocal Effects Section
    juce::GroupComponent vocalEffectsGroup;
    
    // Reverb Controls
    CustomKnob reverbAmountKnob;
    juce::Label reverbAmountLabel;
    
    // Delay Controls
    CustomKnob delayTimeKnob;
    CustomKnob delayFeedbackKnob;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    
    // Harmony Controls
    CustomKnob harmonyAmountKnob;
    CustomKnob harmonyVoicesKnob;
    juce::Label harmonyAmountLabel;
    juce::Label harmonyVoicesLabel;
    
    //==============================================================================
    // GUI Components - Master Controls
    juce::GroupComponent masterGroup;
    CustomKnob inputGainKnob;
    CustomKnob outputGainKnob;
    juce::ToggleButton bypassButton;
    juce::Label inputGainLabel;
    juce::Label outputGainLabel;
    juce::Label bypassLabel;
    
    //==============================================================================
    // GUI Components - Preset Management (Removed - using DAW preset dropdown)
    
    //==============================================================================
    // GUI Components - Visual Feedback
    juce::GroupComponent visualGroup;
    juce::Label currentPitchLabel;
    juce::Label pitchConfidenceLabel;
    juce::Label statusLabel;
    
    // Real-time Spectrum Analyzer
    class SpectrumAnalyzer : public juce::Component, public juce::Timer
    {
    public:
        SpectrumAnalyzer();
        ~SpectrumAnalyzer() override;
        
        void paint(juce::Graphics& g) override;
        void timerCallback() override;
        void updateSpectrum(const float* magnitudes, int numBins);
        
    private:
        std::vector<float> spectrumData;
        std::vector<float> smoothedSpectrum;
        static constexpr int numBins = 512;
        static constexpr int smoothingFactor = 8;
    };
    
    std::unique_ptr<SpectrumAnalyzer> spectrumAnalyzer;
    
    //==============================================================================
    // GUI Components - Branding
    juce::Label titleLabel;
    juce::Label versionLabel;
    juce::Label companyLabel;
    
    // Advanced Visual Components
    std::unique_ptr<juce::Component> pitchMeter;
    std::unique_ptr<juce::Component> confidenceMeter;
    std::unique_ptr<juce::Component> vocalDetector;
    std::unique_ptr<juce::Component> harmonicityDisplay;
    
    //==============================================================================
    // Parameter Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchCorrectionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> pitchCorrectionEnabledAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmonyAmountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmonyVoicesAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    
    //==============================================================================
    // Custom Knob Component
    class CustomKnob : public juce::Slider
    {
    public:
        CustomKnob();
        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
        
    private:
        void drawKnob(juce::Graphics& g, float angle);
        float getAngleFromValue(float value);
        float getValueFromAngle(float angle);
        
        static constexpr float minAngle = -2.5f;
        static constexpr float maxAngle = 2.5f;
        bool isDragging = false;
    };
    
    //==============================================================================
    // Preset Management (Removed - using DAW preset system)
    
    //==============================================================================
    // Visual Updates
    void updateVisualFeedback();
    
    // Advanced Visual Components
    void createAdvancedVisualComponents();
    
    //==============================================================================
    // Constants
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr int MARGIN = 20;
    static constexpr int GROUP_SPACING = 15;
    static constexpr int CONTROL_SPACING = 10;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAIProEditor)
};
