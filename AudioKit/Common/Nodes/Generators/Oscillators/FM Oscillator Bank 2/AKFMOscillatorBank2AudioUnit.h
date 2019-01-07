//
//  AKFMOscillatorBank2AudioUnit.h
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#pragma once

#import "AKBankAudioUnit.h"

@interface AKFMOscillatorBank2AudioUnit : AKBankAudioUnit

@property (nonatomic) float carrierMultiplier;
@property (nonatomic) float modulatingMultiplier;
@property (nonatomic) float modulationIndex;
@property (nonatomic) float detuningOffset;
@property (nonatomic) float detuningMultiplier;

- (void)setupWaveform:(int)size;
- (void)setWaveformValue:(float)value atIndex:(UInt32)index;
//- (void)startNote:(int)note velocity:(int)velocity;
//- (void)stopNote:(int)note;
//- (void)setUpParameterRamp;
- (BOOL)isSetUp;

@end
