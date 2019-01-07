//
//  MogoInstrumentDSPKernel.hpp
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef MogoInstrumentDSPKernel_hpp
#define MogoInstrumentDSPKernel_hpp

#import "DSPKernel.hpp"
#import "ParameterRamper.hpp"
#import <vector>
#include <iostream>

#import <AudioKit/AudioKit-Swift.h>

extern "C" {
#include "soundpipe.h"
}

enum {
    attackDurationAddress = 0,
    decayDurationAddress = 1,
    sustainLevelAddress = 2,
    releaseDurationAddress = 3,
    detuningOffsetAddress = 4,
    detuningMultiplierAddress = 5,
    cutoffFrequencyAddress = 6,
    resonanceAddress = 7
};

//static inline double pow2(double x) {
//    return x * x;
//}

//static inline double noteToHz(int noteNumber)
//{
//    return 440. * exp2((noteNumber - 69)/12.);
//}

class MogoInstrumentDSPKernel : public DSPKernel {
public:
    // MARK: Types
    struct NoteState {
        NoteState* next;
        NoteState* prev;
        MogoInstrumentDSPKernel* kernel;
        
        ParameterRamper cutoffFrequencyRamper = 1000;
        ParameterRamper resonanceRamper = 0.5;
        
        enum { stageOff, stageOn, stageRelease };
        int stage = stageOff;
        
        float internalGate = 0;
        float amp = 0;
        float freq_amp = 0;
        float cutoffFrequency = 1000;
        float resonance = 0.5;
        
        sp_adsr *adsr;
        sp_adsr *freq_adsr;
        sp_blsquare *blsquare;
        sp_blsaw *blsaw1;
        sp_blsaw *blsaw2;
        sp_moogladder *moogladder;

        void init() {
            sp_adsr_create(&adsr);
            sp_adsr_create(&freq_adsr);
            sp_blsquare_create(&blsquare);
            sp_blsaw_create(&blsaw1);
            sp_blsaw_create(&blsaw2);
            sp_moogladder_create(&moogladder);
            sp_adsr_init(kernel->sp, adsr);
            sp_adsr_init(kernel->sp, freq_adsr);
            sp_blsquare_init(kernel->sp, blsquare);
            sp_blsaw_init(kernel->sp, blsaw1);
            sp_blsaw_init(kernel->sp, blsaw2);
            sp_moogladder_init(kernel->sp, moogladder);
            
            moogladder->freq = 1000;
            moogladder->res = 0.5;
            
            *blsquare->freq = 0;
            *blsquare->amp = 0;
            *blsaw1->freq = 0;
            *blsaw1->amp = 0;
            *blsaw2->freq = 0;
            *blsaw2->amp = 0;
            
            freq_adsr->sus = 0.5;
            
            cutoffFrequencyRamper.init();
            resonanceRamper.init();
        }

        void clear() {
            stage = stageOff;
            amp = 0;
        }
        
        // linked list management
        void remove() {
            if (prev) prev->next = next;
            else kernel->playingNotes = next;
            
            if (next) next->prev = prev;
            
            //prev = next = nullptr; Had to remove due to a click, potentially bad
            
            --kernel->playingNotesCount;

//            sp_osc_destroy(&osc);
            
            sp_adsr_destroy(&adsr);
            sp_adsr_destroy(&freq_adsr);
            sp_blsquare_destroy(&blsquare);
            sp_blsaw_destroy(&blsaw1);
            sp_blsaw_destroy(&blsaw2);
            sp_moogladder_destroy(&moogladder);
        }
        
        void add() {
            init();
            prev = nullptr;
            next = kernel->playingNotes;
            if (next) next->prev = this;
            kernel->playingNotes = this;
            ++kernel->playingNotesCount;
        }
        
        void noteOn(int noteNumber, int velocity)
        {
            if (velocity == 0) {
                if (stage == stageOn) {
                    stage = stageRelease;
                    internalGate = 0;
                }
            } else {
                if (stage == stageOff) { add(); }
                *blsquare->freq = (float)noteToHz(noteNumber) * 0.5;
                *blsquare->amp = (float)pow2(velocity / 127.);
                *blsaw1->freq = (float)noteToHz(noteNumber) * 0.9999;
                *blsaw1->amp = (float)pow2(velocity / 127.);
                *blsaw2->freq = (float)noteToHz(noteNumber);
                *blsaw2->amp = (float)pow2(velocity / 127.);
                stage = stageOn;
                internalGate = 1;
            }
        }
        
