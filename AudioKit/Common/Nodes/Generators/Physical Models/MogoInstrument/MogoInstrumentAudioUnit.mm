//
//  MogoInstrumentAudioUnit.mm
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#import "MogoInstrumentAudioUnit.h"
#import "MogoInstrumentDSPKernel.hpp"

#import <AVFoundation/AVFoundation.h>
#import "BufferedAudioBus.hpp"

#import <AudioKit/AudioKit-Swift.h>

@interface MogoInstrumentAudioUnit()

@property AUAudioUnitBus *outputBus;

@property AUAudioUnitBusArray *outputBusArray;

@property (nonatomic, readwrite) AUParameterTree *parameterTree;

@end

@implementation MogoInstrumentAudioUnit {
    // C++ members need to be ivars; they would be copied on access if they were properties.
    MogoInstrumentDSPKernel _kernel;

    BufferedInputBus _inputBus;
}
@synthesize parameterTree = _parameterTree;

//- (void)setCutoffFrequency:(float)cutoffFrequency {
//    _kernel.setCutoffFrequency(cutoffFrequency);
//}
//- (void)setResonance:(float)resonance {
//    _kernel.setResonance(resonance);
//}

- (void)setAttackDuration:(float)attackDuration {
    _kernel.setAttackDuration(attackDuration);
}
- (void)setDecayDuration:(float)decayDuration {
    _kernel.setDecayDuration(decayDuration);
}
- (void)setSustainLevel:(float)sustainLevel {
    _kernel.setSustainLevel(sustainLevel);
}
- (void)setReleaseDuration:(float)releaseDuration {
    _kernel.setReleaseDuration(releaseDuration);
}
- (void)setDetuningOffset:(float)detuningOffset {
    _kernel.setDetuningOffset(detuningOffset);
}
- (void)setDetuningMultiplier:(float)detuningMultiplier {
    _kernel.setDetuningMultiplier(detuningMultiplier);
}

//- (void)setupWaveform:(int)size {
//    _kernel.setupWaveform((uint32_t)size);
//}
//- (void)setWaveformValue:(float)value atIndex:(UInt32)index; {
//    _kernel.setWaveformValue(index, value);
//}

- (void)setResonance:(int)note velocity:(float)velocity {
//    NSLog(@"set resonance side %d, amount %f", note, velocity);
    _kernel.setResonance(note, velocity);
}

- (void)setCutoffFrequency:(int)note velocity:(float)velocity {
//    NSLog(@"set cutofffrequency side %d, amount %f", note, velocity);
    _kernel.setCutoffFrequency(note, velocity);
}

- (void)setCutoffFrequencySustain:(int)note velocity:(float)velocity {
    _kernel.setCutoffFrequencySustain(note, velocity);
}

- (void)startNote:(int)note velocity:(int)velocity {
    _kernel.startNote(note, velocity);
}

- (void)stopNote:(int)note {
    _kernel.stopNote(note);
}

- (BOOL)isPlaying {
    return _kernel.started;
}

- (BOOL)isSetUp {
    return _kernel.resetted;
}

