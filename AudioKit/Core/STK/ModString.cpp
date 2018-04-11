//
//  ModString.cpp
//  Skoog
//
//  Created by Keith Nagle on 22/07/2014.
//  Copyright (c) 2014 Skoogmusic Ltd. All rights reserved.
//

#include "ModString.h"
#include "Delay.h"

namespace stk {
    
    ModString :: ModString( StkFloat lowestFrequency )
    {
        // Concatenate the STK rawwave path to the rawwave files Plucking_a_guitar_string_with_a_buzz_5b.raw
        soundfile[0].openFile((Stk::rawwavePath() + "Plucking_a_guitar_string_with_a_buzz_5b.raw").c_str(), true);
//        soundfile[0] = new FileWvIn( (Stk::rawwavePath() + "Plucking_a_guitar_string_with_a_buzz_5b.raw").c_str(), true );
//        soundfile[1] = new FileWvIn( (Stk::rawwavePath() + "mand2.raw").c_str(), true );
//        soundfile[2] = new FileWvIn( (Stk::rawwavePath() + "mand3.raw").c_str(), true );
//        soundfile[3] = new FileWvIn( (Stk::rawwavePath() + "mand4.raw").c_str(), true );
//        soundfile[4] = new FileWvIn( (Stk::rawwavePath() + "mand5.raw").c_str(), true );
//        soundfile[5] = new FileWvIn( (Stk::rawwavePath() + "mand6.raw").c_str(), true );
//        soundfile[6] = new FileWvIn( (Stk::rawwavePath() + "mand7.raw").c_str(), true );
//        soundfile[7] = new FileWvIn( (Stk::rawwavePath() + "mand8.raw").c_str(), true );
//        soundfile[8] = new FileWvIn( (Stk::rawwavePath() + "mand9.raw").c_str(), true );
//        soundfile[9] = new FileWvIn( (Stk::rawwavePath() + "mand10.raw").c_str(), true );
//        soundfile[10] = new FileWvIn( (Stk::rawwavePath() + "mand11.raw").c_str(), true );
//        soundfile[11] = new FileWvIn( (Stk::rawwavePath() + "mand12.raw").c_str(), true );
        
        this->mic = 0;
        this->dampTime = 0;
        this->waveDone = soundfile[mic].isFinished();
        this->srate = 44100.0F;
        this->one_over_srate = 1.0F / srate;
        
        //twanger.setLowestFrequency(0);
        this->pluckAmplitude = 0.01;
        setFrequency(lowestFrequency);
        
        setFundamental(42.0F);
        //setResting_fundamental;
        setDensity(0.0004F);
        setTension(1165.0F);
        setAirD(0.7F);
        setIntD(0.086F);
        setExcitation(0.2F);
        setPickup(0.11F);
        setHorizD(0.68F);
        setMaxtwang(72);
        setGain(0.1F); // fix later
        setMaxdetune(-0.004F);
        setT60(1470.0F);
        setT60min(76.0F);
        setDamptime(48);
        setBodymode(1);
        
        delay1.setMaximumDelay(44100);
        delay2.setMaximumDelay(44100);
        delay3.setMaximumDelay(44100);
        delay4.setMaximumDelay(44100);
        delay5.setMaximumDelay(44100);
        
        delay2h.setMaximumDelay(44100);
        delay3h.setMaximumDelay(44100);
        delay4h.setMaximumDelay(44100);
        delay5h.setMaximumDelay(44100);
        
        delay1.clear();
        delay2.clear();
        delay3.clear();
        delay4.clear();
        delay5.clear();
        
        delay2h.clear();
        delay3h.clear();
        delay4h.clear();
        delay5h.clear();
        
        delay1.setDelay(1);
        delay4.setDelay(1);
        delay4h.setDelay(1);
        
        detune.setRate(0);
        detune.setTarget(0);
        detune.setValue(0);
        
        damping.setRate(0);
        damping.setTarget(0);
        damping.setValue(0);
        
        polezeroForward1.setCoefficients(0, 0, 0, true);
        polezeroBackward1.setCoefficients(0, 0, 0, true);
        polezeroForward2.setCoefficients(0, 0, 0, true);
        
        polezeroForward1h.setCoefficients(0, 0, 0, true);
        polezeroBackward1h.setCoefficients(0, 0, 0, true);
        polezeroForward2h.setCoefficients(0, 0, 0, true);
    }
    
