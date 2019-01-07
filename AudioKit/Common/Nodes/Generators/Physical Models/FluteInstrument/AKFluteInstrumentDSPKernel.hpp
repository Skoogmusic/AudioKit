//
//  AKFluteDSPKernel.hpp
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef AKFluteInstrumentDSPKernel_hpp
#define AKFluteInstrumentDSPKernel_hpp

#import "DSPKernel.hpp"
#import "ParameterRamper.hpp"

#import <AudioKit/AudioKit-Swift.h>

#include "Flute.h"
#include <iostream>

extern "C" {
#include "soundpipe.h"
}

enum {
    frequencyAddress = 0,
    amplitudeAddress = 1
};

//static inline double noteToHz(int noteNumber)
//{
//    return 440. * exp2((noteNumber - 69)/12.);
//}

class AKFluteInstrumentDSPKernel : public DSPKernel {
public:
    // MARK: Member Functions
    // MARK: Types
    struct NoteState {
        NoteState* next;
        NoteState* prev;
        AKFluteInstrumentDSPKernel* kernel;
        
        ParameterRamper frequencyRamper = 110;
        ParameterRamper amplitudeRamper = 0.5;
        
        enum { stageOff, stageOn, stageRelease };
        int stage = stageOff;
        
        float internalGate;
        float amp = 0;
        
        float frequency = 110;
        float amplitude = 0.5;
        
        bool started = true;
        bool resetted = false;
        
        int internalTrigger = 0;
        
//        NSBundle *frameworkBundle = [NSBundle bundleForClass:[AKOscillator class]];
//        NSString *resourcePath = [frameworkBundle resourcePath];
//        stk::Stk::setRawwavePath([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);
//        stk::Stk::setSampleRate(sampleRate);
        
        stk::Flute *flute;
        
        void init() {
            flute = new stk::Flute(100);
        }
        void clear() {
            stage = stageOff;
            amplitude = 0;
        }
        
        void start() {
            started = true;
        }
        
        void stop() {
            started = false;
        }
        
        void reset() {
            resetted = true;
        }
        
        // linked list management
        void remove() {
            if (prev) prev->next = next;
            else kernel->playingNotes = next;
            
            if (next) next->prev = prev;
            
            //prev = next = nullptr; Had to remove due to a click, potentially bad
            
            --kernel->playingNotesCount;
            delete flute;
            //            sp_osc_destroy(&osc);
        }
        void add() {
            init();
            prev = nullptr;
            next = kernel->playingNotes;
            if (next) next->prev = this;
            kernel->playingNotes = this;
            ++kernel->playingNotesCount;
        }
        
        void setFrequency(float freq) {
            frequency = freq;
            frequencyRamper.setImmediate(freq);
        }
        
        void setAmplitude(float amp) {
            amplitude = amp;
            amplitudeRamper.setImmediate(amp);
        }
        
        void controlChange(int channel, float value) {
            flute->controlChange(channel, value);
        }
        
        void trigger() {
            internalTrigger = 1;
        }
        
        void noteOn(int noteNumber, int velocity)
        {
            if (velocity == 0) {
                if (stage == stageOn) {
                    stage = stageRelease;
                    flute->noteOff(amplitude);
                    internalTrigger = 0;
                }
            } else {
                if (stage == stageOff) {
                    std::cout << "adding note\n";
                    add();
                }
                stage = stageOn;
                setFrequency(noteToHz(noteNumber));
                setAmplitude(float(velocity / 127.));
                flute->noteOn(frequency, amplitude);
                internalTrigger = 1;
            }
        }
        
        void run(int frameCount, float* outL, float* outR)
        {
            frequency = frequencyRamper.getAndStep();
            amplitude = amplitudeRamper.getAndStep();
        
            for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
                if (stage == stageOn) {
                    if (internalTrigger == 1) {
//                        flute->noteOn(frequency, amplitude);
                    }
                }
                else {
//                    flute->noteOff(amplitude);
                }
                float output = flute->tick();
                *outL++ += output;
                *outR++ += output;
            }
            if (internalTrigger == 1) {
                internalTrigger = 0;
            }
            
