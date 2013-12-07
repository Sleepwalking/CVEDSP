TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# QMAKE_CFLAGS += -O3 -msse -msse2 -mavx -ffast-math -lm

QMAKE_CFLAGS += -O3 -msse -ffast-math -lm

SOURCES += \
    Tester.c \
    Plot.c \
    Interpolation.c \
    FrameProcessor.c \
    DFT/StaticRev.c \
    DFT/StaticFFT_Small.c \
    DFT/StaticFFT_9.c \
    DFT/StaticFFT_8.c \
    DFT/StaticFFT_7.c \
    DFT/StaticFFT_6.c \
    DFT/StaticFFT_5.c \
    DFT/SplitRadixGen.c \
    DFT/SplitRadix.c \
    DFT/Radix2Gen.c \
    DFT/Radix2.c \
    DFT/FFT.c \
    DFT/DFT.c \
    IntrinUtil/Memory.c \
    IntrinUtil/LogicArray.c \
    IntrinUtil/IntArray.c \
    IntrinUtil/FloatArray.c \
    IntrinUtil/Calculation.c \
    RUtil/Memory.c \
    RUtil/IO/Wave.c \
    RUtil/IO/StringStream.c \
    RUtil/IO/FileUtil.c \
    RUtil/IO/FileStream.c \
    RUtil/Misc/Converter.c \
    RUtil/Structure/String.c \
    DSPBase/Spectrum.c \
    DSPBase/Filter.c \
    DSPBase/ControlPointFilter.c \
    DSPBase/Window.c \
    DSPBase/TDEffects.c \
    DSPBase/TDAnalysis.c \
    DSPBase/LinearWarping.c \
    DSPBase/LPC.c \
    Algorithm/FECSOLA.c \
    Algorithm/PSOLA.c \
    Algorithm/Formant.c \
    Algorithm/FECSOLA.c \
    Algorithm/BaseFreq.c \
    Algorithm/SpeechDetector.c \
    DFT/StaticRadix.c \
    RUtil/Misc/Math.c

HEADERS += \
    Tester.h \
    Plot.h \
    Interpolation.h \
    FrameProcessor.h \
    DFT/StaticRev.h \
    DFT/StaticFFT_Small.h \
    DFT/StaticFFT_9.h \
    DFT/StaticFFT_8.h \
    DFT/StaticFFT_7.h \
    DFT/StaticFFT_6.h \
    DFT/StaticFFT_5.h \
    DFT/SplitRadixGen.h \
    DFT/SplitRadix.h \
    DFT/Radix2Gen.h \
    DFT/Radix2.h \
    DFT/FFTCommon.h \
    DFT/FFTBlock.h \
    DFT/FFT.h \
    DFT/DFT.h \
    IntrinUtil/Memory.h \
    IntrinUtil/LogicArray.h \
    IntrinUtil/IntArray.h \
    IntrinUtil/FloatArray.h \
    IntrinUtil/Calculation.h \
    RUtil/RUtil.h \
    RUtil/Memory.h \
    RUtil/IO/Wave.h \
    RUtil/IO/StringStream.h \
    RUtil/IO/FileUtil.h \
    RUtil/IO/FileStream.h \
    RUtil/Misc/Converter.h \
    RUtil/Structure/String.h \
    RUtil/Structure/Array.h \
    DSPBase/Spectrum.h \
    DSPBase/Filter.h \
    DSPBase/ControlPointFilter.h \
    DSPBase/LinearWarping.h \
    DSPBase/LPC.h \
    DSPBase/Window.h \
    DSPBase/TDEffects.h \
    DSPBase/TDAnalysis.h \
    Algorithm/FECSOLA.h \
    Algorithm/PSOLA.h \
    Algorithm/Formant.h \
    Algorithm/BaseFreq.h \
    Algorithm/SpeechDetector.h \
    DFT/StaticRadix.h \
    RUtil/Misc/Math.h

