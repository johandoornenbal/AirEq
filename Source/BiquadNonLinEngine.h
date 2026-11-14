#pragma once
// Airwindows BiquadNonLin (MIT). Processing bodies unchanged.
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <algorithm>
class BiquadNonLinEngine {
public:
 BiquadNonLinEngine() { reset(44100.); }
 void reset(double sampleRate);
 void setParameter(int index,float value);
 void processReplacing(float**,float**,int);
 void processDoubleReplacing(double**,double**,int);
 double getSampleRate() const { return rate; }
private:
 using VstInt32=int;
 double rate=44100.;

    
	uint32_t fpdL;
	uint32_t fpdR;
	//default stuff
	
	enum {
		biq_freq,
		biq_reso,
		biq_a0,
		biq_a1,
		biq_a2,
		biq_b1,
		biq_b2,
		biq_aA0,
		biq_aA1,
		biq_aA2,
		biq_bA1,
		biq_bA2,
		biq_aB0,
		biq_aB1,
		biq_aB2,
		biq_bB1,
		biq_bB2,
		biq_sL1,
		biq_sL2,
		biq_sR1,
		biq_sR2,
		biq_total
	};
	double biquad[biq_total];	

    float A;
    float B;
    float C;
    float D;
    float E; //parameters. Always 0-1, and we scale/alter them elsewhere.

};
