#pragma once
// Airwindows BiquadStack (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class BiquadStackEngine {
public:
 BiquadStackEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 using VstInt32=int;
 double rate=44100.;

    
    float A;
    float B;
    float C;
	
	enum {
		biqs_freq, biqs_reso, biqs_level, biqs_levelA, biqs_levelB, biqs_nonlin, biqs_temp, biqs_dis,
		biqs_a0, biqs_aA0, biqs_aB0, biqs_b1, biqs_bA1, biqs_bB1, biqs_b2, biqs_bA2, biqs_bB2,
		biqs_c0, biqs_cA0, biqs_cB0, biqs_d1, biqs_dA1, biqs_dB1, biqs_d2, biqs_dA2, biqs_dB2,
		biqs_e0, biqs_eA0, biqs_eB0, biqs_f1, biqs_fA1, biqs_fB1, biqs_f2, biqs_fA2, biqs_fB2,
		biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
		biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
		biqs_eL1, biqs_eL2, biqs_eR1, biqs_eR2,
		biqs_outL, biqs_outR, biqs_total
	};
	double biqs[biqs_total];
	
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
};
