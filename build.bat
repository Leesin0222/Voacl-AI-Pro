@echo off
echo Building VocalAI Pro Plugin...

REM JUCE 경로 설정 (사용자 환경에 맞게 수정)
REM Windows에서 JUCE를 다운로드한 경로로 수정하세요
set JUCE_PATH=C:\JUCE
if not exist "%JUCE_PATH%" (
    echo ERROR: JUCE not found at %JUCE_PATH%
    echo Please download JUCE from https://juce.com and set the correct path
    pause
    exit /b 1
)

REM Projucer 실행하여 프로젝트 생성
echo Generating project files...
"%JUCE_PATH%\Projucer.exe" --resave VocalAIPro.jucer

REM Visual Studio 빌드
echo Building with Visual Studio...
cd Builds\VisualStudio2022
msbuild VocalAIPro.sln /p:Configuration=Release /p:Platform=x64

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Plugin files created in Builds\VisualStudio2022\x64\Release\VST3\
) else (
    echo Build failed!
    exit /b 1
)

cd ..\..
echo Build complete!
pause