    ModString :: ~ModString( void )
    {
//        for ( int i = 0; i < 12; i++ )
//            delete soundfile[i];
    }
    void ModString :: clear(void)
    {
        //twanger.clear();
    }
    
    void ModString :: setFrequency(StkFloat f){
        this->frequency = f;
        //twanger.setFrequency(f);
        setFundamental(f);
    }

    void ModString :: setFundamental(StkFloat f){
        this->fundamental = 0.00002985 * f * f + 1.0011 * f + 0.295;
        this->resting_fundamental = this->fundamental;
        this->fundamental = this->resting_fundamental + detune.lastOut();
        string2filter();
    }
    
    void ModString :: setDensity(StkFloat f){
        this->density = f;
        string2filter();
    }
    
    void ModString :: setTension(StkFloat f){
        this->tension = f;
        string2filter();
    }
    
    void ModString :: setAirD(StkFloat f){
        this->airD = f;
        string2filter();
    }
    
    void ModString :: setIntD(StkFloat f){
        this->intD = f;
        string2filter();
    }
    
    void ModString :: setExcitation(StkFloat f){
        this->excitation = f;
        string2filter();
    }
    
    void ModString :: setPickup(StkFloat f){
        this->pickup = f;
        string2filter();
    }
    
    void ModString :: setHorizD(StkFloat f){
        this->horizD = f;
        string2filter();
    }
    
    void ModString :: setT60(StkFloat f){
        
        this->T60 = pow(10, -6000 * 32 / (f * this->srate));
        //errorString_ << "ModString::T60: value = " << f <<". expected " << this->T60 <<".";
        //handleError( StkError::WARNING );
    }
    
    void ModString :: setT60min(StkFloat f){
        this->T60min = pow(10, -6000 * 32 / (f * this->srate));
    }
    
    void ModString :: setDamptime(StkFloat f){
        this->damptime = f;
    }
    
    void ModString :: setBodymode(int i){
        this->bodymode = i;
    }
    
    void ModString :: setMaxtwang(StkFloat f){
        this->maxtwang = f;
    }
    
    void ModString :: setMaxdetune(StkFloat f){
        this->maxdetune = f;
    }
    
    void ModString:: string2filter()
    {
        StkFloat c = sqrt(this->tension / this->density);
        StkFloat length = c / (2.0 * this->fundamental);
        this->trvE = (2000.0 * length * this->excitation) / c;
        this->tsdl = (2000.0 * length) / c;
        this->trvpk = (2000.0 * length * this->pickup) / c;
        StkFloat w = 2.0 * PI * this->fundamental;
        StkFloat k =  w / c;
        StkFloat damping1 = this->airD;
        StkFloat damping2 = this->airD + this->intD * k * k;
        
        if (damping1 > damping2 - 0.000001)
            damping1 = damping2 - 0.000001;
        
        StkFloat damping1h = damping1 * horizD;
        StkFloat damping2h = damping2 * horizD;
        
        StkFloat coeffs[2];
        
        delay2.setDelay((this->trvE * (this->srate / 1000)) + 1);
        delay3.setDelay(this->tsdl * (this->srate / 1000));
        delay5.setDelay(this->trvpk * (this->srate / 1000) + 1);
        delay2h.setDelay((this->trvE * (this->srate / 1000)) + 1);
        delay3h.setDelay(this->tsdl * (this->srate / 1000));
        delay5h.setDelay(this->trvpk * (this->srate / 1000) + 1);

        getFilterParams(this->trvE, damping1, damping2, w, coeffs);
        
        polezeroForward1.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);

        getFilterParams(this->trvE, damping1h, damping2h, w, coeffs);
        
