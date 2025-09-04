cmake_minimum_required(VERSION 3.15)

project(VocalAIPro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE 경로 설정 (사용자 환경에 맞게 수정)
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JUCE" CACHE PATH "JUCE directory")

# JUCE 모듈 찾기
if(NOT EXISTS "${JUCE_DIR}")
    message(FATAL_ERROR "JUCE directory not found at ${JUCE_DIR}. Please set JUCE_DIR to the correct path.")
endif()

# JUCE CMake 모듈 포함
include(${JUCE_DIR}/extras/Build/CMake/JUCE.cmake)

# 소스 파일들
set(SOURCES
    Source/VocalAIProPlugin.cpp
    Source/VocalAIProPlugin.h
    Source/VocalAIProEditor.cpp
    Source/VocalAIProEditor.h
    Source/AIPitchTuner.cpp
    Source/AIPitchTuner.h
    Source/VocalEffects.cpp
    Source/VocalEffects.h
)

# JUCE 플러그인 타겟 생성
juce_add_plugin(VocalAIPro
    COMPANY_NAME "LeeYongJin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE VcAI
    PLUGIN_CODE VcAI
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "VocalAI Pro"
)

# 소스 파일 추가
target_sources(VocalAIPro PRIVATE ${SOURCES})

# JUCE 모듈 링크
target_link_libraries(VocalAIPro
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# JUCE 플러그인 설정
juce_generate_juce_header(VocalAIPro)

# 컴파일러 특정 설정
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(VocalAIPro PRIVATE /W4)
else()
    target_compile_options(VocalAIPro PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 디버그/릴리즈 설정
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(VocalAIPro PRIVATE DEBUG=1 _DEBUG=1)
else()
    target_compile_definitions(VocalAIPro PRIVATE NDEBUG=1)
endif()

# 플랫폼별 설정
if(WIN32)
    target_compile_definitions(VocalAIPro PRIVATE
        JUCE_WINDOWS=1
        JUCE_PLUGINHOST_VST=1
        JUCE_PLUGINHOST_VST3=1
    )
elseif(APPLE)
    target_compile_definitions(VocalAIPro PRIVATE
        JUCE_MAC=1
        JUCE_PLUGINHOST_AU=1
        JUCE_PLUGINHOST_VST3=1
    )
elseif(UNIX)
    target_compile_definitions(VocalAIPro PRIVATE
        JUCE_LINUX=1
        JUCE_PLUGINHOST_VST3=1
        JUCE_PLUGINHOST_LV2=1
    )
endif()

# 설치 설정
install(TARGETS VocalAIPro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# 패키징 설정
set(CPACK_PACKAGE_NAME "VocalAI Pro")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "LeeYongJin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AI-Powered Vocal Effects Suite")
set(CPACK_PACKAGE_CONTACT "info@leeyongjin.com")

include(CPack)
