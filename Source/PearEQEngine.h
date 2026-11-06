#pragma once
// Adapted from Airwindows PearEQ (MIT). Processing bodies are unchanged.
#include <cstdint>
#include <cmath>
#include <algorithm>
class PearEQEngine {
public:
    PearEQEngine() { reset(44100.); }
    void reset(double sampleRate);
    void setParameter(int index,float value);
    void processReplacing(float** inputs,float** outputs,int sampleFrames);
    void processDoubleReplacing(double** inputs,double** outputs,int sampleFrames);
    double getSampleRate() const { return rate; }
private:
    double rate=44100.;
    float A=.5f;
    float B=.5f;
    float C=.5f;
    float D=.5f;
    float E=.5f;
    float F=.5f;
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		pear_max,
		figL,
		figR,
		gndL,
		gndR,
		slew,
		freq,
		levl,
		pear_total
	}; //new pear filter
	
	double pearA[pear_total];
	double pearB[pear_total];
	double pearC[pear_total];
	double pearD[pear_total];
	double pearE[pear_total];
		
	uint32_t fpdL;
	uint32_t fpdR;

};
