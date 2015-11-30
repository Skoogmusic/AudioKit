//
//  AKOscillator.swift
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

import AVFoundation

/** Reads from the table sequentially and repeatedly at given frequency. 
 Linear interpolation is applied for table look up from internal phase values. */
public class AKOscillator: AKOperation {

    // MARK: - Properties

    private var internalAU: AKOscillatorAudioUnit?
    private var token: AUParameterObserverToken?

    private var frequencyParameter: AUParameter?
    private var amplitudeParameter: AUParameter?

    /** Frequency in cycles per second */
    public var frequency: Float = 440 {
        didSet {
            frequencyParameter?.setValue(frequency, originator: token!)
        }
    }
    /** Amplitude of the output */
    public var amplitude: Float = 1 {
        didSet {
            amplitudeParameter?.setValue(amplitude, originator: token!)
        }
    }

    // MARK: - Initializers

    /** Initialize this oscillator operation */
    public init(
        table: AKTable = AKTable(.Sine),
        frequency: Float = 440,
        amplitude: Float = 1) {

        self.frequency = frequency
        self.amplitude = amplitude
        super.init()

        var description = AudioComponentDescription()
        description.componentType         = kAudioUnitType_Generator
        description.componentSubType      = 0x6f73636c /*'oscl'*/
        description.componentManufacturer = 0x41754b74 /*'AuKt'*/
        description.componentFlags        = 0
        description.componentFlagsMask    = 0
            
        AUAudioUnit.registerSubclass(
            AKOscillatorAudioUnit.self,
            asComponentDescription: description,
            name: "Local AKOscillator",
            version: UInt32.max)

        AVAudioUnit.instantiateWithComponentDescription(description, options: []) {
            avAudioUnit, error in

            guard let avAudioUnitGenerator = avAudioUnit else { return }

            self.output = avAudioUnitGenerator
            self.internalAU = avAudioUnitGenerator.AUAudioUnit as? AKOscillatorAudioUnit
            AKManager.sharedInstance.engine.attachNode(self.output!)
            self.internalAU?.setupTable(Int32(table.size))
            for var i = 0; i < table.size; i++ {
                self.internalAU?.setTableValue(table.values[i], atIndex: UInt32(i))
            }
        }

        guard let tree = internalAU?.parameterTree else { return }

        frequencyParameter = tree.valueForKey("frequency") as? AUParameter
        amplitudeParameter = tree.valueForKey("amplitude") as? AUParameter

        token = tree.tokenByAddingParameterObserver {
            address, value in

            dispatch_async(dispatch_get_main_queue()) {
                if address == self.frequencyParameter!.address {
                    self.frequency = value
                } else if address == self.amplitudeParameter!.address {
                    self.amplitude = value
                }
            }
        }

    }
}
