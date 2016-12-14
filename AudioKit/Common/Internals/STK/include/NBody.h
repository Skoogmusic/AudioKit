//
//  NBody.h
//  Skoog
//
//  Created by Keith Nagle on 23/07/2014.
//  Copyright (c) 2014 Skoogmusic Ltd. All rights reserved.
//

#ifndef STK_NBODY_H
#define STK_NBODY_H

#include "Stk.h"

#define SQRT_TWO 1.414213562
#define BUFSIZE 8192 //4*2048

#define ORDER 12

#define GUITAR1 1
#define GUITAR2 2
#define VIOLIN 3
#define MANDOLIN 4
#define HARDANGER 5
#define ARCHTOP 6

namespace stk {
    class NBody
    {
    public:
        /****PROTOTYPES****/
        
        //setup funcs
        NBody(); 
        //! Class destructor.
        ~NBody();
        void guitar1(float f);
        void guitar2(float f);
        void mandolin(float f);
        void violin(float f);
        void hardanger(float f);
        void archtop(float f);

        
        //allpole funcs
        void setPoleCoeffs(float *coeffs);
        float ap_tick(float sample);
        
        //onezero funcs
        void setOneZeroCoeff(float aValue);
        float oz_tick(float sample);
        
        //! Input one sample to the filter and return one output.
        float tick(float input);
        
    protected:
        short instrument;
        short direction;
        
        //allpole vars
        float poleCoeffs[ORDER];
        float ap_outputs[ORDER];
        float ap_gain;
        
        //onezero vars
        float zeroCoeff;
        float sgain;
        float oz_gain;
        float oz_input;
        
        //user controlled vars
        
        //signals connected? or controls...
        short num_objectsConnected;
        short res_freqConnected;
        short shake_dampConnected;
        short shake_maxConnected;
        
        float srate, one_over_srate;
    };
    
    inline float NBody :: tick(float input)
    {
        float temp;
        temp = oz_tick(input);
		temp = ap_tick(temp);
        return temp;
    }
    
    } // namespace

#endif
