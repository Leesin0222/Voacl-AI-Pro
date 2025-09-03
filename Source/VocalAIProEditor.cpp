#include "VocalAIProEditor.h"

//==============================================================================
VocalAIProEditor::VocalAIProEditor (VocalAIProPlugin& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set window size
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Start timer for visual updates
    startTimer (50); // 20 FPS
    
    //==============================================================================
    // Setup AI Pitch Tuning Section
    setupGroupComponent(aiPitchGroup, "AI Pitch Tuning");
    addAndMakeVisible(aiPitchGroup);
    
    setupSlider(pitchCorrectionSlider, "%");
    pitchCorrectionSlider.setRange(0.0, 100.0, 0.1);
    pitchCorrectionSlider.setValue(50.0);
    addAndMakeVisible(pitchCorrectionSlider);
    
    setupSlider(pitchSpeedSlider, "%");
    pitchSpeedSlider.setRange(0.0, 100.0, 0.1);
    pitchSpeedSlider.setValue(50.0);
    addAndMakeVisible(pitchSpeedSlider);
    
    setupButton(pitchCorrectionEnabledButton);
    pitchCorrectionEnabledButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(pitchCorrectionEnabledButton);
    
    setupLabel(pitchCorrectionLabel, "Correction");
    setupLabel(pitchSpeedLabel, "Speed");
    setupLabel(pitchCorrectionEnabledLabel, "Enabled");
    
    //==============================================================================
    // Setup Vocal Effects Section
    setupGroupComponent(vocalEffectsGroup, "Vocal Effects");
    addAndMakeVisible(vocalEffectsGroup);
    
    // Reverb
    setupSlider(reverbAmountSlider, "%");
    reverbAmountSlider.setRange(0.0, 100.0, 0.1);
    reverbAmountSlider.setValue(0.0);
    addAndMakeVisible(reverbAmountSlider);
    setupLabel(reverbAmountLabel, "Reverb");
    
    // Delay
    setupSlider(delayTimeSlider, "ms");
    delayTimeSlider.setRange(0.0, 2000.0, 1.0);
    delayTimeSlider.setValue(250.0);
    addAndMakeVisible(delayTimeSlider);
    setupLabel(delayTimeLabel, "Delay Time");
    
    setupSlider(delayFeedbackSlider, "%");
    delayFeedbackSlider.setRange(0.0, 95.0, 0.1);
    delayFeedbackSlider.setValue(30.0);
    addAndMakeVisible(delayFeedbackSlider);
    setupLabel(delayFeedbackLabel, "Feedback");
    
    // Harmony
    setupSlider(harmonyAmountSlider, "%");
    harmonyAmountSlider.setRange(0.0, 100.0, 0.1);
    harmonyAmountSlider.setValue(0.0);
    addAndMakeVisible(harmonyAmountSlider);
    setupLabel(harmonyAmountLabel, "Harmony");
    
    setupSlider(harmonyVoicesSlider, "");
    harmonyVoicesSlider.setRange(1, 8, 1);
    harmonyVoicesSlider.setValue(2);
    addAndMakeVisible(harmonyVoicesSlider);
    setupLabel(harmonyVoicesLabel, "Voices");
    
    //==============================================================================
    // Setup Master Controls
    setupGroupComponent(masterGroup, "Master");
    addAndMakeVisible(masterGroup);
    
    setupSlider(inputGainSlider, "dB");
    inputGainSlider.setRange(-24.0, 24.0, 0.1);
    inputGainSlider.setValue(0.0);
    addAndMakeVisible(inputGainSlider);
    setupLabel(inputGainLabel, "Input Gain");
    
    setupSlider(outputGainSlider, "dB");
    outputGainSlider.setRange(-24.0, 24.0, 0.1);
    outputGainSlider.setValue(0.0);
    addAndMakeVisible(outputGainSlider);
    setupLabel(outputGainLabel, "Output Gain");
    
    setupButton(bypassButton);
    bypassButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(bypassButton);
    setupLabel(bypassLabel, "Bypass");
    
    //==============================================================================
    // Setup Preset Management
    setupGroupComponent(presetGroup, "Presets");
    addAndMakeVisible(presetGroup);
    
    presetComboBox.addItem("Default", 1);
    presetComboBox.addItem("Vocal Doubling", 2);
    presetComboBox.addItem("Harmony", 3);
    presetComboBox.addItem("Reverb", 4);
    presetComboBox.addItem("Delay", 5);
    presetComboBox.setSelectedId(1);
    addAndMakeVisible(presetComboBox);
    
    savePresetButton.setButtonText("Save");
    savePresetButton.addListener(this);
    addAndMakeVisible(savePresetButton);
    
    deletePresetButton.setButtonText("Delete");
    deletePresetButton.addListener(this);
    addAndMakeVisible(deletePresetButton);
    
    setupLabel(presetLabel, "Preset");
    
    //==============================================================================
    // Setup Visual Feedback
    setupGroupComponent(visualGroup, "Analysis");
    addAndMakeVisible(visualGroup);
    
    setupLabel(currentPitchLabel, "Pitch: -- Hz");
    setupLabel(pitchConfidenceLabel, "Confidence: --%");
    setupLabel(statusLabel, "Status: Ready");
    
    //==============================================================================
    // Setup Branding
    titleLabel.setText("VocalAI Pro", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4a90e2));
    addAndMakeVisible(titleLabel);
    
    versionLabel.setText("v1.0.0", juce::dontSendNotification);
    versionLabel.setFont(juce::Font(12.0f));
    versionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(versionLabel);
    
    companyLabel.setText("VocalAI", juce::dontSendNotification);
    companyLabel.setFont(juce::Font(10.0f));
    companyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(companyLabel);
    
    //==============================================================================
    // Setup Parameter Attachments
    pitchCorrectionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "pitchCorrection", pitchCorrectionSlider);
    
    pitchSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "pitchSpeed", pitchSpeedSlider);
    
    pitchCorrectionEnabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "pitchCorrectionEnabled", pitchCorrectionEnabledButton);
    
    reverbAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "reverbAmount", reverbAmountSlider);
    
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "delayTime", delayTimeSlider);
    
    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "delayFeedback", delayFeedbackSlider);
    
    harmonyAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "harmonyAmount", harmonyAmountSlider);
    
    harmonyVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "harmonyVoices", harmonyVoicesSlider);
    
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "inputGain", inputGainSlider);
    
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "outputGain", outputGainSlider);
    
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "bypass", bypassButton);
    
    //==============================================================================
    // Add listeners
    pitchCorrectionSlider.addListener(this);
    pitchSpeedSlider.addListener(this);
    pitchCorrectionEnabledButton.addListener(this);
    reverbAmountSlider.addListener(this);
    delayTimeSlider.addListener(this);
    delayFeedbackSlider.addListener(this);
    harmonyAmountSlider.addListener(this);
    harmonyVoicesSlider.addListener(this);
    inputGainSlider.addListener(this);
    outputGainSlider.addListener(this);
    bypassButton.addListener(this);
}

