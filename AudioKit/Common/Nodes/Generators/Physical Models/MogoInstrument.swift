//
//  MogoInstrument.swift
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

import AVFoundation

/// Reads from the table sequentially and repeatedly at given frequency. Linear
/// interpolation is applied for table look up from internal phase values.
///
/// - Parameters:
///   - waveform:  The waveform of oscillation
///   - attackDuration: Attack time
///   - decayDuration: Decay time
///   - sustainLevel: Sustain Level
///   - releaseDuration: Release time
///   - detuningOffset: Frequency offset in Hz.
///   - detuningMultiplier: Frequency detuning multiplier
///
open class MogoInstrument: AKPolyphonicNode, AKComponent {
    public typealias AKAudioUnitType = MogoInstrumentAudioUnit
    static let ComponentDescription = AudioComponentDescription(generator: "mogo")

    // MARK: - Properties

    internal var internalAU: AKAudioUnitType?
    internal var token: AUParameterObserverToken?

//    fileprivate var waveform: AKTable?

    fileprivate var attackDurationParameter: AUParameter?
    fileprivate var decayDurationParameter: AUParameter?
    fileprivate var sustainLevelParameter: AUParameter?
    fileprivate var releaseDurationParameter: AUParameter?
    fileprivate var detuningOffsetParameter: AUParameter?
    fileprivate var detuningMultiplierParameter: AUParameter?
    fileprivate var cutoffFrequencyParameter: AUParameter?
    fileprivate var resonanceParameter: AUParameter?

    /// Ramp Time represents the speed at which parameters are allowed to change
    open var rampTime: Double = AKSettings.rampTime {
        willSet {
            if rampTime != newValue {
                internalAU?.rampTime = newValue
                internalAU?.setUpParameterRamp()
            }
        }
    }

    /// Attack time
    open var attackDuration: Double = 0.1 {
        willSet {
            if attackDuration != newValue {
                if internalAU!.isSetUp() {
                    attackDurationParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.attackDuration = Float(newValue)
                }
            }
        }
    }
    /// Decay time
    open var decayDuration: Double = 0.1 {
        willSet {
            if decayDuration != newValue {
                if internalAU!.isSetUp() {
                    decayDurationParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.decayDuration = Float(newValue)
                }
            }
        }
    }
    /// Sustain Level
    open var sustainLevel: Double = 1.0 {
        willSet {
            if sustainLevel != newValue {
                if internalAU!.isSetUp() {
                    sustainLevelParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.sustainLevel = Float(newValue)
                }
            }
        }
    }
    /// Release time
    open var releaseDuration: Double = 0.1 {
        willSet {
            if releaseDuration != newValue {
                if internalAU!.isSetUp() {
                    releaseDurationParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.releaseDuration = Float(newValue)
                }
            }
        }
    }

    /// Frequency offset in Hz.
    open var detuningOffset: Double = 0 {
        willSet {
            if detuningOffset != newValue {
                if internalAU!.isSetUp() {
                    detuningOffsetParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.detuningOffset = Float(newValue)
                }
            }
        }
    }

    /// Frequency detuning multiplier
    open var detuningMultiplier: Double = 1 {
        willSet {
            if detuningMultiplier != newValue {
                if internalAU!.isSetUp() {
                    detuningMultiplierParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.detuningMultiplier = Float(newValue)
                }
            }
        }
    }
    
    /// Filter cutoff frequency.
    open var cutoffFrequency: Double = 1000 {
        willSet {
            if cutoffFrequency != newValue {
                if internalAU!.isSetUp() {
                    cutoffFrequencyParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.cutoffFrequency = Float(newValue)
                }
            }
        }
    }
    /// Resonance, generally < 1, but not limited to it. Higher than 1 resonance values might cause aliasing, analogue synths generally allow resonances to be above 1.
    open var resonance: Double = 0.5 {
        willSet {
            if resonance != newValue {
                if internalAU!.isSetUp() {
                    resonanceParameter?.setValue(Float(newValue), originator: token!)
                } else {
                    internalAU?.resonance = Float(newValue)
                }
            }
        }
    }

    // MARK: - Initialization
    
    /// Initialize the oscillator with defaults
    public convenience override init() {
        print("Init of mogo Instrument")
        self.init()
    }