        void setResonance(float velocity) {
            float value = 0.2f + (velocity * 0.59);
            resonance = clamp(value, 0.0f, 2.0f);
            resonanceRamper.setImmediate(resonance);
        }
        
        void setCutoffFrequency(float velocity) {
            float value =  20.0f + (velocity * 10000);
            cutoffFrequency = clamp(value, 12.0f, 20000.0f);
            cutoffFrequencyRamper.setImmediate(cutoffFrequency);
        }
        void setCutoffFrequencySustain(float velocity) {
            if (stage == stageOn) {
                freq_adsr->sus = velocity;
            }
        }
        
        void run(int frameCount, float* outL, float* outR)
        {
            float originalFrequency = *blsaw2->freq;
//            osc->freq *= kernel->detuningMultiplier;
//            osc->freq += kernel->detuningOffset;
//            osc->freq = clamp(osc->freq, 0.0f, 22050.0f);
//            *blsquare->freq *= kernel->detuningMultiplier;
//            *blsquare->freq += kernel->detuningOffset;
            *blsquare->freq = clamp(*blsquare->freq, 0.0f, 22050.0f);
            
//            *blsaw1->freq *= kernel->detuningMultiplier;
//            *blsaw1->freq += kernel->detuningOffset;
            *blsaw1->freq = clamp(*blsaw1->freq, 0.0f, 22050.0f);
            
//            *blsaw2->freq *= kernel->detuningMultiplier;
//            *blsaw2->freq += kernel->detuningOffset;
            *blsaw2->freq = clamp(*blsaw2->freq, 0.0f, 22050.0f);
            
            adsr->atk = (float)kernel->attackDuration;
            adsr->dec = (float)kernel->decayDuration;
            adsr->sus = (float)kernel->sustainLevel;
            adsr->rel = (float)kernel->releaseDuration;
            
            freq_adsr->atk = 0.001;
            freq_adsr->dec = 0.0396;
//            freq_adsr->sus = 0.66;
            freq_adsr->rel = (float)kernel->releaseDuration;
            
            cutoffFrequency = cutoffFrequencyRamper.getAndStep();
            moogladder->freq = (float)cutoffFrequency;
            resonance = resonanceRamper.getAndStep();
            moogladder->res = (float)resonance;

            for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
                float x, y, z = 0;
                sp_adsr_compute(kernel->sp, adsr, &internalGate, &amp);
                sp_adsr_compute(kernel->sp, freq_adsr, &internalGate, &freq_amp);
                sp_blsquare_compute(kernel->sp, blsquare, nil, &x);
                sp_blsaw_compute(kernel->sp, blsaw1, nil, &y);
                sp_blsaw_compute(kernel->sp, blsaw2, nil, &z);
                moogladder->freq = (float)cutoffFrequency * freq_amp;
                float tempIn = amp * (x + y + z);
                float tempOut;
                sp_moogladder_compute(kernel->sp, moogladder, &tempIn, &tempOut);
                *outL++ += tempOut;
                *outR++ += tempOut;
                
            }

            *blsaw2->freq = originalFrequency;
            if (stage == stageRelease && amp < 0.00001) {
                clear();
                remove();
            }
        }
    
    };

    // MARK: Member Functions

    MogoInstrumentDSPKernel() {
        noteStates.resize(128);
        for (NoteState& state : noteStates) {
            state.kernel = this;
        }
    }

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp->sr = sampleRate;
        sp->nchan = channels;
        
//        sp_moogladder *moogladder;
//        sp_moogladder_create(&moogladder);
//        sp_moogladder_init(sp, moogladder);
//        moogladder->freq = 1000;
//        moogladder->res = 0.5;
        
        attackDurationRamper.init();
        decayDurationRamper.init();
        sustainLevelRamper.init();
        releaseDurationRamper.init();
        detuningOffsetRamper.init();
        detuningMultiplierRamper.init();
//        cutoffFrequencyRamper.init();
//        resonanceRamper.init();
    }

