# VocalAI Pro - AI-Powered Vocal Effects Suite

## 개요

VocalAI Pro는 JUCE 프레임워크로 개발된 전문적인 AI 기반 보컬 이펙트 플러그인입니다. 실시간 피치 튜닝, 보컬 이펙트, 하모니 생성 등의 고급 기능을 제공합니다.

## 주요 기능
### AI 피치 튜닝
- **실시간 피치 감지**: 자동상관관계 기반 정밀한 피치 분석
- **자연스러운 피치 보정**: AI 알고리즘을 통한 자연스러운 보정
- **스케일 정량화**: 크로마틱, 메이저, 마이너 등 다양한 스케일 지원
- **낮은 레이턴시**: 실시간 라이브 공연에서도 사용 가능

### 보컬 이펙트
- **리버브**: 다양한 알고리즘의 고품질 리버브
- **딜레이**: 피드백과 필터링이 포함된 딜레이
- **하모니 생성**: 최대 8개의 하모니 보이스 생성
- **보컬 더블링**: 자연스러운 보컬 더블링 효과
- **포먼트 시프팅**: 보컬 톤 변경

### 마스터 컨트롤
- **입력/출력 게인**: 정밀한 레벨 조절
- **바이패스**: 실시간 온/오프
- **프리셋 시스템**: 다양한 사전 설정된 프리셋

## 기술 사양

### 지원 플러그인 포맷
- **VST3** - Windows, macOS, Linux
- **Audio Unit (AU)** - macOS
- **AAX** - Pro Tools
- **LV2** - Linux

### 시스템 요구사항
- **Windows**: Windows 10 이상, Visual Studio 2019 이상
- **macOS**: macOS 10.15 이상, Xcode 12 이상
- **Linux**: Ubuntu 18.04 이상, GCC 7 이상

### 오디오 사양
- **샘플 레이트**: 44.1kHz - 192kHz
- **비트 깊이**: 32-bit float
- **채널**: 모노/스테레오
- **레이턴시**: < 10ms (44.1kHz)

## 빌드 방법

### 1. JUCE 설치
1. [JUCE 공식 웹사이트](https://juce.com)에서 JUCE 다운로드
2. 압축 해제 후 적절한 경로에 설치
3. `build.bat` (Windows) 또는 `build.sh` (Linux/macOS)에서 JUCE 경로 수정

### 2. 빌드 실행
**Windows:**
```bash
build.bat
```

**Linux/macOS:**
```bash
chmod +x build.sh
./build.sh
```

### 3. 플러그인 설치
빌드된 플러그인 파일을 다음 경로에 복사:

**Windows:**
- VST3: `C:\Program Files\Common Files\VST3\`
- AAX: `C:\Program Files\Common Files\Avid\Audio\Plug-Ins\`

**macOS:**
- VST3: `/Library/Audio/Plug-Ins/VST3/`
- AU: `/Library/Audio/Plug-Ins/Components/`
- AAX: `/Library/Application Support/Avid/Audio/Plug-Ins/`

**Linux:**
- VST3: `/usr/local/lib/vst3/`
- LV2: `/usr/local/lib/lv2/`

## 사용법

### 기본 사용법
1. DAW에서 VocalAI Pro 플러그인을 로드
2. AI 피치 튜닝 섹션에서 보정 강도와 속도 조절
3. 보컬 이펙트 섹션에서 원하는 이펙트 활성화
4. 마스터 컨트롤에서 입출력 레벨 조절

### 프리셋 사용
- **Default**: 기본 설정
- **Vocal Doubling**: 보컬 더블링 효과
- **Harmony**: 하모니 생성
- **Reverb**: 리버브 중심
- **Delay**: 딜레이 중심

### 고급 설정
- **피치 보정**: 0-100% 범위에서 자연스러운 보정
- **피치 속도**: 보정 반응 속도 조절
- **하모니 보이스**: 1-8개 보이스 설정
- **딜레이 시간**: 0-2000ms 범위

## 타겟 시장

### 주요 사용자
- **음악 프로듀서**: 보컬 녹음 및 편집
- **보컬리스트**: 실시간 피치 보정
- **사운드 엔지니어**: 보컬 믹싱 및 마스터링
- **콘텐츠 크리에이터**: YouTube, TikTok 등

### 장르별 활용
- **팝**: 깔끔한 보컬 사운드
- **R&B**: 부드러운 보컬 이펙트
- **록**: 강렬한 보컬 더블링
- **일렉트로닉**: 크리에이티브 보컬 처리

## 개발 정보

### 아키텍처
- **AudioProcessor**: 핵심 오디오 처리 로직
- **AIPitchTuner**: AI 피치 튜닝 알고리즘
- **VocalEffects**: 보컬 이펙트 모듈
- **Editor**: 사용자 인터페이스

### 주요 클래스
```cpp
VocalAIProPlugin     // 메인 플러그인 클래스
AIPitchTuner         // AI 피치 튜닝
VocalEffects         // 보컬 이펙트
VocalAIProEditor     // GUI 에디터
```

### 알고리즘
- **피치 감지**: 자동상관관계 + YIN 알고리즘
- **피치 보정**: 실시간 피치 시프팅
- **하모니 생성**: 다중 보이스 피치 시프팅
- **리버브**: JUCE 내장 리버브 알고리즘

## 향후 개발 계획

### v1.1.0
- [ ] 더 정교한 AI 피치 감지
- [ ] 추가 보컬 이펙트
- [ ] MIDI 컨트롤 지원

### v1.2.0
- [ ] 클라우드 기반 AI 모델
- [ ] 실시간 스펙트럼 분석
- [ ] 커스텀 프리셋 저장

### v2.0.0
- [ ] 머신러닝 기반 보컬 분석
- [ ] 자동 믹싱 기능
- [ ] 다국어 지원



## 라이선스

이 프로젝트는 All Rights Reserved 라이선스 하에 배포됩니다. 자세한 내용은 `LICENSE` 파일을 참조하세요.

## 지원

- **이메일**: dragon02tv@gmail.com
- **웹사이트**: https://github.com/Leesin0222

## 감사의 말

- JUCE 프레임워크 팀 땡큐요요

---

**VocalAI Pro** - AI로 만드는 완벽한 보컬 사운드