- (instancetype)initWithComponentDescription:(AudioComponentDescription)componentDescription
                                     options:(AudioComponentInstantiationOptions)options
                                       error:(NSError **)outError {
    self = [super initWithComponentDescription:componentDescription options:options error:outError];

    if (self == nil) {
        return nil;
    }

    // Initialize a default format for the busses.
    AVAudioFormat *defaultFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:AKSettings.sampleRate
                                                                                  channels:AKSettings.channelCount];

    // Create a DSP kernel to handle the signal processing.
    _kernel.init(defaultFormat.channelCount, defaultFormat.sampleRate);

    
    AudioUnitParameterOptions flags = kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable | kAudioUnitParameterFlag_DisplayLogarithmic;

    // Create a parameter object for the attackDuration.
    AUParameter *attackDurationAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"attackDuration"
                                              name:@"Attack time"
                                           address:attackDurationAddress
                                               min:0
                                               max:99
                                              unit:kAudioUnitParameterUnit_Seconds
                                          unitName:nil
                                             flags:flags
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the decayDuration.
    AUParameter *decayDurationAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"decayDuration"
                                              name:@"Decay time"
                                           address:decayDurationAddress
                                               min:0
                                               max:99
                                              unit:kAudioUnitParameterUnit_Seconds
                                          unitName:nil
                                             flags:flags
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the sustainLevel.
    AUParameter *sustainLevelAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"sustainLevel"
                                              name:@"Sustain Level"
                                           address:sustainLevelAddress
                                               min:0
                                               max:99
                                              unit:kAudioUnitParameterUnit_Generic
                                          unitName:nil
                                             flags:flags
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the releaseDuration.
    AUParameter *releaseDurationAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"releaseDuration"
                                              name:@"Release time"
                                           address:releaseDurationAddress
                                               min:0
                                               max:99
                                              unit:kAudioUnitParameterUnit_Seconds
                                          unitName:nil
                                             flags:flags
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the detuningOffset.
    AUParameter *detuningOffsetAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"detuningOffset"
                                              name:@"Frequency offset (Hz)"
                                           address:detuningOffsetAddress
                                               min:-1000
                                               max:1000
                                              unit:kAudioUnitParameterUnit_Hertz
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the detuningMultiplier.
    AUParameter *detuningMultiplierAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"detuningMultiplier"
                                              name:@"Frequency detuning multiplier"
                                           address:detuningMultiplierAddress
                                               min:0.5
                                               max:2.0
                                              unit:kAudioUnitParameterUnit_Generic
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the cutoffFrequency.
    AUParameter *cutoffFrequencyAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"cutoffFrequency"
                                              name:@"Cutoff Frequency (Hz)"
                                           address:cutoffFrequencyAddress
                                               min:12.0
                                               max:20000.0
                                              unit:kAudioUnitParameterUnit_Hertz
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];
    // Create a parameter object for the resonance.
    AUParameter *resonanceAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"resonance"
                                              name:@"Resonance (%)"
                                           address:resonanceAddress
                                               min:0.0
                                               max:2.0
                                              unit:kAudioUnitParameterUnit_Percent
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];

    // Initialize the parameter values.
    attackDurationAUParameter.value = 0.1;
    decayDurationAUParameter.value = 0.1;
    sustainLevelAUParameter.value = 1.0;
    releaseDurationAUParameter.value = 0.1;
    detuningOffsetAUParameter.value = 0;
    detuningMultiplierAUParameter.value = 1;
    cutoffFrequencyAUParameter.value = 1000;
    resonanceAUParameter.value = 0.5;
    
    _rampTime = AKSettings.rampDuration;

    _kernel.setParameter(attackDurationAddress,  attackDurationAUParameter.value);
    _kernel.setParameter(decayDurationAddress,   decayDurationAUParameter.value);
    _kernel.setParameter(sustainLevelAddress,    sustainLevelAUParameter.value);
    _kernel.setParameter(releaseDurationAddress, releaseDurationAUParameter.value);
    _kernel.setParameter(detuningOffsetAddress,     detuningOffsetAUParameter.value);
    _kernel.setParameter(detuningMultiplierAddress, detuningMultiplierAUParameter.value);
    _kernel.setParameter(cutoffFrequencyAddress, cutoffFrequencyAUParameter.value);
    _kernel.setParameter(resonanceAddress,       resonanceAUParameter.value);

    // Create the parameter tree.
    _parameterTree = [AUParameterTree createTreeWithChildren:@[
        attackDurationAUParameter,
        decayDurationAUParameter,
        sustainLevelAUParameter,
        releaseDurationAUParameter,
        detuningOffsetAUParameter,
        detuningMultiplierAUParameter,
        cutoffFrequencyAUParameter,
        resonanceAUParameter
    ]];

    // Create the input and output busses.
    _inputBus.init(defaultFormat, 8);
    _outputBus = [[AUAudioUnitBus alloc] initWithFormat:defaultFormat error:nil];

    // Create the output bus array.
    _outputBusArray = [[AUAudioUnitBusArray alloc] initWithAudioUnit:self
                                                             busType:AUAudioUnitBusTypeOutput
                                                              busses: @[_outputBus]];

    // Make a local pointer to the kernel to avoid capturing self.
    __block MogoInstrumentDSPKernel *oscillatorKernel = &_kernel;

    // implementorValueObserver is called when a parameter changes value.
    _parameterTree.implementorValueObserver = ^(AUParameter *param, AUValue value) {
        oscillatorKernel->setParameter(param.address, value);
    };

    // implementorValueProvider is called when the value needs to be refreshed.
    _parameterTree.implementorValueProvider = ^(AUParameter *param) {
        return oscillatorKernel->getParameter(param.address);
    };

    self.maximumFramesToRender = 512;

    return self;
}

