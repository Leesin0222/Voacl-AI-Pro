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
    
    pitchCorrectionKnob.setRange(0.0, 100.0, 0.1);
    pitchCorrectionKnob.setValue(50.0);
    pitchCorrectionKnob.setTextValueSuffix(" %");
    addAndMakeVisible(pitchCorrectionKnob);
    
    pitchSpeedKnob.setRange(0.0, 100.0, 0.1);
    pitchSpeedKnob.setValue(50.0);
    pitchSpeedKnob.setTextValueSuffix(" %");
    addAndMakeVisible(pitchSpeedKnob);
    
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
    reverbAmountKnob.setRange(0.0, 100.0, 0.1);
    reverbAmountKnob.setValue(0.0);
    reverbAmountKnob.setTextValueSuffix(" %");
    addAndMakeVisible(reverbAmountKnob);
    setupLabel(reverbAmountLabel, "Reverb");
    
    // Delay
    delayTimeKnob.setRange(0.0, 2000.0, 1.0);
    delayTimeKnob.setValue(250.0);
    delayTimeKnob.setTextValueSuffix(" ms");
    addAndMakeVisible(delayTimeKnob);
    setupLabel(delayTimeLabel, "Delay Time");
    
    delayFeedbackKnob.setRange(0.0, 95.0, 0.1);
    delayFeedbackKnob.setValue(30.0);
    delayFeedbackKnob.setTextValueSuffix(" %");
    addAndMakeVisible(delayFeedbackKnob);
    setupLabel(delayFeedbackLabel, "Feedback");
    
    // Harmony
    harmonyAmountKnob.setRange(0.0, 100.0, 0.1);
    harmonyAmountKnob.setValue(0.0);
    harmonyAmountKnob.setTextValueSuffix(" %");
    addAndMakeVisible(harmonyAmountKnob);
    setupLabel(harmonyAmountLabel, "Harmony");
    
    harmonyVoicesKnob.setRange(1, 8, 1);
    harmonyVoicesKnob.setValue(2);
    addAndMakeVisible(harmonyVoicesKnob);
    setupLabel(harmonyVoicesLabel, "Voices");
    
    //==============================================================================
    // Setup Master Controls
    setupGroupComponent(masterGroup, "Master");
    addAndMakeVisible(masterGroup);
    
    inputGainKnob.setRange(-24.0, 24.0, 0.1);
    inputGainKnob.setValue(0.0);
    inputGainKnob.setTextValueSuffix(" dB");
    addAndMakeVisible(inputGainKnob);
    setupLabel(inputGainLabel, "Input Gain");
    
    outputGainKnob.setRange(-24.0, 24.0, 0.1);
    outputGainKnob.setValue(0.0);
    outputGainKnob.setTextValueSuffix(" dB");
    addAndMakeVisible(outputGainKnob);
    setupLabel(outputGainLabel, "Output Gain");
    
    setupButton(bypassButton);
    bypassButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(bypassButton);
    setupLabel(bypassLabel, "Bypass");
    
    //==============================================================================
    // Preset Management removed - using DAW preset dropdown
    
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
        audioProcessor.getValueTreeState(), "pitchCorrection", pitchCorrectionKnob);
    
    pitchSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "pitchSpeed", pitchSpeedKnob);
    
    pitchCorrectionEnabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "pitchCorrectionEnabled", pitchCorrectionEnabledButton);
    
    reverbAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "reverbAmount", reverbAmountKnob);
    
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "delayTime", delayTimeKnob);
    
    delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "delayFeedback", delayFeedbackKnob);
    
    harmonyAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "harmonyAmount", harmonyAmountKnob);
    
    harmonyVoicesAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "harmonyVoices", harmonyVoicesKnob);
    
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "inputGain", inputGainKnob);
    
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "outputGain", outputGainKnob);
    
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "bypass", bypassButton);
    
    //==============================================================================
    // Initialize Spectrum Analyzer
    spectrumAnalyzer = std::make_unique<SpectrumAnalyzer>();
    addAndMakeVisible(spectrumAnalyzer.get());
    
    // Initialize Advanced Visual Components
    createAdvancedVisualComponents();
    
    //==============================================================================
    // Add listeners
    pitchCorrectionKnob.addListener(this);
    pitchSpeedKnob.addListener(this);
    pitchCorrectionEnabledButton.addListener(this);
    reverbAmountKnob.addListener(this);
    delayTimeKnob.addListener(this);
    delayFeedbackKnob.addListener(this);
    harmonyAmountKnob.addListener(this);
    harmonyVoicesKnob.addListener(this);
    inputGainKnob.addListener(this);
    outputGainKnob.addListener(this);
    bypassButton.addListener(this);
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
    pitchCorrectionKnob.setBounds(aiPitchTop.removeFromLeft(120));
    aiPitchTop.removeFromLeft(CONTROL_SPACING);
    pitchSpeedLabel.setBounds(aiPitchTop.removeFromLeft(80));
    pitchSpeedKnob.setBounds(aiPitchTop);
    
    pitchCorrectionEnabledLabel.setBounds(aiPitchBottom.removeFromLeft(80));
    pitchCorrectionEnabledButton.setBounds(aiPitchBottom.removeFromLeft(50));
    
    leftColumn.removeFromTop(GROUP_SPACING);
    
    auto vocalEffectsArea = leftColumn.removeFromTop(250);
    vocalEffectsGroup.setBounds(vocalEffectsArea);
    
    auto vocalEffectsContent = vocalEffectsArea.reduced(15, 25);
    
    // Reverb
    auto reverbArea = vocalEffectsContent.removeFromTop(50);
    reverbAmountLabel.setBounds(reverbArea.removeFromLeft(80));
    reverbAmountKnob.setBounds(reverbArea);
    
    // Delay
    auto delayArea = vocalEffectsContent.removeFromTop(50);
    delayTimeLabel.setBounds(delayArea.removeFromLeft(80));
    delayTimeKnob.setBounds(delayArea.removeFromLeft(100));
    delayArea.removeFromLeft(CONTROL_SPACING);
    delayFeedbackLabel.setBounds(delayArea.removeFromLeft(80));
    delayFeedbackKnob.setBounds(delayArea);
    
    // Harmony
    auto harmonyArea = vocalEffectsContent.removeFromTop(50);
    harmonyAmountLabel.setBounds(harmonyArea.removeFromLeft(80));
    harmonyAmountKnob.setBounds(harmonyArea.removeFromLeft(100));
    harmonyArea.removeFromLeft(CONTROL_SPACING);
    harmonyVoicesLabel.setBounds(harmonyArea.removeFromLeft(80));
    harmonyVoicesKnob.setBounds(harmonyArea);
    
    // Right column
    auto masterArea = rightColumn.removeFromTop(150);
    masterGroup.setBounds(masterArea);
    
    auto masterContent = masterArea.reduced(15, 25);
    
    // Master controls
    auto inputGainArea = masterContent.removeFromTop(40);
    inputGainLabel.setBounds(inputGainArea.removeFromLeft(80));
    inputGainKnob.setBounds(inputGainArea.removeFromLeft(120));
    inputGainArea.removeFromLeft(CONTROL_SPACING);
    outputGainLabel.setBounds(inputGainArea.removeFromLeft(80));
    outputGainKnob.setBounds(inputGainArea);
    
    auto bypassArea = masterContent.removeFromTop(40);
    bypassLabel.setBounds(bypassArea.removeFromLeft(80));
    bypassButton.setBounds(bypassArea.removeFromLeft(50));
    
    rightColumn.removeFromTop(GROUP_SPACING);
    
    // Preset area removed - using DAW preset dropdown
    auto visualArea = rightColumn.removeFromTop(200);
    visualGroup.setBounds(visualArea);
    
    auto visualContent = visualArea.reduced(15, 25);
    
    // Visual feedback
    currentPitchLabel.setBounds(visualContent.removeFromTop(25));
    pitchConfidenceLabel.setBounds(visualContent.removeFromTop(25));
    statusLabel.setBounds(visualContent.removeFromTop(25));
    
    // Spectrum analyzer
    if (spectrumAnalyzer)
        spectrumAnalyzer->setBounds(visualContent);
    
    // Advanced visual components
    if (pitchMeter)
        pitchMeter->setBounds(visualContent.removeFromTop(30));
    if (confidenceMeter)
        confidenceMeter->setBounds(visualContent.removeFromTop(30));
    if (vocalDetector)
        vocalDetector->setBounds(visualContent.removeFromTop(30));
    if (harmonicityDisplay)
        harmonicityDisplay->setBounds(visualContent.removeFromTop(30));
}

