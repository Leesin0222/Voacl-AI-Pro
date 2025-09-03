#!/bin/bash

echo "Building VocalAI Pro Plugin..."

# JUCE 경로 설정 (사용자 환경에 맞게 수정)
JUCE_PATH="/usr/local/JUCE"
if [ ! -d "$JUCE_PATH" ]; then
    echo "ERROR: JUCE not found at $JUCE_PATH"
    echo "Please download JUCE from https://juce.com and set the correct path"
    exit 1
fi

# Projucer 실행하여 프로젝트 생성
echo "Generating project files..."
"$JUCE_PATH/Projucer" --resave VocalAIPro.jucer

# Makefile 빌드
echo "Building with Makefile..."
cd Builds/LinuxMakefile
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Plugin files created in Builds/LinuxMakefile/build/"
else
    echo "Build failed!"
    exit 1
fi

cd ../..
echo "Build complete!"