VocalAIProEditor::~VocalAIProEditor()
{
}

//==============================================================================
void VocalAIProEditor::paint (juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient gradient(juce::Colour(0xff2c3e50), 0, 0,
                                 juce::Colour(0xff34495e), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Draw subtle grid pattern
    g.setColour(juce::Colour(0x10ffffff));
    for (int x = 0; x < getWidth(); x += 20)
        g.drawVerticalLine(x, 0, getHeight());
    for (int y = 0; y < getHeight(); y += 20)
        g.drawHorizontalLine(y, 0, getWidth());
}

void VocalAIProEditor::resized()
{
    auto bounds = getLocalBounds().reduced(MARGIN);
    
    // Title section
    auto titleArea = bounds.removeFromTop(60);
    titleLabel.setBounds(titleArea.removeFromTop(30));
    versionLabel.setBounds(titleArea.removeFromTop(20));
    companyLabel.setBounds(titleArea.removeFromTop(15));
    
    bounds.removeFromTop(GROUP_SPACING);
    
    // Main content area
    auto leftColumn = bounds.removeFromLeft(bounds.getWidth() / 2);
    auto rightColumn = bounds;
    
    // Left column
    auto aiPitchArea = leftColumn.removeFromTop(200);
    aiPitchGroup.setBounds(aiPitchArea);
    
    auto aiPitchContent = aiPitchArea.reduced(15, 25);
    auto aiPitchTop = aiPitchContent.removeFromTop(aiPitchContent.getHeight() / 2);
    auto aiPitchBottom = aiPitchContent;
    
    // AI Pitch controls
    pitchCorrectionLabel.setBounds(aiPitchTop.removeFromLeft(80));
    pitchCorrectionSlider.setBounds(aiPitchTop.removeFromLeft(120));
    aiPitchTop.removeFromLeft(CONTROL_SPACING);
    pitchSpeedLabel.setBounds(aiPitchTop.removeFromLeft(80));
    pitchSpeedSlider.setBounds(aiPitchTop);
    
    pitchCorrectionEnabledLabel.setBounds(aiPitchBottom.removeFromLeft(80));
    pitchCorrectionEnabledButton.setBounds(aiPitchBottom.removeFromLeft(50));
    
    leftColumn.removeFromTop(GROUP_SPACING);
    
    auto vocalEffectsArea = leftColumn.removeFromTop(250);
    vocalEffectsGroup.setBounds(vocalEffectsArea);
    
    auto vocalEffectsContent = vocalEffectsArea.reduced(15, 25);
    
    // Reverb
    auto reverbArea = vocalEffectsContent.removeFromTop(50);
    reverbAmountLabel.setBounds(reverbArea.removeFromLeft(80));
    reverbAmountSlider.setBounds(reverbArea);
    
    // Delay
    auto delayArea = vocalEffectsContent.removeFromTop(50);
    delayTimeLabel.setBounds(delayArea.removeFromLeft(80));
    delayTimeSlider.setBounds(delayArea.removeFromLeft(100));
    delayArea.removeFromLeft(CONTROL_SPACING);
    delayFeedbackLabel.setBounds(delayArea.removeFromLeft(80));
    delayFeedbackSlider.setBounds(delayArea);
    
    // Harmony
    auto harmonyArea = vocalEffectsContent.removeFromTop(50);
    harmonyAmountLabel.setBounds(harmonyArea.removeFromLeft(80));
    harmonyAmountSlider.setBounds(harmonyArea.removeFromLeft(100));
    harmonyArea.removeFromLeft(CONTROL_SPACING);
    harmonyVoicesLabel.setBounds(harmonyArea.removeFromLeft(80));
    harmonyVoicesSlider.setBounds(harmonyArea);
    
    // Right column
    auto masterArea = rightColumn.removeFromTop(150);
    masterGroup.setBounds(masterArea);
    
    auto masterContent = masterArea.reduced(15, 25);
    
    // Master controls
    auto inputGainArea = masterContent.removeFromTop(40);
    inputGainLabel.setBounds(inputGainArea.removeFromLeft(80));
    inputGainSlider.setBounds(inputGainArea.removeFromLeft(120));
    inputGainArea.removeFromLeft(CONTROL_SPACING);
    outputGainLabel.setBounds(inputGainArea.removeFromLeft(80));
    outputGainSlider.setBounds(inputGainArea);
    
    auto bypassArea = masterContent.removeFromTop(40);
    bypassLabel.setBounds(bypassArea.removeFromLeft(80));
    bypassButton.setBounds(bypassArea.removeFromLeft(50));
    
    rightColumn.removeFromTop(GROUP_SPACING);
    
    auto presetArea = rightColumn.removeFromTop(120);
    presetGroup.setBounds(presetArea);
    
    auto presetContent = presetArea.reduced(15, 25);
    
    // Preset controls
    auto presetComboArea = presetContent.removeFromTop(30);
    presetLabel.setBounds(presetComboArea.removeFromLeft(80));
    presetComboBox.setBounds(presetComboArea.removeFromLeft(150));
    
    auto presetButtonsArea = presetContent.removeFromTop(30);
    savePresetButton.setBounds(presetButtonsArea.removeFromLeft(60));
    presetButtonsArea.removeFromLeft(CONTROL_SPACING);
    deletePresetButton.setBounds(presetButtonsArea.removeFromLeft(60));
    
    auto visualArea = rightColumn.removeFromTop(150);
    visualGroup.setBounds(visualArea);
    
    auto visualContent = visualArea.reduced(15, 25);
    
    // Visual feedback
    currentPitchLabel.setBounds(visualContent.removeFromTop(25));
    pitchConfidenceLabel.setBounds(visualContent.removeFromTop(25));
    statusLabel.setBounds(visualContent.removeFromTop(25));
}

void VocalAIProEditor::timerCallback()
{
    updateVisualFeedback();
}

//==============================================================================
void VocalAIProEditor::buttonClicked(juce::Button* button)
{
    if (button == &savePresetButton)
    {
        saveCurrentPreset();
    }
    else if (button == &deletePresetButton)
    {
        deleteCurrentPreset();
    }
}

void VocalAIProEditor::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
    // Parameter changes are handled by attachments
}