//    void setupWaveform(uint32_t size) {
//        ftbl_size = size;
//        sp_ftbl_create(sp, &ftbl, ftbl_size);
//    }
//
//    void setWaveformValue(uint32_t index, float value) {
//        ftbl->tbl[index] = value;
//    }

    void startNote(int note, int velocity) {
        noteStates[note].noteOn(note, velocity);
    }

    void stopNote(int note) {
        noteStates[note].noteOn(note, 0);
    }

    void destroy() {
//        sp_moogladder_destroy(&moogladder);
        sp_destroy(&sp);
    }

    void reset() {
        for (NoteState& state : noteStates) {
            state.cutoffFrequencyRamper.reset();
            state.resonanceRamper.reset();
            state.clear();
        }
        playingNotes = nullptr;
        playingNotesCount = 0;
        resetted = true;
        
        attackDurationRamper.reset();
        decayDurationRamper.reset();
        sustainLevelRamper.reset();
        releaseDurationRamper.reset();
        detuningOffsetRamper.reset();
        detuningMultiplierRamper.reset();
    }
//    void setCutoffFrequency(float value) {
//        cutoffFrequency = clamp(value, 12.0f, 20000.0f);
//        cutoffFrequencyRamper.setImmediate(cutoffFrequency);
//    }
    
    void setCutoffFrequency(int note, float velocity) {
        noteStates[note].setCutoffFrequency(velocity);
    }
    void setCutoffFrequencySustain(int note, float velocity) {
        noteStates[note].setCutoffFrequencySustain(velocity);
    }
    
    void setResonance(int note, float velocity) {
        noteStates[note].setResonance(velocity);
    }
    
