#pragma once
#include <array>
namespace controls {
constexpr int modelCount=9;
constexpr std::array<const char*,modelCount> models{"PearLiteEQ","BezEQ3","FatEQ","SmoothEQ3","SmoothEQ2","PearEQ","EverySlew","Slew4","Air4"};
constexpr std::array<const char*,8> names{"High","High mid","Low mid","Bass","High frequency","High mid frequency","Low mid frequency","Bass frequency"};
inline juce::String modelName(int model) { return models[static_cast<size_t>(model)]; }
inline int count(int model) { constexpr std::array<int,modelCount> counts{4,8,5,3,8,6,4,1,4}; return counts[static_cast<size_t>(model)]; }
inline juce::String prefix(int model) { constexpr std::array<const char*,modelCount> prefixes{"pear_","bez_","fat_","smooth3_","smooth2_","peareq_","everyslew_","slew4_","air4_"}; return prefixes[static_cast<size_t>(model)]; }
inline juce::String id(int model,int index) { return prefix(model)+juce::String(index); }
inline juce::String matchId(int model) { return prefix(model)+"matchEnabled"; }
inline juce::String originalId(int model) { return prefix(model)+"original"; }
inline juce::String inputId(int model) { return prefix(model)+"inputTrim"; }
inline juce::String outputId(int model) { return prefix(model)+"outputTrim"; }
inline juce::String linkId(int model) { return prefix(model)+"linkIO"; }
inline bool hasFrequencies(int model) { return model==1 || model==4; }
inline int primaryCount(int model) { return hasFrequencies(model)?4:count(model); }
inline int primaryIndex(int model,int column) { return model>=6?column:model==2?(column==4?4:3-column):primaryCount(model)-1-column; }
inline float defaultValue(int model,int index) { if(model==8) { constexpr std::array<float,4> defaults{.5f,.5f,.52f,0.f}; return defaults[static_cast<size_t>(index)]; } if(model==6) { constexpr std::array<float,4> defaults{0.f,1.f,.16f,1.f}; return defaults[static_cast<size_t>(index)]; } return model==2 && index==4?1.f:.5f; }
inline int latency(int model) { return model==2?1:model==7?2:0; }
inline juce::StringArray choices() { juce::StringArray result; for(auto* name:models) result.add(name); return result; }
inline const char* name(int model,int index) {
    if(model==6) { constexpr std::array<const char*,4> n{"Slew","Depth","Halo","Inv/Wet"}; return n[static_cast<size_t>(index)]; }
    if(model==7) return "Slew";
    if(model==8) { constexpr std::array<const char*,4> n{"Air","Gnd","DarkF","Ratio"}; return n[static_cast<size_t>(index)]; }
    if(model==2 && index==4) return "EQ output";
    if(model==3) { constexpr std::array<const char*,3> n{"High","Mid","Bass"}; return n[static_cast<size_t>(index)]; }
    if(model==5) { constexpr std::array<const char*,6> n{"High","High mid","Mid","Low mid","Bass","Sub"}; return n[static_cast<size_t>(index)]; }
    return names[static_cast<size_t>(index)];
}
inline const char* description(int model) {
    constexpr std::array<const char*,modelCount> text{
        "PEAR FILTER  /  FOUR-BAND EQUALIZER", "BEZIER  /  FOUR-BAND EQUALIZER",
        "FAT EQ  /  ORIGINAL CLIPONLY2 OUTPUT", "SMOOTH EQ 3  /  THREE-BAND EQUALIZER",
        "SMOOTH EQ 2  /  FOUR-BAND EQUALIZER", "PEAR FILTER  /  SIX-BAND EQUALIZER",
        "EVERYSLEW  /  MULTISTAGE SLEW LIMITER", "SLEW4  /  ADAPTIVE SLEW PROCESSOR", "AIR4  /  AIR + GROUND WITH SINEW"};
    return text[static_cast<size_t>(model)];
}
inline juce::String tip(int model,int index) {
    if(model==6) {
        constexpr std::array<const char*,4> tips{
            "Original slew threshold mapping. Default 0%.",
            "Original stepped stage-depth mapping. Default 100%.",
            "Original halo contour. Default 16%.",
            "Original inverse/dry/wet blend: 0% subtracts the processed signal, 50% is dry, 100% is wet."};
        return tips[static_cast<size_t>(index)];
    }
    if(model==8) {
        constexpr std::array<const char*,4> tips{
            "Original Air gain mapping. Default 50%.",
            "Original ground gain mapping. Default 50%.",
            "Original Sinew threshold mapping. Default 52%; normalized control, not Hz.",
            "Original Sinew depth mapping. Default 0%; normalized control, not a numerical ratio."};
        return tips[static_cast<size_t>(index)];
    }
    if(model==7) return "Original slew amount, with adaptive averaging and two-sample lookahead. Default 50%; 0% removes averaging but retains lookahead.";
    if(model==2 && index==4) return "Original FatEQ output gain before ClipOnly2. Defaults to 100%. Separate from wrapper Output trim.";
    return !hasFrequencies(model) || index<4?"Original Airwindows band level. 50% is neutral.":
        "Original normalized frequency control. The crossover mapping depends on other controls and sample rate; this is not a Hz readout.";
}
inline juce::String display(int,double v) { return juce::String(v*100.,1)+" %"; }
inline double parse(int,const juce::String& text) { return juce::jlimit(0.,1.,text.getDoubleValue()/100.); }
}
