#pragma once
#include <array>
namespace controls {
constexpr int modelCount=2;
constexpr std::array<const char*,modelCount> models{"PearLiteEQ","BezEQ3"};
constexpr std::array<const char*,8> names{"High","High mid","Low mid","Bass","High frequency","High mid frequency","Low mid frequency","Bass frequency"};
inline juce::String modelName(int model) { return models[static_cast<size_t>(model)]; }
inline int count(int model) { return model==0?4:8; }
inline juce::String prefix(int model) { return model==0?"pear_":"bez_"; }
inline juce::String id(int model,int index) { return prefix(model)+juce::String(index); }
inline juce::String matchId(int model) { return prefix(model)+"matchEnabled"; }
inline juce::String originalId(int model) { return prefix(model)+"original"; }
inline juce::String inputId(int model) { return prefix(model)+"inputTrim"; }
inline juce::String outputId(int model) { return prefix(model)+"outputTrim"; }
inline juce::String linkId(int model) { return prefix(model)+"linkIO"; }
inline const char* name(int,int index) { return names[static_cast<size_t>(index)]; }
inline juce::String tip(int,int index) {
    return index<4?"Original Airwindows band level. 50% is neutral.":
        "Original normalized frequency control. The crossover mapping depends on other controls and sample rate; this is not a Hz readout.";
}
inline juce::String display(int,double v) { return juce::String(v*100.,1)+" %"; }
inline double parse(int,const juce::String& text) { return juce::jlimit(0.,1.,text.getDoubleValue()/100.); }
}
