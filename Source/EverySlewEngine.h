#pragma once
// Adapted from Airwindows EverySlew (MIT). Processing bodies are unchanged.
#include <cstdint>
#include <cmath>
#include <algorithm>
class EverySlewEngine {
public:
    EverySlewEngine() { reset(44100.); }
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
	enum {
		prevSampL1,
		prevSampR1,
		invSampL1,
		invSampR1,
		threshold1,
		prevSampL2,
		prevSampR2,
		invSampL2,
		invSampR2,
		threshold2,
		prevSampL3,
		prevSampR3,
		invSampL3,
		invSampR3,
		threshold3,
		prevSampL4,
		prevSampR4,
		invSampL4,
		invSampR4,
		threshold4,
		prevSampL5,
		prevSampR5,
		invSampL5,
		invSampR5,
		threshold5,
		prevSampL6,
		prevSampR6,
		invSampL6,
		invSampR6,
		threshold6,
		prevSampL7,
		prevSampR7,
		invSampL7,
		invSampR7,
		threshold7,
		prevSampL8,
		prevSampR8,
		invSampL8,
		invSampR8,
		threshold8,
		prevSampL9,
		prevSampR9,
		invSampL9,
		invSampR9,
		threshold9,
		prevSampL10,
		prevSampR10,
		invSampL10,
		invSampR10,
		threshold10,
		gslew_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	double gslew[gslew_total]; //probably worth just using a number here
	
	uint32_t fpdL;
	uint32_t fpdR;

};
