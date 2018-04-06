//
//  NBody.cpp
//  Skoog
//
//  Created by Keith Nagle on 23/07/2014.
//  Copyright (c) 2014 Skoogmusic Ltd. All rights reserved.
//

#include "NBody.h"
#include <math.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include "bodycoeffs.h"
#include "Dunlap12.lpc.h"
#include "Fiddle12.lpc.h"
#include "Mando12.lpc.h"
#include "Violin12.lpc.h"
#include "Arch12.lpc.h"
#include "Abreu12.lpc.h"
#include <cmath>
namespace stk {
    
    NBody :: NBody(void)
    {
        //allpole init
        for (int i = 0; i < ORDER; i++)
            this->ap_outputs[i] = (float) 0.0;
        this->ap_gain = 1.;
        
        //onezero init
        this->zeroCoeff = 1.;
        this->sgain = .5;
        this->oz_gain = 1.;
        this->oz_input = 0.;
        
        this->instrument = GUITAR1;
        this->direction = 1;
        guitar1(0.);
        
        this->srate = 44100;
        this->one_over_srate = 1./this->srate;
    }
    
    NBody :: ~NBody(void)
    {

    }
    
    void NBody :: setOneZeroCoeff(float aValue)
    {
        this->zeroCoeff = aValue;
        if (this->zeroCoeff > 0.0)                  /*  Normalize gain to 1.0 max  */
            this->sgain = this->oz_gain / ((float) 1.0 + this->zeroCoeff);
        else
            this->sgain = this->oz_gain / ((float) 1.0 - this->zeroCoeff);
        
    }
    float NBody :: oz_tick(float sample)
    {
        float temp, outsamp;
        temp = this->sgain * sample;
        outsamp = (this->oz_input * this->zeroCoeff) + temp;
        this->oz_input = temp;
        return outsamp;
    }
    
    void NBody :: setPoleCoeffs(float *coeffs)
    {
        int i;
        for(i = 0; i < ORDER; i++) {
            this->poleCoeffs[i] = coeffs[i];
        }
    }
    
    float NBody :: ap_tick(float sample)
    {
        int i;
        float temp;
        
        temp = sample * this->ap_gain;
        
        for (i = ORDER - 1; i > 0; i--)	{
            temp -= this->ap_outputs[i] * this->poleCoeffs[i];
            this->ap_outputs[i] = this->ap_outputs[i-1];
        }
        temp -= this->ap_outputs[0] * this->poleCoeffs[0]; 
        this->ap_outputs[0] = temp;
        
        return temp;
    }
    
    void NBody :: guitar1(float f)
    {
        this->instrument = GUITAR1;
        this->ap_gain = (0.07 * abreuGains[abreuMap[this->direction]]);
        setPoleCoeffs(abreu_coeffs[abreuMap[this->direction]]);
        setOneZeroCoeff(abreuZero);
    }
    
    void NBody :: guitar2(float f)
    {
        this->instrument = GUITAR2;
        this->ap_gain = (0.07 * dunlapGains[dunlapMap[this->direction]]);
        setPoleCoeffs(dunlap_coeffs[dunlapMap[this->direction]]);
        setOneZeroCoeff(dunlapZero);
    }
    
    void NBody :: mandolin(float f)
    {
        this->instrument = MANDOLIN;
        this->ap_gain = (0.1 * mandoGains[mandoMap[this->direction]]); 
        setPoleCoeffs(mando_coeffs[mandoMap[this->direction]]);
        setOneZeroCoeff(mandoZero);
    }
    
    void NBody :: violin(float f)
    {
        this->instrument = VIOLIN;
        this->ap_gain = (0.2*violinGains[violinMap[this->direction]]);
        setPoleCoeffs(violin_coeffs[violinMap[this->direction]]);
        setOneZeroCoeff(violinZero);
    }
    
    void NBody :: hardanger(float f)
    {
        this->instrument = HARDANGER;
        this->ap_gain = (fiddleGain * fiddleGains[fiddleMap[this->direction]]);
        setPoleCoeffs(fiddle_coeffs[fiddleMap[this->direction]]);
        setOneZeroCoeff(fiddleZero);
    }
    
    void NBody :: archtop(float f)
    {
        this->instrument = ARCHTOP;
        this->ap_gain = (0.07 * archGains[archMap[this->direction]]);
        setPoleCoeffs(arch_coeffs[archMap[this->direction]]);
        setOneZeroCoeff(archZero);
    }
}
