#include "EverySlewEngine.h"
void EverySlewEngine::reset(double sampleRate) {
    rate=sampleRate;
	A = 0.0;
	B = 1.0;
	C = 0.16;
	D = 1.0;
	for (int x = 0; x < gslew_total; x++) gslew[x] = 0.0;
	fpdL = 0x12345678u;
	fpdR = 0x87654321u;

}
void EverySlewEngine::setParameter(int index,float value) {
    value=std::clamp(value,0.f,1.f);
    switch(index) {
        case 0: A=value; break;
        case 1: B=value; break;
        case 2: C=value; break;
        case 3: D=value; break;
        default: break;
    }
}
void EverySlewEngine::processReplacing(float **inputs, float **outputs, int sampleFrames) 
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= getSampleRate();
	
	double source = pow(1-A,4)/overallscale;
	int stages = (1.0-B)*9.99;
	stages *= 5;
	double halo = C;
	double wet = (D*2.0)-1.0; //inv-dry-wet for highpass
	double dry = 2.0-(D*2.0);
	if (dry > 1.0) dry = 1.0; //full dry for use with inv, to 0.0 at full wet
	
	gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold1] = source;
	source *= 1.618033988749894848204586;
	
    while (--sampleFrames >= 0)
    {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		for (int x = stages; x < gslew_total; x += 5) {
			if (((inputSampleL-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleL = (gslew[x]-((gslew[x]-gslew[x+2])*halo)) + (gslew[x+4]*(1.0-halo));
			if (-((inputSampleL-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleL = (gslew[x]-((gslew[x]-gslew[x+2])*halo*0.78)) - (gslew[x+4]*(1.0-(halo*0.78)));
			gslew[x+2] = gslew[x]*(1.0-halo);
			gslew[x] = inputSampleL;
			
			if (((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*halo)) + (gslew[x+4]*(1.0-halo));
			if (-((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*halo*0.78)) - (gslew[x+4]*(1.0-(halo*0.78)));
			gslew[x+3] = gslew[x+1]*(1.0-halo);
			gslew[x+1] = inputSampleR;			
		}
		
		inputSampleL *= wet;
		inputSampleR *= wet;
		drySampleL *= dry;
		drySampleR *= dry;
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		
		//begin 32 bit stereo floating point dither
		int expon; frexpf((float)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		frexpf((float)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}

void EverySlewEngine::processDoubleReplacing(double **inputs, double **outputs, int sampleFrames) 
{
    double* in1  =  inputs[0];
    double* in2  =  inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

	double overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= getSampleRate();
	
	double source = pow(1-A,4)/overallscale;
	int stages = (1.0-B)*9.99;
	stages *= 5;
	double halo = C;
	double wet = (D*2.0)-1.0; //inv-dry-wet for highpass
	double dry = 2.0-(D*2.0);
	if (dry > 1.0) dry = 1.0; //full dry for use with inv, to 0.0 at full wet
	
	gslew[threshold10] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold9] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold8] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold7] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold6] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold5] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold4] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold3] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold2] = source;
	source *= 1.618033988749894848204586;
	gslew[threshold1] = source;
	source *= 1.618033988749894848204586;
	
    while (--sampleFrames >= 0)
    {
		double inputSampleL = *in1;
		double inputSampleR = *in2;
		if (fabs(inputSampleL)<1.18e-23) inputSampleL = fpdL * 1.18e-17;
		if (fabs(inputSampleR)<1.18e-23) inputSampleR = fpdR * 1.18e-17;
		double drySampleL = inputSampleL;
		double drySampleR = inputSampleR;
		
		for (int x = stages; x < gslew_total; x += 5) {
			if (((inputSampleL-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleL = (gslew[x]-((gslew[x]-gslew[x+2])*halo)) + (gslew[x+4]*(1.0-halo));
			if (-((inputSampleL-gslew[x])-((gslew[x]-gslew[x+2])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleL = (gslew[x]-((gslew[x]-gslew[x+2])*halo*0.78)) - (gslew[x+4]*(1.0-(halo*0.78)));
			gslew[x+2] = gslew[x]*(1.0-halo);
			gslew[x] = inputSampleL;
			
			if (((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*halo)) + (gslew[x+4]*(1.0-halo));
			if (-((inputSampleR-gslew[x+1])-((gslew[x+1]-gslew[x+3])*0.618033988749894848204586)) > gslew[x+4])
				inputSampleR = (gslew[x+1]-((gslew[x+1]-gslew[x+3])*halo*0.78)) - (gslew[x+4]*(1.0-(halo*0.78)));
			gslew[x+3] = gslew[x+1]*(1.0-halo);
			gslew[x+1] = inputSampleR;			
		}
		
		inputSampleL *= wet;
		inputSampleR *= wet;
		drySampleL *= dry;
		drySampleR *= dry;
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		
		//begin 64 bit stereo floating point dither
		//int expon; frexp((double)inputSampleL, &expon);
		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
		//inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		//frexp((double)inputSampleR, &expon);
		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
		//inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
		//end 64 bit stereo floating point dither
		
		*out1 = inputSampleL;
		*out2 = inputSampleR;

		in1++;
		in2++;
		out1++;
		out2++;
    }
}