    /// Initialize this oscillator node
    ///
    /// - Parameters:
    ///   - waveform:  The waveform of oscillation
    ///   - attackDuration: Attack time
    ///   - decayDuration: Decay time
    ///   - sustainLevel: Sustain Level
    ///   - releaseDuration: Release time
    ///   - detuningOffset: Frequency offset in Hz.
    ///   - detuningMultiplier: Frequency detuning multiplier
    ///
    public init(
//        waveform: AKTable,
        attackDuration: Double = 0.1,
        decayDuration: Double = 0.1,
        sustainLevel: Double = 1.0,
        releaseDuration: Double = 0.1,
        detuningOffset: Double = 0,
        detuningMultiplier: Double = 1,
        cutoffFrequency: Double = 1000,
        resonance: Double = 0.5) {

print("Second Init of mogo Instrument")
//        self.waveform = waveform
        self.attackDuration = attackDuration
        self.decayDuration = decayDuration
        self.sustainLevel = sustainLevel
        self.releaseDuration = releaseDuration
        self.detuningOffset = detuningOffset
        self.detuningMultiplier = detuningMultiplier
        self.cutoffFrequency = cutoffFrequency
        self.resonance = resonance

        _Self.register()

        super.init()
        AVAudioUnit.instantiate(with: _Self.ComponentDescription, options: []) {
            avAudioUnit, error in

            guard let avAudioUnitGenerator = avAudioUnit else { return }

            self.avAudioNode = avAudioUnitGenerator
            self.internalAU = avAudioUnitGenerator.auAudioUnit as? AKAudioUnitType

            AudioKit.engine.attach(self.avAudioNode)
//            self.internalAU?.setupWaveform(Int32(waveform.count))
//            for (i, sample) in waveform.enumerated() {
//                self.internalAU?.setWaveformValue(sample, at: UInt32(i))
//            }
        }

        guard let tree = internalAU?.parameterTree else { return }

        attackDurationParameter     = tree["attackDuration"]
        decayDurationParameter      = tree["decayDuration"]
        sustainLevelParameter       = tree["sustainLevel"]
        releaseDurationParameter    = tree["releaseDuration"]
        detuningOffsetParameter     = tree["detuningOffset"]
        detuningMultiplierParameter = tree["detuningMultiplier"]
        cutoffFrequencyParameter    = tree["cutoffFrequency"]
        resonanceParameter          = tree["resonance"]

        token = tree.token (byAddingParameterObserver: {
            address, value in

            DispatchQueue.main.async {
                if address == self.attackDurationParameter!.address {
                    self.attackDuration = Double(value)
                } else if address == self.decayDurationParameter!.address {
                    self.decayDuration = Double(value)
                } else if address == self.sustainLevelParameter!.address {
                    self.sustainLevel = Double(value)
                } else if address == self.releaseDurationParameter!.address {
                    self.releaseDuration = Double(value)
                } else if address == self.detuningOffsetParameter!.address {
                    self.detuningOffset = Double(value)
                } else if address == self.detuningMultiplierParameter!.address {
                    self.detuningMultiplier = Double(value)
                } else if address == self.cutoffFrequencyParameter!.address {
                    self.cutoffFrequency = Double(value)
                } else if address == self.resonanceParameter!.address {
                    self.resonance = Double(value)
                }
            }
        })
        internalAU?.attackDuration = Float(attackDuration)
        internalAU?.decayDuration = Float(decayDuration)
        internalAU?.sustainLevel = Float(sustainLevel)
        internalAU?.releaseDuration = Float(releaseDuration)
        internalAU?.detuningOffset = Float(detuningOffset)
        internalAU?.detuningMultiplier = Float(detuningMultiplier)
        internalAU?.cutoffFrequency = Float(cutoffFrequency)
        internalAU?.resonance = Float(resonance)
    }

    // MARK: - AKPolyphonic

    /// Function to start, play, or activate the node, all do the same thing
    open override func play(noteNumber: MIDINoteNumber, velocity: MIDIVelocity) {
        internalAU?.setResonance(Int32(noteNumber), velocity: Float(velocity) / 128.0)
        internalAU?.setCutoffFrequency(Int32(noteNumber), velocity: Float(velocity) / 128.0)
        self.internalAU!.startNote(Int32(noteNumber), velocity: Int32(velocity))
    }

    /// Function to stop or bypass the node, both are equivalent
    open override func stop(noteNumber: MIDINoteNumber) {
        self.internalAU!.stopNote(Int32(noteNumber))
    }
    
    open func afterTouch(noteNumber: MIDINoteNumber, velocity: Double) {
//        internalAU?.setResonance(Int32(noteNumber), velocity: Float(velocity))
        internalAU?.setCutoffFrequencySustain(Int32(noteNumber), velocity: Float(velocity))
    }
}
