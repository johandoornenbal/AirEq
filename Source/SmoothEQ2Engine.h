#pragma once
// Adapted from Airwindows SmoothEQ2 (MIT). Processing bodies are unchanged.
#include <cstdint>
#include <cmath>
#include <algorithm>
class SmoothEQ2Engine {
public:
    SmoothEQ2Engine() { reset(44100.); }
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
    float G=.5f;
    float H=.5f;
	enum {
		biq_freq,
		biq_reso,
		biq_a0,
		biq_a1,
		biq_a2,
		biq_b1,
		biq_b2,
		biq_sL1,
		biq_sL2,
		biq_sR1,
		biq_sR2,
		biq_total
	}; //coefficient interpolating bessel filter, stereo
	double highA[biq_total];
	double highB[biq_total];
	double highC[biq_total];
	double highLIIR;
	double highRIIR;
	
	double midA[biq_total];
	double midB[biq_total];
	double midC[biq_total];
	double midLIIR;
	double midRIIR;
	
	double lowA[biq_total];
	double lowB[biq_total];
	double lowC[biq_total];
	double lowLIIR;
	double lowRIIR;
	
	uint32_t fpdL;
	uint32_t fpdR;

};
