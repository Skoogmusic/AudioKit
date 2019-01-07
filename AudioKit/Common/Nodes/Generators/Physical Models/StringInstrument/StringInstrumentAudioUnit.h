//
//  StringInstrumentAudioUnit.h
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef StringInstrumentAudioUnit_h
#define StringInstrumentAudioUnit_h

#import <AudioToolbox/AudioToolbox.h>

@interface StringInstrumentAudioUnit : AUAudioUnit
@property (nonatomic) float frequency;
@property (nonatomic) float amplitude;

- (void)triggerFrequency:(float)frequency amplitude:(float)amplitude;
- (void)startNote:(int)note velocity:(int)vel;
- (void)stopNote:(int)note;
- (void)start;
- (void)stop;
- (BOOL)isPlaying;
- (void)setUpParameterRamp;
- (BOOL)isSetUp;

@property double rampTime;

@end

#endif /* StringInstrumentAudioUnit_h */
