#pragma once
#include <array>
namespace controls {
constexpr int modelCount=6;
constexpr std::array<const char*,modelCount> models{"PearLiteEQ","BezEQ3","FatEQ","SmoothEQ3","SmoothEQ2","PearEQ"};
constexpr std::array<const char*,8> names{"High","High mid","Low mid","Bass","High frequency","High mid frequency","Low mid frequency","Bass frequency"};
inline juce::String modelName(int model) { return models[static_cast<size_t>(model)]; }
inline int count(int model) { constexpr std::array<int,modelCount> counts{4,8,5,3,8,6}; return counts[static_cast<size_t>(model)]; }
inline juce::String prefix(int model) { constexpr std::array<const char*,modelCount> prefixes{"pear_","bez_","fat_","smooth3_","smooth2_","peareq_"}; return prefixes[static_cast<size_t>(model)]; }
inline juce::String id(int model,int index) { return prefix(model)+juce::String(index); }
inline juce::String matchId(int model) { return prefix(model)+"matchEnabled"; }
inline juce::String originalId(int model) { return prefix(model)+"original"; }
inline juce::String inputId(int model) { return prefix(model)+"inputTrim"; }
inline juce::String outputId(int model) { return prefix(model)+"outputTrim"; }
inline juce::String linkId(int model) { return prefix(model)+"linkIO"; }
inline bool hasFrequencies(int model) { return model==1 || model==4; }
inline int primaryCount(int model) { return hasFrequencies(model)?4:count(model); }
inline int primaryIndex(int model,int column) { return model==2?(column==4?4:3-column):primaryCount(model)-1-column; }
inline float defaultValue(int model,int index) { return model==2 && index==4?1.f:.5f; }
inline int latency(int model) { return model==2?1:0; }
inline juce::StringArray choices() { juce::StringArray result; for(auto* name:models) result.add(name); return result; }
inline const char* name(int model,int index) {
    if(model==2 && index==4) return "EQ output";
    if(model==3) { constexpr std::array<const char*,3> n{"High","Mid","Bass"}; return n[static_cast<size_t>(index)]; }
    if(model==5) { constexpr std::array<const char*,6> n{"High","High mid","Mid","Low mid","Bass","Sub"}; return n[static_cast<size_t>(index)]; }
    return names[static_cast<size_t>(index)];
}
inline const char* description(int model) {
    constexpr std::array<const char*,modelCount> text{
        "PEAR FILTER  /  FOUR-BAND EQUALIZER", "BEZIER  /  FOUR-BAND EQUALIZER",
        "FAT EQ  /  ORIGINAL CLIPONLY2 OUTPUT", "SMOOTH EQ 3  /  THREE-BAND EQUALIZER",
        "SMOOTH EQ 2  /  FOUR-BAND EQUALIZER", "PEAR FILTER  /  SIX-BAND EQUALIZER"};
    return text[static_cast<size_t>(model)];
}
inline juce::String tip(int model,int index) {
    if(model==2 && index==4) return "Original FatEQ output gain before ClipOnly2. Defaults to 100%. Separate from wrapper Output trim.";
    return !hasFrequencies(model) || index<4?"Original Airwindows band level. 50% is neutral.":
        "Original normalized frequency control. The crossover mapping depends on other controls and sample rate; this is not a Hz readout.";
}
inline juce::String display(int,double v) { return juce::String(v*100.,1)+" %"; }
inline double parse(int,const juce::String& text) { return juce::jlimit(0.,1.,text.getDoubleValue()/100.); }
}
