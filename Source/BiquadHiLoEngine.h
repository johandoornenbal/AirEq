#pragma once
// Airwindows BiquadHiLo (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class BiquadHiLoEngine {
public:
 BiquadHiLoEngine() { reset(44100.); }
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
	
	enum {
		hilp_freq, hilp_temp,
		hilp_a0, hilp_a1, hilp_b1, hilp_b2,
		hilp_c0, hilp_c1, hilp_d1, hilp_d2,
		hilp_e0, hilp_e1, hilp_f1, hilp_f2,
		hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
		hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
		hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
		hilp_total
	};
	double highpass[hilp_total];
	double lowpass[hilp_total];	

	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
};
