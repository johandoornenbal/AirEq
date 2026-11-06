#pragma once
// Adapted from Airwindows Slew4 (MIT). Processing bodies are unchanged.
#include <cstdint>
#include <cmath>
#include <algorithm>
class Slew4Engine {
public:
    Slew4Engine() { reset(44100.); }
    void reset(double sampleRate);
    void setParameter(int index,float value);
    void processReplacing(float** inputs,float** outputs,int sampleFrames);
    void processDoubleReplacing(double** inputs,double** outputs,int sampleFrames);
    double getSampleRate() const { return rate; }
private:
    double rate=44100.;
    float A=.5f;
	double slewL[24][12];
	double sustainSlewL;
	double secondSampleL;
	double thirdSampleL;

	double slewR[24][12];
	double sustainSlewR;
	double secondSampleR;
	double thirdSampleR;
	
	int slewCount;

	uint32_t fpdL;
	uint32_t fpdR;

};
