//
//  MogoInstrumentAudioUnit.h
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef MogoInstrumentAudioUnit_h
#define MogoInstrumentAudioUnit_h

#import <AudioToolbox/AudioToolbox.h>

@interface MogoInstrumentAudioUnit : AUAudioUnit
@property (nonatomic) float attackDuration;
@property (nonatomic) float decayDuration;
@property (nonatomic) float sustainLevel;
@property (nonatomic) float releaseDuration;
@property (nonatomic) float detuningOffset;
@property (nonatomic) float detuningMultiplier;
@property (nonatomic) float cutoffFrequency;
@property (nonatomic) float resonance;

//- (void)setupWaveform:(int)size;
//- (void)setWaveformValue:(float)value atIndex:(UInt32)index;
- (void)startNote:(int)note velocity:(int)velocity;
- (void)stopNote:(int)note;
- (void)setResonance:(int)note velocity:(float)velocity;
- (void)setCutoffFrequency:(int)note velocity:(float)velocity;
- (void)setCutoffFrequencySustain:(int)note velocity:(float)velocity;
- (BOOL)isPlaying;
- (void)setUpParameterRamp;
- (BOOL)isSetUp;

@property double rampTime;

@end

#endif /* MogoInstrumentAudioUnit_h */