//==============================================================================
void VocalAIProEditor::setupSlider(juce::Slider& slider, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a90e2));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff7f8c8d));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe74c3c));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffecf0f1));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff34495e));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff7f8c8d));
    
    if (!suffix.isEmpty())
        slider.setTextValueSuffix(" " + suffix);
}

void VocalAIProEditor::setupButton(juce::Button& button)
{
    button.setColour(juce::Button::buttonColourId, juce::Colour(0xff34495e));
    button.setColour(juce::Button::buttonOnColourId, juce::Colour(0xff4a90e2));
    button.setColour(juce::Button::textColourOnId, juce::Colour(0xffecf0f1));
    button.setColour(juce::Button::textColourOffId, juce::Colour(0xffbdc3c7));
}

void VocalAIProEditor::setupLabel(juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffecf0f1));
    label.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(label);
}

void VocalAIProEditor::setupGroupComponent(juce::GroupComponent& group, const juce::String& text)
{
    group.setText(text);
    group.setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff7f8c8d));
    group.setColour(juce::GroupComponent::textColourId, juce::Colour(0xff4a90e2));
    group.setFont(juce::Font(14.0f, juce::Font::bold));
}

//==============================================================================
void VocalAIProEditor::loadPresets()
{
    // Load presets from file or create default presets
    updatePresetComboBox();
}

