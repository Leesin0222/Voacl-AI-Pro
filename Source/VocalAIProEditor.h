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
                         public juce::Slider::Listener
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

private:
    //==============================================================================
    VocalAIProPlugin& audioProcessor;
    
    //==============================================================================
    // GUI Components - AI Pitch Tuning Section
    juce::GroupComponent aiPitchGroup;
    juce::Slider pitchCorrectionSlider;
    juce::Slider pitchSpeedSlider;
    juce::ToggleButton pitchCorrectionEnabledButton;
    juce::Label pitchCorrectionLabel;
    juce::Label pitchSpeedLabel;
    juce::Label pitchCorrectionEnabledLabel;
    
    //==============================================================================
    // GUI Components - Vocal Effects Section
    juce::GroupComponent vocalEffectsGroup;
    
    // Reverb Controls
    juce::Slider reverbAmountSlider;
    juce::Label reverbAmountLabel;
    
    // Delay Controls
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    
    // Harmony Controls
    juce::Slider harmonyAmountSlider;
    juce::Slider harmonyVoicesSlider;
    juce::Label harmonyAmountLabel;
    juce::Label harmonyVoicesLabel;
    
    //==============================================================================
    // GUI Components - Master Controls
    juce::GroupComponent masterGroup;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::ToggleButton bypassButton;
    juce::Label inputGainLabel;
    juce::Label outputGainLabel;
    juce::Label bypassLabel;
    
    //==============================================================================
    // GUI Components - Preset Management
    juce::GroupComponent presetGroup;
    juce::ComboBox presetComboBox;
    juce::TextButton savePresetButton;
    juce::TextButton deletePresetButton;
    juce::Label presetLabel;
    
    //==============================================================================
    // GUI Components - Visual Feedback
    juce::GroupComponent visualGroup;
    juce::Label currentPitchLabel;
    juce::Label pitchConfidenceLabel;
    juce::Label statusLabel;
    
    //==============================================================================
    // GUI Components - Branding
    juce::Label titleLabel;
    juce::Label versionLabel;
    juce::Label companyLabel;
    
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
    // GUI Styling
    void setupSlider(juce::Slider& slider, const juce::String& suffix = "");
    void setupButton(juce::Button& button);
    void setupLabel(juce::Label& label, const juce::String& text);
    void setupGroupComponent(juce::GroupComponent& group, const juce::String& text);
    
    //==============================================================================
    // Preset Management
    void loadPresets();
    void saveCurrentPreset();
    void deleteCurrentPreset();
    void applyPreset(int presetIndex);
    
    //==============================================================================
    // Visual Updates
    void updateVisualFeedback();
    void updatePresetComboBox();
    
    //==============================================================================
    // Constants
    static constexpr int WINDOW_WIDTH = 800;
    static constexpr int WINDOW_HEIGHT = 600;
    static constexpr int MARGIN = 20;
    static constexpr int GROUP_SPACING = 15;
    static constexpr int CONTROL_SPACING = 10;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAIProEditor)
};
