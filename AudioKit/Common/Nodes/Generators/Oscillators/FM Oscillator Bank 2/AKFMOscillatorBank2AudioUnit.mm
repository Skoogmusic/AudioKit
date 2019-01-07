//
//  AKFMOscillatorBankAudioUnit.mm
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#import "AKFMOscillatorBank2AudioUnit.h"
#import "AKFMOscillatorBank2DSPKernel.hpp"

#import "BufferedAudioBus.hpp"

#import <AudioKit/AudioKit-Swift.h>

//@property AUAudioUnitBus *outputBus;
//
//@property AUAudioUnitBusArray *outputBusArray;

//@property (nonatomic, readwrite) AUParameterTree *parameterTree;
//
//@end

@implementation AKFMOscillatorBank2AudioUnit {
    // C++ members need to be ivars; they would be copied on access if they were properties.
    AKFMOscillatorBank2DSPKernel _kernel;
//    BufferedInputBus _inputBus;
    BufferedOutputBus _outputBusBuffer;
}
@synthesize parameterTree = _parameterTree;

- (void)setCarrierMultiplier:(float)carrierMultiplier {
    _kernel.setCarrierMultiplier(carrierMultiplier);
}
- (void)setModulatingMultiplier:(float)modulatingMultiplier {
    _kernel.setModulatingMultiplier(modulatingMultiplier);
}
- (void)setModulationIndex:(float)modulationIndex {
    _kernel.setModulationIndex(modulationIndex);
}

- (void)setDetuningOffset:(float)detuningOffset {
    _kernel.setDetuningOffset(detuningOffset);
}
- (void)setDetuningMultiplier:(float)detuningMultiplier {
    _kernel.setDetuningMultiplier(detuningMultiplier);
}

standardBankFunctions()

- (void)setupWaveform:(int)size {
    _kernel.setupWaveform((uint32_t)size);
}
- (void)setWaveformValue:(float)value atIndex:(UInt32)index; {
    _kernel.setWaveformValue(index, value);
}

- (void)createParameters {
    
    standardGeneratorSetup(FMOscillatorBank2)
    standardBankParameters()
    
    // Create a parameter object for the carrier multiplier.
    AUParameter *carrierMultiplierAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"carrierMultiplier"
                                              name:@"Carrier Multiplier"
                                           address:carrierMultiplierAddress
                                               min:0.0
                                               max:1000.0
                                              unit:kAudioUnitParameterUnit_Generic
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];
    
    // Create a parameter object for the modulating multiplier.
    AUParameter *modulatingMultiplierAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"modulatingMultiplier"
                                              name:@"Modulating Multiplier"
                                           address:modulatingMultiplierAddress
                                               min:0.0
                                               max:1000.0
                                              unit:kAudioUnitParameterUnit_Generic
                                          unitName:nil
                                             flags:0
                                      valueStrings:nil
                               dependentParameters:nil];
    
    // Create a parameter object for the modulation index.
    AUParameter *modulationIndexAUParameter =
    [AUParameterTree createParameterWithIdentifier:@"modulationIndex"
                                              name:@"Modulation Index"
                                           address:modulationIndexAddress
                                               min:0.0
                                               max:1000.0
                                              unit:kAudioUnitParameterUnit_Generic
                                          unitName:nil
                                             flags:0
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
    
    // Initialize the parameter values.
    carrierMultiplierAUParameter.value = 1.0;
    modulatingMultiplierAUParameter.value = 1;
    modulationIndexAUParameter.value = 1;
    
    _kernel.setParameter(carrierMultiplierAddress,    carrierMultiplierAUParameter.value);
    _kernel.setParameter(modulatingMultiplierAddress, modulatingMultiplierAUParameter.value);
    _kernel.setParameter(modulationIndexAddress,      modulationIndexAUParameter.value);
    _kernel.setParameter(detuningOffsetAddress,     detuningOffsetAUParameter.value);
    _kernel.setParameter(detuningMultiplierAddress, detuningMultiplierAUParameter.value);
    
    // Create the parameter tree.
    _parameterTree = [AUParameterTree createTreeWithChildren:@[
                                                               standardBankAUParameterList(),
                                                               carrierMultiplierAUParameter,
                                                               modulatingMultiplierAUParameter,
                                                               modulationIndexAUParameter,
                                                               detuningOffsetAUParameter,
                                                               detuningMultiplierAUParameter
                                                               ]];
    parameterTreeBlock(FMOscillatorBank2)
}

AUAudioUnitGeneratorOverrides(FMOscillatorBank2)

@end
