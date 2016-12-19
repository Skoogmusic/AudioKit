//
//  AKFluteInstrumentAudioUnit.h
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef AKFluteInstrumentAudioUnit_h
#define AKFluteInstrumentAudioUnit_h

#import <AudioToolbox/AudioToolbox.h>

@interface AKFluteInstrumentAudioUnit : AUAudioUnit
@property (nonatomic) float frequency;
@property (nonatomic) float amplitude;

- (void)triggerFrequency:(float)frequency amplitude:(float)amplitude;
- (void)controlChange:(int)channel value:(int)value;
- (void)startNote:(int)note velocity:(int)vel;
- (void)stopNote:(int)note;
- (void)start;
- (void)stop;
- (BOOL)isPlaying;
- (void)setUpParameterRamp;
- (BOOL)isSetUp;

@property double rampTime;

@end

#endif /* AKFluteInstrumentAudioUnit_h */
