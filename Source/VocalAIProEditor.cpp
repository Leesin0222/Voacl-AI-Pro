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
    
    // GOD Presets - 음악 제작자를 위한 최적화된 설정들
    presetComboBox.addItem("🔥 GOD - Perfect Vocal", 1);
    presetComboBox.addItem("🎤 GOD - Studio Master", 2);
    presetComboBox.addItem("🎵 GOD - Pop Hit Maker", 3);
    presetComboBox.addItem("🎸 GOD - Rock Legend", 4);
    presetComboBox.addItem("🎹 GOD - R&B Smooth", 5);
    presetComboBox.addItem("🎧 GOD - Trap Vocal", 6);
    presetComboBox.addItem("🎭 GOD - Live Performance", 7);
    presetComboBox.addItem("🎪 GOD - Creative Chaos", 8);
    
    // 기본 프리셋들
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
        applyPreset(selectedId);
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
    // GOD Presets - 음악 제작자를 위한 최적화된 설정들
    switch (presetIndex)
    {
        case 1: // 🔥 GOD - Perfect Vocal
            pitchCorrectionSlider.setValue(85.0);      // 강력한 피치 보정
            pitchSpeedSlider.setValue(70.0);           // 빠른 반응
            reverbAmountSlider.setValue(25.0);         // 적당한 공간감
            delayTimeSlider.setValue(200.0);           // 짧은 딜레이
            delayFeedbackSlider.setValue(20.0);        // 낮은 피드백
            harmonyAmountSlider.setValue(15.0);        // 미묘한 하모니
            harmonyVoicesSlider.setValue(2);           // 2개 보이스
            inputGainSlider.setValue(2.0);             // 약간의 부스트
            outputGainSlider.setValue(0.0);            // 출력 게인 0
            break;
            
        case 2: // 🎤 GOD - Studio Master
            pitchCorrectionSlider.setValue(95.0);      // 최고 수준 피치 보정
            pitchSpeedSlider.setValue(80.0);           // 매우 빠른 반응
            reverbAmountSlider.setValue(35.0);         // 스튜디오 리버브
            delayTimeSlider.setValue(300.0);           // 중간 딜레이
            delayFeedbackSlider.setValue(25.0);        // 적당한 피드백
            harmonyAmountSlider.setValue(20.0);        // 부드러운 하모니
            harmonyVoicesSlider.setValue(3);           // 3개 보이스
            inputGainSlider.setValue(1.5);             // 미묘한 부스트
            outputGainSlider.setValue(-1.0);           // 약간의 출력 감소
            break;
            
        case 3: // 🎵 GOD - Pop Hit Maker
            pitchCorrectionSlider.setValue(90.0);      // 팝 스타일 피치 보정
            pitchSpeedSlider.setValue(75.0);           // 빠른 반응
            reverbAmountSlider.setValue(40.0);         // 팝 리버브
            delayTimeSlider.setValue(250.0);           // 팝 딜레이
            delayFeedbackSlider.setValue(30.0);        // 팝 피드백
            harmonyAmountSlider.setValue(45.0);        // 강한 하모니
            harmonyVoicesSlider.setValue(4);           // 4개 보이스
            inputGainSlider.setValue(3.0);             // 팝 부스트
            outputGainSlider.setValue(0.5);            // 약간의 출력 부스트
            break;
            
        case 4: // 🎸 GOD - Rock Legend
            pitchCorrectionSlider.setValue(60.0);      // 자연스러운 피치 보정
            pitchSpeedSlider.setValue(50.0);           // 중간 반응
            reverbAmountSlider.setValue(60.0);         // 강한 리버브
            delayTimeSlider.setValue(500.0);           // 록 딜레이
            delayFeedbackSlider.setValue(40.0);        // 록 피드백
            harmonyAmountSlider.setValue(30.0);        // 록 하모니
            harmonyVoicesSlider.setValue(3);           // 3개 보이스
            inputGainSlider.setValue(4.0);             // 록 부스트
            outputGainSlider.setValue(1.0);            // 출력 부스트
            break;
            
        case 5: // 🎹 GOD - R&B Smooth
            pitchCorrectionSlider.setValue(80.0);      // 부드러운 피치 보정
            pitchSpeedSlider.setValue(60.0);           // 부드러운 반응
            reverbAmountSlider.setValue(50.0);         // R&B 리버브
            delayTimeSlider.setValue(400.0);           // R&B 딜레이
            delayFeedbackSlider.setValue(35.0);        // R&B 피드백
            harmonyAmountSlider.setValue(55.0);        // 강한 하모니
            harmonyVoicesSlider.setValue(5);           // 5개 보이스
            inputGainSlider.setValue(2.5);             // R&B 부스트
            outputGainSlider.setValue(0.0);            // 출력 게인 0
            break;
            
        case 6: // 🎧 GOD - Trap Vocal
            pitchCorrectionSlider.setValue(70.0);      // 트랩 피치 보정
            pitchSpeedSlider.setValue(65.0);           // 트랩 반응
            reverbAmountSlider.setValue(70.0);         // 트랩 리버브
            delayTimeSlider.setValue(150.0);           // 트랩 딜레이
            delayFeedbackSlider.setValue(50.0);        // 트랩 피드백
            harmonyAmountSlider.setValue(25.0);        // 트랩 하모니
            harmonyVoicesSlider.setValue(2);           // 2개 보이스
            inputGainSlider.setValue(5.0);             // 트랩 부스트
            outputGainSlider.setValue(2.0);            // 강한 출력 부스트
            break;
            
        case 7: // 🎭 GOD - Live Performance
            pitchCorrectionSlider.setValue(75.0);      // 라이브 피치 보정
            pitchSpeedSlider.setValue(85.0);           // 매우 빠른 반응
            reverbAmountSlider.setValue(30.0);         // 라이브 리버브
            delayTimeSlider.setValue(200.0);           // 라이브 딜레이
            delayFeedbackSlider.setValue(15.0);        // 낮은 피드백
            harmonyAmountSlider.setValue(10.0);        // 미묘한 하모니
            harmonyVoicesSlider.setValue(2);           // 2개 보이스
            inputGainSlider.setValue(1.0);             // 라이브 부스트
            outputGainSlider.setValue(0.0);            // 출력 게인 0
            break;
            
        case 8: // 🎪 GOD - Creative Chaos
            pitchCorrectionSlider.setValue(50.0);      // 창의적 피치 보정
            pitchSpeedSlider.setValue(40.0);           // 창의적 반응
            reverbAmountSlider.setValue(80.0);         // 창의적 리버브
            delayTimeSlider.setValue(800.0);           // 창의적 딜레이
            delayFeedbackSlider.setValue(60.0);        // 창의적 피드백
            harmonyAmountSlider.setValue(70.0);        // 창의적 하모니
            harmonyVoicesSlider.setValue(6);           // 6개 보이스
            inputGainSlider.setValue(3.5);             // 창의적 부스트
            outputGainSlider.setValue(1.5);            // 창의적 출력 부스트
            break;
            
        default:
            // 기본 프리셋들
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