#pragma mark - AUAudioUnit Overrides

- (AUAudioUnitBusArray *)outputBusses {
    return _outputBusArray;
}

- (BOOL)allocateRenderResourcesAndReturnError:(NSError **)outError {
    if (![super allocateRenderResourcesAndReturnError:outError]) {
        return NO;
    }
    _inputBus.allocateRenderResources(self.maximumFramesToRender);

    _kernel.init(self.outputBus.format.channelCount, self.outputBus.format.sampleRate);
    _kernel.reset();

    [self setUpParameterRamp];
    
    return YES;
}

- (void)setUpParameterRamp {
    /*
     While rendering, we want to schedule all parameter changes. Setting them
     off the render thread is not thread safe.
     */
    __block AUScheduleParameterBlock scheduleParameter = self.scheduleParameterBlock;
    
    // Ramp over rampTime in seconds.
    __block AUAudioFrameCount rampTime = AUAudioFrameCount(_rampTime * self.outputBus.format.sampleRate);
    
    self.parameterTree.implementorValueObserver = ^(AUParameter *param, AUValue value) {
        scheduleParameter(AUEventSampleTimeImmediate, rampTime, param.address, value);
    };
}

- (void)deallocateRenderResources {
    [super deallocateRenderResources];
    _kernel.destroy();

    _inputBus.deallocateRenderResources();
}

- (AUInternalRenderBlock)internalRenderBlock {
    /*
     Capture in locals to avoid ObjC member lookups. If "self" is captured in
     render, we're doing it wrong.
     */
    __block MogoInstrumentDSPKernel *state = &_kernel;
    __block BufferedInputBus *input = &_inputBus;

    return ^AUAudioUnitStatus(
                              AudioUnitRenderActionFlags *actionFlags,
                              const AudioTimeStamp       *timestamp,
                              AVAudioFrameCount           frameCount,
                              NSInteger                   outputBusNumber,
                              AudioBufferList            *outputData,
                              const AURenderEvent        *realtimeEventListHead,
                              AURenderPullInputBlock      pullInputBlock) {

        AudioBufferList *inAudioBufferList = input->mutableAudioBufferList;

        /*
         If the caller passed non-nil output pointers, use those. Otherwise,
         process in-place in the input buffer. If your algorithm cannot process
         in-place, then you will need to preallocate an output buffer and use
         it here.
         */
        AudioBufferList *outAudioBufferList = outputData;
        if (outAudioBufferList->mBuffers[0].mData == nullptr) {
            for (UInt32 i = 0; i < outAudioBufferList->mNumberBuffers; ++i) {
                outAudioBufferList->mBuffers[i].mData = inAudioBufferList->mBuffers[i].mData;
            }
        }

        state->setBuffer(outAudioBufferList);
        state->processWithEvents(timestamp, frameCount, realtimeEventListHead);

        return noErr;
    };
}


@end