        polezeroForward1h.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);
        
        getFilterParams(this->tsdl, damping1, damping2, w, coeffs);
        
        polezeroBackward1.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);
        
        getFilterParams(this->tsdl, damping1h, damping2h, w, coeffs);
        
        polezeroBackward1h.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);
        
        getFilterParams(this->trvpk, damping1, damping2, w, coeffs);
        
        polezeroForward2.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);
        
        getFilterParams(this->trvpk, damping1h, damping2h, w, coeffs);
        
        polezeroForward2h.setCoefficients(coeffs[0], coeffs[0], coeffs[1], false);
    }
    
    void ModString:: getFilterParams(StkFloat tt, StkFloat damp1, StkFloat damp2, StkFloat radianfreq, StkFloat *array)
    {
        StkFloat P0 = exp(-damp1 * 0.001 * tt);
        StkFloat P1 = exp(-damp2 * 0.001 * tt);
        
        StkFloat gamma = sqrt(P0 * P0 - P1 * P1)/(P0 * radianfreq);
        
        array[0] = (P0 * this->one_over_srate)/(this->one_over_srate + 2 * gamma);
        array[1] = (this->one_over_srate - 2 * gamma)/(this->one_over_srate + 2 * gamma);
    }
    
    void ModString :: pluck( StkFloat amplitude )
    {
        // This function gets interesting, because pluck
        // may be longer than string length, so we just
        // reset the soundfile and add in the pluck in
        // the tick method.
        soundfile[mic].reset();
        this->waveDone = false;
        this->pluckAmplitude = amplitude;
        if ( amplitude < 0.0 ) {
            //oStream_ << "ModString::pluck: amplitude parameter less than zero ... setting to 0.0!";
            //handleError( StkError::WARNING );
            pluckAmplitude = 0.0;
        }
        else if ( amplitude > 1.0 ) {
            //oStream_ << "ModString::pluck: amplitude parameter greater than one ... setting to 1.0!";
            //handleError( StkError::WARNING );
            pluckAmplitude = 1.0;
        }
        detune.setValue(this->fundamental * -1 * SCALE(this->amp, 0.0, this->maxdetune, 0.0, 1.0));
        detune.setTime(SCALE(this->amp, 0.0, this->maxtwang / 1000, 0.0, 1.0));
        detune.setTarget(0);
    }
    
    void ModString :: noteOn(StkFloat frequency, StkFloat amplitude)
    {
        if (this->bodymode == 1){
            this->damping.setValue(this->T60);
            //oStream_ << "ModString::damping: value = " << this->T60 <<".";
            //handleError( StkError::WARNING );
        }
        this->setFrequency(frequency);
        this->pluck(amplitude);
        
#if defined(_STK_DEBUG_)
        oStream_ << "ModString::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
        handleError( StkError::DEBUG_WARNING );
#endif
    }
    
    void ModString :: noteOff(StkFloat amplitude )
    {
        if (this->bodymode == 1) {
            this->damping.setTime(this->damptime / 1000);
            this->damping.setTarget(this->T60min);
        }
        //this->setFrequency( frequency );
        //this->pluck( amplitude );
        
#if defined(_STK_DEBUG_)
        oStream_ << "ModString::NoteOff: frequency = " << frequency << ", amplitude = " << amplitude << ".";
        handleError( StkError::DEBUG_WARNING );
#endif
    }
    
    void ModString :: setGain(StkFloat gainValue)
    {
        if (gainValue > 0.0) {
            this->amp = gainValue;
            this->noteOn(this->frequency, gainValue);
        }
        else {
            this->amp = gainValue;
            this->noteOff(gainValue);
        }

    }
    
    void ModString :: controlChange( int number, StkFloat value )
    {
        StkFloat norm = value;
        if (number == 2)
        {
            
        }
        else if (number == 128) // 128
            this->setGain(norm);
        else {
            //oStream_ << "MoogICST::controlChange: undefined control number (" << number << ")!";
            //handleError( StkError::WARNING );
        }
        
#if defined(_STK_DEBUG_)
        errorString_ << "MoogICST::controlChange: number = " << number << ", value = " << value << ".";
        handleError( StkError::DEBUG_WARNING );
#endif
    }
}