//==============================================================================
// Advanced Visual Components
void VocalAIProEditor::createAdvancedVisualComponents()
{
    // Create Pitch Meter
    pitchMeter = std::make_unique<juce::Component>();
    pitchMeter->setName("Pitch Meter");
    addAndMakeVisible(pitchMeter.get());
    
    // Create Confidence Meter
    confidenceMeter = std::make_unique<juce::Component>();
    confidenceMeter->setName("Confidence Meter");
    addAndMakeVisible(confidenceMeter.get());
    
    // Create Vocal Detector
    vocalDetector = std::make_unique<juce::Component>();
    vocalDetector->setName("Vocal Detector");
    addAndMakeVisible(vocalDetector.get());
    
    // Create Harmonicity Display
    harmonicityDisplay = std::make_unique<juce::Component>();
    harmonicityDisplay->setName("Harmonicity Display");
    addAndMakeVisible(harmonicityDisplay.get());
}

void VocalAIProEditor::timerCallback()
{
    updateVisualFeedback();
}

//==============================================================================
void VocalAIProEditor::buttonClicked(juce::Button* button)
{
    // Preset buttons removed - using DAW preset system
    juce::ignoreUnused(button);
}

void VocalAIProEditor::sliderValueChanged(juce::Slider* slider)
{
    juce::ignoreUnused(slider);
    // Parameter changes are handled by attachments
}

void VocalAIProEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Preset combo box removed - using DAW preset system
    juce::ignoreUnused(comboBox);
}

//==============================================================================
void VocalAIProEditor::setupSlider(juce::Slider& slider, const juce::String& suffix)
{
    // Professional knob style
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    // Professional knob colors - inspired by FabFilter, Waves, etc.
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a90e2));      // Blue fill
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff34495e));   // Dark outline
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe74c3c));                 // Red thumb
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffecf0f1));           // Light text
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff2c3e50));     // Dark background
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff7f8c8d));        // Gray outline
    slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4a90e2));                 // Blue track
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff2c3e50));            // Dark background
    
    // Enhanced knob appearance
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4a90e2));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff34495e));
    
    // Professional knob settings
    slider.setMouseDragSensitivity(200);  // Smooth dragging
    slider.setScrollWheelEnabled(true);   // Mouse wheel support
    slider.setDoubleClickReturnValue(true, 0.0); // Double-click to reset
    
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
// Preset functions removed - using DAW preset system

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
    
    // Update spectrum analyzer
    if (spectrumAnalyzer)
    {
        const auto& magnitudes = audioProcessor.getSpectrumMagnitudes();
        if (!magnitudes.empty())
        {
            spectrumAnalyzer->updateSpectrum(magnitudes.data(), static_cast<int>(magnitudes.size()));
        }
    }
}

// updatePresetComboBox removed - using DAW preset system

//==============================================================================
// CustomKnob Implementation
VocalAIProEditor::CustomKnob::CustomKnob()
{
    setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    setMouseDragSensitivity(200);
    setScrollWheelEnabled(true);
    setDoubleClickReturnValue(true, 0.0);
}

