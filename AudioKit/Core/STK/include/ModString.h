//
//  ModString.h
//  Skoog
//
//  Created by Keith Nagle on 22/07/2014.
//  Copyright (c) 2014 Skoogmusic Ltd. All rights reserved.
//
#ifndef STK_MODSTRING_H
#define STK_MODSTRING_H

#include "Instrmnt.h"
#include "Delay.h"
#include "PoleZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "FileWvIn.h"
#include "NBody.h"
//#include "Twang.h"
#include <cmath>

#define SCALE(OldValue, OldMin, NewMax, NewMin, OldMax) (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin

namespace stk {
    class ModString : public Instrmnt {
    public:
        //! Class constructor, taking the lowest desired playing frequency.
        /*!
         An StkError will be thrown if the rawwave path is incorrectly set.
         */
        ModString( StkFloat lowestFrequency );
        
        //! Class destructor.
        ~ModString(void);
        
        //! Reset and clear all internal state.
        void clear(void);
        
        void pluck( StkFloat amplitude );
        
        //! Set instrument parameters for a particular frequency.
        void setFrequency(StkFloat frequency);
        
        void setGain(StkFloat gain);

        //! Start a note with the given frequency and amplitude.
        void noteOn( StkFloat frequency, StkFloat amplitude);
        
        //! Stop a note with the given amplitude (speed of decay).
        void noteOff(StkFloat amplitude);
        
        //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
        void controlChange(int number, StkFloat value);
        
        void setFundamental(StkFloat f); // could be replaced with setFrequency
        void setDensity(StkFloat f);
        void setTension(StkFloat f);
        void setAirD(StkFloat f);
        void setIntD(StkFloat f);
        void setExcitation(StkFloat f);
        void setPickup(StkFloat f);
        void setHorizD(StkFloat f);
        void setTwangtime(StkFloat f);
        void setDetune(StkFloat f);
        void setMaxtwang(StkFloat f);
        //void setAmp(StkFloat f);
        void setMaxdetune(StkFloat f);
        void setNoteon(StkFloat f);
        void setAutonoteoff(StkFloat f);
        void setSkoognoteoff(StkFloat f);
        void setT60(StkFloat f);
        void setT60min(StkFloat f);
        void setDamptime(StkFloat f);
        void setDetunetype(int i);
        void detunebang();
        void setBodymode(int i);
        void string2filter();
        void getFilterParams(StkFloat tt, StkFloat damp1, StkFloat damp2, StkFloat radianfreq, StkFloat *array);
        
        //! Compute and return one output sample.
        
        StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
        StkFloat tick(  unsigned int channel = 0 );
        
        Delay delay1;
        Delay delay2;
        Delay delay3;
        Delay delay4;
        Delay delay5;
        
        Delay delay2h;
        Delay delay3h;
        Delay delay4h;
        Delay delay5h;
        
        PoleZero polezeroForward1;
        PoleZero polezeroBackward1;
        PoleZero polezeroForward2;
        
        PoleZero polezeroForward1h;
        PoleZero polezeroBackward1h;
        PoleZero polezeroForward2h;
        
        Envelope detune;
        Envelope damping;
        
        //DelayL  combDelay;
        //Twang twanger;
        NBody body;
        
        Noise noise;
        
    protected:

        
    	StkFloat frequency;
        StkFloat fundamental;
        StkFloat resting_fundamental;
        StkFloat density;
        StkFloat tension;
        StkFloat airD;
        StkFloat intD;
        StkFloat excitation;
        StkFloat pickup;
        StkFloat horizD;
        StkFloat maxtwang;
        StkFloat amp;
        StkFloat maxdetune;
        StkFloat trvE;
        StkFloat tsdl;
        StkFloat trvpk;
        StkFloat T60;
        StkFloat T60min;
        StkFloat damptime;
        int bodymode;
        StkFloat srate;
        StkFloat one_over_srate;
        StkFloat pluckAmplitude;
        
