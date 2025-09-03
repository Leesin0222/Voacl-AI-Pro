#include "VocalAIProEditor.h"
#include "AIPitchTuner.h"

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
    
    // Professional Vocal Presets - Optimized for Music Producers
    presetComboBox.addItem("Perfect Vocal", 1);
    presetComboBox.addItem("Studio Master", 2);
    presetComboBox.addItem("Pop Hit Maker", 3);
    presetComboBox.addItem("Rock Legend", 4);
    presetComboBox.addItem("R&B Smooth", 5);
    presetComboBox.addItem("Trap Vocal", 6);
    presetComboBox.addItem("Live Performance", 7);
    presetComboBox.addItem("Creative Chaos", 8);
    
    // Basic Presets
    presetComboBox.addItem("Default", 9);
    presetComboBox.addItem("Vocal Doubling", 10);
    presetComboBox.addItem("Harmony", 11);
    presetComboBox.addItem("Reverb", 12);
    presetComboBox.addItem("Delay", 13);
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
    // Initialize Spectrum Analyzer
    spectrumAnalyzer = std::make_unique<SpectrumAnalyzer>();
    addAndMakeVisible(spectrumAnalyzer.get());
    
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
    presetComboBox.addListener(this);
}

VocalAIProEditor::~VocalAIProEditor()
{
}

//==============================================================================
void VocalAIProEditor::paint (juce::Graphics& g)
{
    // Enhanced background with multiple gradients
    juce::ColourGradient mainGradient(juce::Colour(0xff1a252f), 0, 0,
                                     juce::Colour(0xff2c3e50), 0, getHeight(), false);
    g.setGradientFill(mainGradient);
    g.fillAll();
    
    // Add subtle overlay gradient
    juce::ColourGradient overlayGradient(juce::Colour(0x20ffffff), 0, 0,
                                        juce::Colour(0x00000000), 0, getHeight() / 2, false);
    g.setGradientFill(overlayGradient);
    g.fillAll();
    
    // Draw modern grid pattern with varying opacity
    g.setColour(juce::Colour(0x08ffffff));
    for (int x = 0; x < getWidth(); x += 40)
        g.drawVerticalLine(x, 0, getHeight());
    for (int y = 0; y < getHeight(); y += 40)
        g.drawHorizontalLine(y, 0, getWidth());
    
    // Add subtle border
    g.setColour(juce::Colour(0xff3498db));
    g.drawRect(getLocalBounds(), 2);
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
    
    // Spectrum analyzer
    if (spectrumAnalyzer)
        spectrumAnalyzer->setBounds(visualContent);
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

void VocalAIProEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &presetComboBox)
    {
        int selectedId = presetComboBox.getSelectedId();
        
        // Validate preset ID range
        if (selectedId >= 1 && selectedId <= 13) {
            applyPreset(selectedId);
        } else {
            // Fallback to default preset
            presetComboBox.setSelectedId(1);
            applyPreset(1);
        }
    }
}

//==============================================================================
void VocalAIProEditor::setupSlider(juce::Slider& slider, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 24);
    
    // Modern gradient colors
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff3498db));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2c3e50));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe74c3c));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffecf0f1));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff34495e));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff3498db));
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff3498db));
    
    // Enhanced visual feedback
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff2c3e50));
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff3498db));
    
    if (!suffix.isEmpty())
        slider.setTextValueSuffix(" " + suffix);
}

void VocalAIProEditor::setupButton(juce::Button& button)
{
    button.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff34495e));
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4a90e2));
    button.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffecf0f1));
    button.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffbdc3c7));
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
    // JUCE 8에서는 setFont가 제거되었으므로 제거
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
        // JUCE 8에서는 removeItem이 제거되었으므로 clear하고 다시 추가
        int currentIndex = presetComboBox.getSelectedItemIndex();
        if (currentIndex >= 0)
        {
            // 현재 선택된 항목을 제외하고 모든 항목을 다시 추가
            juce::StringArray items;
            for (int i = 0; i < presetComboBox.getNumItems(); ++i)
            {
                if (i != currentIndex)
                {
                    items.add(presetComboBox.getItemText(i));
                }
            }
            
            presetComboBox.clear();
            for (int i = 0; i < items.size(); ++i)
            {
                presetComboBox.addItem(items[i], i + 1);
            }
            presetComboBox.setSelectedId(1); // Select default
        }
    }
}