void VocalAIProEditor::CustomKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.4f;
    
    // Draw knob background (outer ring)
    g.setColour(juce::Colour(0xff2c3e50));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
    
    // Draw knob outline
    g.setColour(juce::Colour(0xff7f8c8d));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2, 2.0f);
    
    // Draw knob fill (progress arc)
    auto angle = getAngleFromValue(getValue());
    auto startAngle = minAngle;
    auto endAngle = angle;
    
    // Ensure proper arc drawing
    if (endAngle > startAngle)
    {
        juce::Path arc;
        arc.addCentredArc(centre.x, centre.y, radius * 0.8f, radius * 0.8f, 0.0f, startAngle, endAngle, true);
        
        g.setColour(juce::Colour(0xff4a90e2));
        g.strokePath(arc, juce::PathStrokeType(8.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    // Draw knob pointer
    auto pointerLength = radius * 0.7f;
    auto pointerThickness = 3.0f;
    
    juce::Path pointer;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, -pointerLength);
    
    g.setColour(juce::Colour(0xffe74c3c));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    
    // Draw center dot
    g.setColour(juce::Colour(0xff34495e));
    g.fillEllipse(centre.x - 4, centre.y - 4, 8, 8);
}

void VocalAIProEditor::CustomKnob::mouseDown(const juce::MouseEvent& e)
{
    isDragging = true;
    juce::Slider::mouseDown(e);
}

void VocalAIProEditor::CustomKnob::mouseDrag(const juce::MouseEvent& e)
{
    if (isDragging)
    {
        auto centre = getLocalBounds().getCentre().toFloat();
        auto angle = std::atan2(e.position.y - centre.y, e.position.x - centre.x);
        
        // Convert angle to value with bounds checking
        auto normalizedAngle = (angle - minAngle) / (maxAngle - minAngle);
        normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
        auto value = getMinimum() + normalizedAngle * (getMaximum() - getMinimum());
        
        // Clamp value to valid range
        value = juce::jlimit(getMinimum(), getMaximum(), value);
        
        setValue(value, juce::sendNotificationSync);
    }
}

void VocalAIProEditor::CustomKnob::mouseUp(const juce::MouseEvent& e)
{
    isDragging = false;
    juce::Slider::mouseUp(e);
}

void VocalAIProEditor::CustomKnob::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    // More controlled wheel sensitivity
    auto delta = wheel.deltaY * 0.05f; // Reduced sensitivity
    auto currentValue = getValue();
    auto newValue = currentValue + delta * (getMaximum() - getMinimum());
    
    // Clamp to valid range
    newValue = juce::jlimit(getMinimum(), getMaximum(), newValue);
    
    setValue(newValue, juce::sendNotificationSync);
}

float VocalAIProEditor::CustomKnob::getAngleFromValue(float value)
{
    // Clamp value to valid range
    value = juce::jlimit(getMinimum(), getMaximum(), value);
    
    auto normalizedValue = (value - getMinimum()) / (getMaximum() - getMinimum());
    return minAngle + normalizedValue * (maxAngle - minAngle);
}

float VocalAIProEditor::CustomKnob::getValueFromAngle(float angle)
{
    // Normalize angle to [0, 1] range
    auto normalizedAngle = (angle - minAngle) / (maxAngle - minAngle);
    normalizedAngle = juce::jlimit(0.0f, 1.0f, normalizedAngle);
    
    return getMinimum() + normalizedAngle * (getMaximum() - getMinimum());
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
        
        // Apply smoothing with different factors for different frequency ranges
        float smoothingFactor = 0.8f;
        if (i < numBins / 4) // Low frequencies - more smoothing
            smoothingFactor = 0.9f;
        else if (i > numBins * 3 / 4) // High frequencies - less smoothing
            smoothingFactor = 0.7f;
            
        smoothedSpectrum[i] = smoothedSpectrum[i] * smoothingFactor + spectrumData[i] * (1.0f - smoothingFactor);
    }
}