void VocalAIProEditor::saveCurrentPreset()
{
    // Save current parameter values as a new preset
    juce::String presetName = juce::String("Preset ") + juce::String(presetComboBox.getNumItems() + 1);
    presetComboBox.addItem(presetName, presetComboBox.getNumItems() + 1);
    presetComboBox.setSelectedId(presetComboBox.getNumItems());
}

void VocalAIProEditor::deleteCurrentPreset()
{
    int selectedId = presetComboBox.getSelectedId();
    if (selectedId > 1) // Don't delete default preset
    {
        presetComboBox.removeItem(presetComboBox.getSelectedItemIndex());
        presetComboBox.setSelectedId(1); // Select default
    }
}

void VocalAIProEditor::applyPreset(int presetIndex)
{
    juce::ignoreUnused(presetIndex);
    // Apply preset parameter values
    // This would load specific parameter combinations
}

void VocalAIProEditor::updateVisualFeedback()
{
    // Update real-time visual feedback
    // This would show current pitch, confidence, etc.
    if (audioProcessor.getAIPitchTuner())
    {
        float currentPitch = audioProcessor.getAIPitchTuner()->getCurrentPitch();
        float confidence = audioProcessor.getAIPitchTuner()->getPitchConfidence();
        bool detected = audioProcessor.getAIPitchTuner()->isPitchDetected();
        
        if (detected && currentPitch > 0.0f)
        {
            currentPitchLabel.setText("Pitch: " + juce::String(currentPitch, 1) + " Hz", juce::dontSendNotification);
            pitchConfidenceLabel.setText("Confidence: " + juce::String(confidence * 100.0f, 1) + "%", juce::dontSendNotification);
            statusLabel.setText("Status: Processing", juce::dontSendNotification);
        }
        else
        {
            currentPitchLabel.setText("Pitch: -- Hz", juce::dontSendNotification);
            pitchConfidenceLabel.setText("Confidence: --%", juce::dontSendNotification);
            statusLabel.setText("Status: No Signal", juce::dontSendNotification);
        }
    }
}

void VocalAIProEditor::updatePresetComboBox()
{
    // Update preset combo box with available presets
    // This would load from file or create default presets
}
