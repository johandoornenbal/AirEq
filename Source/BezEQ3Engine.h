#pragma once
// Adapted from Airwindows BezEQ3 (MIT). Processing bodies are unchanged.
#include <cstdint>
#include <cmath>
#include <algorithm>
class BezEQ3Engine {
public:
    BezEQ3Engine() { reset(44100.); }
    void reset(double sampleRate);
    void setParameter(int index,float value);
    void processReplacing(float** inputs,float** outputs,int sampleFrames);
    void processDoubleReplacing(double** inputs,double** outputs,int sampleFrames);
    double getSampleRate() const { return rate; }
private:
    double rate=44100.;
    float A=.5f,B=.5f,C=.5f,D=.5f,E=.5f,F=.5f,G=.5f,H=.5f;
	enum {
		bez_HMAL, bez_HMBL, bez_HMCL, bez_HMDL,
		bez_MAL, bez_MBL, bez_MCL, bez_MDL,
		bez_LMAL, bez_LMBL, bez_LMCL, bez_LMDL,
		bez_HMAR, bez_HMBR, bez_HMCR, bez_HMDR, bez_HMcycle,
		bez_MAR, bez_MBR, bez_MCR, bez_MDR, bez_Mcycle,
		bez_LMAR, bez_LMBR, bez_LMCR, bez_LMDR, bez_LMcycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	double bezEQ[bez_total][3];
	
	uint32_t fpdL;
	uint32_t fpdR;

};