//    void setResonance(float value) {
//        resonance = clamp(value, 0.0f, 2.0f);
//        resonanceRamper.setImmediate(resonance);
//    }

    void setAttackDuration(float value) {
        attackDuration = clamp(value, 0.0f, 99.0f);
        attackDurationRamper.setImmediate(attackDuration);
    }
    
    void setDecayDuration(float value) {
        decayDuration = clamp(value, 0.0f, 99.0f);
        decayDurationRamper.setImmediate(decayDuration);
    }
    
    void setSustainLevel(float value) {
        sustainLevel = clamp(value, 0.0f, 99.0f);
        sustainLevelRamper.setImmediate(sustainLevel);
    }
    
    void setReleaseDuration(float value) {
        releaseDuration = clamp(value, 0.0f, 99.0f);
        releaseDurationRamper.setImmediate(releaseDuration);
    }
    
    void setDetuningOffset(float value) {
        detuningOffset = clamp(value, (float)-1000, (float)1000);
        detuningOffsetRamper.setImmediate(detuningOffset);
    }

    void setDetuningMultiplier(float value) {
        detuningMultiplier = clamp(value, (float)0.5, (float)2.0);
        detuningMultiplierRamper.setImmediate(detuningMultiplier);
    }


    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {

            case attackDurationAddress:
                attackDurationRamper.setUIValue(clamp(value, 0.0f, 99.0f));
                break;
                
            case decayDurationAddress:
                decayDurationRamper.setUIValue(clamp(value, 0.0f, 99.0f));
                break;
                
            case sustainLevelAddress:
                sustainLevelRamper.setUIValue(clamp(value, 0.0f, 99.0f));
                break;
                
            case releaseDurationAddress:
                releaseDurationRamper.setUIValue(clamp(value, 0.0f, 99.0f));
                break;
                
            case detuningOffsetAddress:
                detuningOffsetRamper.setUIValue(clamp(value, (float)-1000, (float)1000));
                break;

            case detuningMultiplierAddress:
                detuningMultiplierRamper.setUIValue(clamp(value, (float)0.5, (float)2.0));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {

            case attackDurationAddress:
                return attackDurationRamper.getUIValue();
                
            case decayDurationAddress:
                return decayDurationRamper.getUIValue();
                
            case sustainLevelAddress:
                return sustainLevelRamper.getUIValue();
                
            case releaseDurationAddress:
                return releaseDurationRamper.getUIValue();
            
            case detuningOffsetAddress:
                return detuningOffsetRamper.getUIValue();

            case detuningMultiplierAddress:
                return detuningMultiplierRamper.getUIValue();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
                
            case attackDurationAddress:
                attackDurationRamper.startRamp(clamp(value, 0.0f, 99.0f), duration);
                break;
                
            case decayDurationAddress:
                decayDurationRamper.startRamp(clamp(value, 0.0f, 99.0f), duration);
                break;
                
            case sustainLevelAddress:
                sustainLevelRamper.startRamp(clamp(value, 0.0f, 99.0f), duration);
                break;
                
            case releaseDurationAddress:
                releaseDurationRamper.startRamp(clamp(value, 0.0f, 99.0f), duration);
                break;

            case detuningOffsetAddress:
                detuningOffsetRamper.startRamp(clamp(value, (float)-1000, (float)1000), duration);
                break;

            case detuningMultiplierAddress:
                detuningMultiplierRamper.startRamp(clamp(value, (float)0.5, (float)2.0), duration);
                break;
            case cutoffFrequencyAddress:
//                cutoffFrequencyRamper.startRamp(clamp(value, 12.0f, 20000.0f), duration);
                break;
                
            case resonanceAddress:
//                resonanceRamper.startRamp(clamp(value, 0.0f, 2.0f), duration);
                break;

        }
    }

    void setBuffer(AudioBufferList *outBufferList) {
        outBufferListPtr = outBufferList;
    }
    
    virtual void handleMIDIEvent(AUMIDIEvent const& midiEvent) override {
        if (midiEvent.length != 3) return;
        uint8_t status = midiEvent.data[0] & 0xF0;
        //uint8_t channel = midiEvent.data[0] & 0x0F; // works in omni mode.
        switch (status) {
            case 0x80 : { // note off
                uint8_t note = midiEvent.data[1];
                if (note > 127) break;
                noteStates[note].noteOn(note, 0);
                break;
            }
            case 0x90 : { // note on
                uint8_t note = midiEvent.data[1];
                uint8_t veloc = midiEvent.data[2];
                if (note > 127 || veloc > 127) break;
                noteStates[note].noteOn(note, veloc);
                break;
            }
            case 0xB0 : { // control
                uint8_t num = midiEvent.data[1];
                if (num == 123) { // all notes off
                    NoteState* noteState = playingNotes;
                    while (noteState) {
                        noteState->clear();
                        noteState = noteState->next;
                    }
                    playingNotes = nullptr;
                    playingNotesCount = 0;
                }
                break;
            }
        }
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {

        float* outL = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset;
        float* outR = (float*)outBufferListPtr->mBuffers[1].mData + bufferOffset;

        attackDuration = attackDurationRamper.getAndStep();
        decayDuration = decayDurationRamper.getAndStep();
        sustainLevel = sustainLevelRamper.getAndStep();
        releaseDuration = releaseDurationRamper.getAndStep();

        detuningOffset = double(detuningOffsetRamper.getAndStep());
        detuningMultiplier = double(detuningMultiplierRamper.getAndStep());
        
        for (AUAudioFrameCount i = 0; i < frameCount; ++i) {
            outL[i] = 0.0f;
            outR[i] = 0.0f;
        }
        
        NoteState* noteState = playingNotes;
        while (noteState) {
            noteState->run(frameCount, outL, outR);
            noteState = noteState->next;
        }

        for (AUAudioFrameCount i = 0; i < frameCount; ++i) {
            outL[i] *= .5f;
            outR[i] *= .5f;
        }
    }

    // MARK: Member Variables

private:
    std::vector<NoteState> noteStates;

    int channels = AKSettings.channelCount;
    float sampleRate = AKSettings.sampleRate;
    double frequencyScale = 2. * M_PI / sampleRate;
    
    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
//    sp_ftbl *ftbl;
//    UInt32 ftbl_size = 4096;
//    sp_moogladder *moogladder;
    
    float cutoffFrequency = 1000;
    float resonance = 0.5;

    float attackDuration = 0.1;
    float decayDuration = 0.1;
    float sustainLevel = 1.0;
    float releaseDuration = 0.1;

    float detuningOffset = 0;
    float detuningMultiplier = 1;

public:
    NoteState* playingNotes = nullptr;
    int playingNotesCount = 0;
    bool started = true;
    bool resetted = false;
    
    ParameterRamper attackDurationRamper = 0.1;
    ParameterRamper decayDurationRamper = 0.1;
    ParameterRamper sustainLevelRamper = 1.0;
    ParameterRamper releaseDurationRamper = 0.1;

    ParameterRamper detuningOffsetRamper = 0;
    ParameterRamper detuningMultiplierRamper = 1;
    
//    ParameterRamper cutoffFrequencyRamper = 1000;
//    ParameterRamper resonanceRamper = 0.5;
};

#endif /* MogoInstrumentDSPKernel_hpp */