void VocalAIProEditor::applyPreset(int presetIndex)
{
    // Professional Vocal Presets - Optimized for Music Producers
    switch (presetIndex)
    {
        case 1: // Perfect Vocal
            pitchCorrectionSlider.setValue(85.0);      // Strong pitch correction
            pitchSpeedSlider.setValue(70.0);           // Fast response
            reverbAmountSlider.setValue(25.0);         // Moderate space
            delayTimeSlider.setValue(200.0);           // Short delay
            delayFeedbackSlider.setValue(20.0);        // Low feedback
            harmonyAmountSlider.setValue(15.0);        // Subtle harmony
            harmonyVoicesSlider.setValue(2);           // 2 voices
            inputGainSlider.setValue(2.0);             // Slight boost
            outputGainSlider.setValue(0.0);            // Output gain 0
            break;
            
        case 2: // Studio Master
            pitchCorrectionSlider.setValue(95.0);      // Maximum pitch correction
            pitchSpeedSlider.setValue(80.0);           // Very fast response
            reverbAmountSlider.setValue(35.0);         // Studio reverb
            delayTimeSlider.setValue(300.0);           // Medium delay
            delayFeedbackSlider.setValue(25.0);        // Moderate feedback
            harmonyAmountSlider.setValue(20.0);        // Smooth harmony
            harmonyVoicesSlider.setValue(3);           // 3 voices
            inputGainSlider.setValue(1.5);             // Subtle boost
            outputGainSlider.setValue(-1.0);           // Slight output reduction
            break;
            
        case 3: // Pop Hit Maker
            pitchCorrectionSlider.setValue(90.0);      // Pop style pitch correction
            pitchSpeedSlider.setValue(75.0);           // Fast response
            reverbAmountSlider.setValue(40.0);         // Pop reverb
            delayTimeSlider.setValue(250.0);           // Pop delay
            delayFeedbackSlider.setValue(30.0);        // Pop feedback
            harmonyAmountSlider.setValue(45.0);        // Strong harmony
            harmonyVoicesSlider.setValue(4);           // 4 voices
            inputGainSlider.setValue(3.0);             // Pop boost
            outputGainSlider.setValue(0.5);            // Slight output boost
            break;
            
        case 4: // Rock Legend
            pitchCorrectionSlider.setValue(60.0);      // Natural pitch correction
            pitchSpeedSlider.setValue(50.0);           // Medium response
            reverbAmountSlider.setValue(60.0);         // Strong reverb
            delayTimeSlider.setValue(500.0);           // Rock delay
            delayFeedbackSlider.setValue(40.0);        // Rock feedback
            harmonyAmountSlider.setValue(30.0);        // Rock harmony
            harmonyVoicesSlider.setValue(3);           // 3 voices
            inputGainSlider.setValue(4.0);             // Rock boost
            outputGainSlider.setValue(1.0);            // Output boost
            break;
            
        case 5: // R&B Smooth
            pitchCorrectionSlider.setValue(80.0);      // Smooth pitch correction
            pitchSpeedSlider.setValue(60.0);           // Smooth response
            reverbAmountSlider.setValue(50.0);         // R&B reverb
            delayTimeSlider.setValue(400.0);           // R&B delay
            delayFeedbackSlider.setValue(35.0);        // R&B feedback
            harmonyAmountSlider.setValue(55.0);        // Strong harmony
            harmonyVoicesSlider.setValue(5);           // 5 voices
            inputGainSlider.setValue(2.5);             // R&B boost
            outputGainSlider.setValue(0.0);            // Output gain 0
            break;
            
        case 6: // Trap Vocal
            pitchCorrectionSlider.setValue(70.0);      // Trap pitch correction
            pitchSpeedSlider.setValue(65.0);           // Trap response
            reverbAmountSlider.setValue(70.0);         // Trap reverb
            delayTimeSlider.setValue(150.0);           // Trap delay
            delayFeedbackSlider.setValue(40.0);        // Safe feedback
            harmonyAmountSlider.setValue(25.0);        // Trap harmony
            harmonyVoicesSlider.setValue(2);           // 2 voices
            inputGainSlider.setValue(3.0);             // Safe boost
            outputGainSlider.setValue(1.0);            // Safe output boost
            break;
            
        case 7: // Live Performance
            pitchCorrectionSlider.setValue(75.0);      // Live pitch correction
            pitchSpeedSlider.setValue(85.0);           // Very fast response
            reverbAmountSlider.setValue(30.0);         // Live reverb
            delayTimeSlider.setValue(200.0);           // Live delay
            delayFeedbackSlider.setValue(15.0);        // Low feedback
            harmonyAmountSlider.setValue(10.0);        // Subtle harmony
            harmonyVoicesSlider.setValue(2);           // 2 voices
            inputGainSlider.setValue(1.0);             // Live boost
            outputGainSlider.setValue(0.0);            // Output gain 0
            break;
            
        case 8: // Creative Chaos
            pitchCorrectionSlider.setValue(50.0);      // Creative pitch correction
            pitchSpeedSlider.setValue(40.0);           // Creative response
            reverbAmountSlider.setValue(60.0);         // Safe reverb
            delayTimeSlider.setValue(600.0);           // Safe delay
            delayFeedbackSlider.setValue(40.0);        // Safe feedback
            harmonyAmountSlider.setValue(50.0);        // Safe harmony
            harmonyVoicesSlider.setValue(4);           // Safe voice count
            inputGainSlider.setValue(2.0);             // Safe boost
            outputGainSlider.setValue(0.5);            // Safe output boost
            break;
            
        default:
            // Default presets
            break;
    }
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

//==============================================================================
// SpectrumAnalyzer Implementation
VocalAIProEditor::SpectrumAnalyzer::SpectrumAnalyzer()
{
    spectrumData.resize(numBins, 0.0f);
    smoothedSpectrum.resize(numBins, 0.0f);
    startTimer(30); // 30 FPS
}

VocalAIProEditor::SpectrumAnalyzer::~SpectrumAnalyzer()
{
    stopTimer();
}

void VocalAIProEditor::SpectrumAnalyzer::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Background
    g.setColour(juce::Colour(0xff2c3e50));
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(juce::Colour(0xff3498db));
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);
    
    // Draw spectrum bars
    const float barWidth = bounds.getWidth() / numBins;
    const float maxHeight = bounds.getHeight() - 20.0f;
    
    for (int i = 0; i < numBins; ++i)
    {
        const float barHeight = smoothedSpectrum[i] * maxHeight;
        const float x = bounds.getX() + i * barWidth;
        const float y = bounds.getBottom() - barHeight - 10.0f;
        
        // Gradient for each bar
        juce::ColourGradient gradient(juce::Colour(0xffe74c3c), x, y,
                                     juce::Colour(0xff3498db), x, y + barHeight, false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(x + 1, y, barWidth - 2, barHeight, 2.0f);
    }
}

void VocalAIProEditor::SpectrumAnalyzer::timerCallback()
{
    repaint();
}

void VocalAIProEditor::SpectrumAnalyzer::updateSpectrum(const float* magnitudes, int numBins)
{
    if (numBins != this->numBins) return;
    
    // Update spectrum data with smoothing
    for (int i = 0; i < numBins; ++i)
    {
        spectrumData[i] = juce::jlimit(0.0f, 1.0f, magnitudes[i]);
        
        // Apply smoothing
        smoothedSpectrum[i] = smoothedSpectrum[i] * 0.8f + spectrumData[i] * 0.2f;
    }
}