        FileWvIn *soundfile[12];
        
        int mic;
        long dampTime;
        bool waveDone;
    };
    
    inline StkFrames& ModString :: tick( StkFrames& frames, unsigned int channel )
    {
        unsigned int nChannels = lastFrame_.channels();
#if defined(_STK_DEBUG_)
        if ( channel > frames.channels() - nChannels ) {
            oStream_ << "ModString::tick(): channel and StkFrames arguments are incompatible!";
            handleError( StkError::FUNCTION_ARGUMENT );
        }
#endif
        
        StkFloat *samples = &frames[channel];
        unsigned int j, hop = frames.channels() - nChannels;
        if ( nChannels == 1 ) {
            for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
                *samples++ = tick();
        }
        else {
            for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
                *samples++ = tick();
                for ( j=1; j<nChannels; j++ )
                    *samples++ = lastFrame_[j];
            }
        }
        
        return frames;
    }
    
    inline StkFloat ModString :: tick( unsigned int )
    {
        StkFloat temp = 0.0;
        if (!waveDone) {
            // Scale the pluck excitation with comb
            // filtering for the duration of the file.
            temp = soundfile[mic]->tick() * this->pluckAmplitude;
            waveDone = soundfile[mic]->isFinished();
        }
        
        float input; // input for vertical and horizontal filters
        float feedbackOutput, feedbackOutputh;
        
        // if the detune envelope (line) is currently active then we need to alter the
        // fundamental frequency and re-calculate coefficients accordingly
        if (detune.getState()) {
            this->fundamental = this->resting_fundamental + this->detune.lastOut();
            this->string2filter();
        }
            
        this->detune.tick(); // keep the envelope ticking along if it needs to
            
        input = temp; // excitation
        
        
            
        // feed forward 1
        
        // send the input down a delay line
        // send it down another delay line for a polezero object
        // feed the result to the polezero
        // subtract the output from the original delayed signal
        //forwardOneOutput = this->delay1.tick(input) - this->polezeroForward1.tick(this->delay2.tick(input));
        
        // feedbackward
        // add input signal to the output of a polezero object
        feedbackOutput = this->delay1.tick(input) - this->polezeroForward1.tick(this->delay2.tick(input)) + this->polezeroBackward1.lastOut();
        // send the result down a delay back to the polezero
        // along with a damping multiplier
        this->polezeroBackward1.tick(this->delay3.tick(feedbackOutput) * this->damping.tick());
            
        // feed forward 2
        // now take output from feedbackward and send down a delay line
        // send it down another delay line for a polezero object
        // feed the result to the polezero, we will subtract the output from the original at the end
        //this->polezeroForward2.tick(this->delay5.tick(feedbackOutput));
        
        // feed forward 1 - Horizontal
        // send the original input down a delay line for a polezero object
        // feed the result to the polezero
        // subtract the output from the original delayed signal
        //forwardOneOutputh = this->delay1.lastOut() - this->polezeroForward1h.tick(this->delay2h.tick(input));
        
        // feedbackward - Horizontal
        feedbackOutputh = this->delay1.lastOut() - this->polezeroForward1h.tick(this->delay2h.tick(input)) + this->polezeroBackward1h.lastOut();
        
        this->polezeroBackward1h.tick(this->delay3h.tick(feedbackOutputh) * /*1.0F*/ this->damping.lastOut());
        
        // finally combine the output of both feedforward 2 objects to get our actual sample
        temp += this->delay4.tick(feedbackOutput) - this->polezeroForward2.tick(this->delay5.tick(feedbackOutput)) + this->delay4h.tick(feedbackOutputh) - this->polezeroForward2h.tick(this->delay5h.tick(feedbackOutputh)) + (noise.tick() * 0.0001);
        
        return this->body.tick(temp);
        //return this->body.tick(twanger.tick(input));
    }
}
#endif /* defined(__Skoog__ModString__) */