            if (stage == stageRelease && amplitude < 0.00001) {
                clear();
                remove();
            }
            
        }
        void controlChange(float velocity) {
            if (stage == stageOn)
                flute->controlChange(128, velocity);
        }
    };

    AKFluteInstrumentDSPKernel() {
        std::cout << "Kernel call!\n";
        noteStates.resize(128);
        for (NoteState& state : noteStates) {
            state.kernel = this;
        }
    }

    void init(int channelCount, double inSampleRate) {
        
        channels = channelCount;

        sampleRate = float(inSampleRate);
        
        // iOS Hack
        NSBundle *frameworkBundle = [NSBundle bundleForClass:[AKOscillator class]];
        NSString *resourcePath = [frameworkBundle resourcePath];
        stk::Stk::setRawwavePath([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);
        
        stk::Stk::setSampleRate(sampleRate);
//        flute = new stk::Flute(100);
    }
    
    void startNote(int note, int velocity) {
        std::cout << "start note: (" << note << "), vel: (" << velocity << ")\n";
        
        noteStates[note].noteOn(note, velocity);
    }
    
    void stopNote(int note) {
        std::cout << "stop note: (" << note << ")\n";
        noteStates[note].noteOn(note, 0);
    }

    void destroy() {
//        delete flute;
    }

    void reset() {
        for (NoteState& state : noteStates) {
            state.frequencyRamper.reset();
            state.amplitudeRamper.reset();
            state.clear();
        }
        playingNotes = nullptr;
        playingNotesCount = 0;
//        resetted = true;
    }

//    void setFrequency(float freq) {
//        frequency = freq;
//        frequencyRamper.setImmediate(freq);
//    }
//
//    void setAmplitude(float amp) {
//        amplitude = amp;
//        amplitudeRamper.setImmediate(amp);
//    }
    
    void controlChange(int note, float velocity) {
        noteStates[note].controlChange(velocity);
//        flute->controlChange(channel, value);
    }

//    void trigger() {
//        internalTrigger = 1;
//    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.setUIValue(clamp(value, (float)0, (float)22000));
                break;

            case amplitudeAddress:
                amplitudeRamper.setUIValue(clamp(value, (float)0, (float)1));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case frequencyAddress:
                return frequencyRamper.getUIValue();

            case amplitudeAddress:
                return amplitudeRamper.getUIValue();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case frequencyAddress:
                frequencyRamper.startRamp(clamp(value, (float)0, (float)22000), duration);
                break;

            case amplitudeAddress:
                amplitudeRamper.startRamp(clamp(value, (float)0, (float)1), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList *outBufferList) {
        outBufferListPtr = outBufferList;
    }
    
    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        
        float* outL = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset;
        float* outR = (float*)outBufferListPtr->mBuffers[1].mData + bufferOffset;
        
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

//    double frequencyScale = 2. * M_PI / sampleRate;
    
    AudioBufferList *outBufferListPtr = nullptr;
    
//    stk::Flute *flute;
//    float frequency = 110;
//    float amplitude = 0.5;
    
//    float cutoffFrequency = 1000;
//    float resonance = 0.5;
//    
//    float attackDuration = 0.1;
//    float decayDuration = 0.1;
//    float sustainLevel = 1.0;
//    float releaseDuration = 0.1;
//    
//    float detuningOffset = 0;
//    float detuningMultiplier = 1;
    
public:
    NoteState* playingNotes = nullptr;
    int playingNotesCount = 0;
    ParameterRamper frequencyRamper = 110;
    ParameterRamper amplitudeRamper = 0.5;
};

#endif /* AKFluteDSPKernel_hpp */
