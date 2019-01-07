//
//  AKFMOscillatorBank.swift
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

import AVFoundation

/// Frequency Modulation Polyphonic Oscillator
///
/// - Parameters:
///   - waveform:             The waveform of oscillation
///   - carrierMultiplier:    This multiplied by the baseFrequency gives the carrier frequency.
///   - modulatingMultiplier: This multiplied by the baseFrequency gives the modulating frequency.
///   - modulationIndex:      This multiplied by the modulating frequency gives the modulation amplitude.
///   - attackDuration:       Attack time
///   - decayDuration:        Decay time
///   - sustainLevel:         Sustain Level
///   - releaseDuration:      Release time
///   - detuningOffset:       Frequency offset in Hz.
///   - detuningMultiplier:   Frequency detuning multiplier
///
open class AKFMOscillatorBank2: AKPolyphonicNode, AKComponent {
    public typealias AKAudioUnitType = AKFMOscillatorBank2AudioUnit
    public static let ComponentDescription = AudioComponentDescription(generator: "fmob")
    
    // MARK: - Properties
    
    internal var internalAU: AKAudioUnitType?
    internal var token: AUParameterObserverToken?
    
    /// Waveform of the oscillator
    open var waveform: AKTable? {
        //TODO: Add error checking for table size...needs to match init()
        willSet {
            if let wf = newValue {
                for (i, sample) in wf.enumerated() {
                    self.internalAU?.setWaveformValue(sample, at: UInt32(i))
                }
            }
        }
    }
    fileprivate var carrierMultiplierParameter: AUParameter?
    fileprivate var modulatingMultiplierParameter: AUParameter?
    fileprivate var modulationIndexParameter: AUParameter?
    
    fileprivate var attackDurationParameter: AUParameter?
    fileprivate var decayDurationParameter: AUParameter?
    fileprivate var sustainLevelParameter: AUParameter?
    fileprivate var releaseDurationParameter: AUParameter?
    fileprivate var pitchBendParameter: AUParameter?
    fileprivate var vibratoDepthParameter: AUParameter?
    fileprivate var vibratoRateParameter: AUParameter?
    fileprivate var detuningOffsetParameter: AUParameter?
    fileprivate var detuningMultiplierParameter: AUParameter?
    
    /// Ramp Time represents the speed at which parameters are allowed to change
    @objc open dynamic var rampDuration: Double = AKSettings.rampDuration {
        willSet {
            internalAU?.rampDuration = newValue
        }
    }
    
    /// This multiplied by the baseFrequency gives the carrier frequency.
    @objc open dynamic var carrierMultiplier: Double = 1.0 {
        willSet {
            if carrierMultiplier != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        carrierMultiplierParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.carrierMultiplier = Float(newValue)
                }
            }
        }
    }
    
    /// This multiplied by the baseFrequency gives the modulating frequency.
    @objc open dynamic var modulatingMultiplier: Double = 1 {
        willSet {
            if modulatingMultiplier != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        modulatingMultiplierParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.modulatingMultiplier = Float(newValue)
                }
            }
        }
    }
    
    /// This multiplied by the modulating frequency gives the modulation amplitude.
    @objc open dynamic var modulationIndex: Double = 1 {
        willSet {
            if modulationIndex != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        modulationIndexParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.modulationIndex = Float(newValue)
                }
            }
        }
    }
    
    /// Attack duration in seconds
    @objc open dynamic var attackDuration: Double = 0.1 {
        willSet {
            if attackDuration != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        attackDurationParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.attackDuration = Float(newValue)
                }
            }
        }
    }
    /// Decay duration in seconds
    @objc open dynamic var decayDuration: Double = 0.1 {
        willSet {
            if decayDuration != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        decayDurationParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.decayDuration = Float(newValue)
                }
            }
        }
    }
    /// Sustain Level
    @objc open dynamic var sustainLevel: Double = 1.0 {
        willSet {
            if sustainLevel != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        sustainLevelParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.sustainLevel = Float(newValue)
                }
            }
        }
    }
    /// Release duration in seconds
    @objc open dynamic var releaseDuration: Double = 0.1 {
        willSet {
            if releaseDuration != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        releaseDurationParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.releaseDuration = Float(newValue)
                }
            }
        }
    }
    
    /// Pitch Bend as number of semitones
    @objc open dynamic var pitchBend: Double = 0 {
        willSet {
            if pitchBend != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        pitchBendParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.pitchBend = Float(newValue)
                }
            }
        }
    }
    
    /// Vibrato Depth in semitones
    @objc open dynamic var vibratoDepth: Double = 0 {
        willSet {
            if vibratoDepth != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        vibratoDepthParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.vibratoDepth = Float(newValue)
                }
            }
        }
    }
    
    /// Vibrato Rate in Hz
    @objc open dynamic var vibratoRate: Double = 0 {
        willSet {
            if vibratoRate != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        vibratoRateParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.vibratoRate = Float(newValue)
                }
            }
        }
    }
    
    /// Frequency offset in Hz.
    open var detuningOffset: Double = 0 {
        willSet {
            if detuningOffset != newValue {
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        detuningOffsetParameter?.setValue(Float(newValue), originator: existingToken)
                    }
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
                if internalAU?.isSetUp ?? false {
                    if let existingToken = token {
                        detuningMultiplierParameter?.setValue(Float(newValue), originator: existingToken)
                    }
                } else {
                    internalAU?.detuningMultiplier = Float(newValue)
                }
            }
        }
    }
    
    // MARK: - Initialization
    
    /// Initialize the oscillator with defaults
    public convenience override init() {
        self.init(waveform: AKTable(.sine))
    }
    
    /// Initialize this oscillator node
    ///
    /// - Parameters:
    ///   - waveform:             The waveform of oscillation
    ///   - carrierMultiplier:    This multiplied by the baseFrequency gives the carrier frequency.
    ///   - modulatingMultiplier: This multiplied by the baseFrequency gives the modulating frequency.
    ///   - modulationIndex:      This multiplied by the modulating frequency gives the modulation amplitude.
    ///   - attackDuration:       Attack time
    ///   - decayDuration:        Decay time
    ///   - sustainLevel:         Sustain Level
    ///   - releaseDuration:      Release time
    ///   - detuningOffset:       Frequency offset in Hz.
    ///   - detuningMultiplier:   Frequency detuning multiplier
    ///
    public init(
        waveform: AKTable,
        carrierMultiplier: Double = 1.0,
        modulatingMultiplier: Double = 1.0,
        modulationIndex: Double = 1.0,
        attackDuration: Double = 0.1,
        decayDuration: Double = 0.1,
        sustainLevel: Double = 1.0,
        releaseDuration: Double = 0.1,
        detuningOffset: Double = 0.0,
        detuningMultiplier: Double = 1.0) {
        
        
        self.waveform = waveform
        self.carrierMultiplier = carrierMultiplier
        self.modulatingMultiplier = modulatingMultiplier
        self.modulationIndex = modulationIndex
        
        self.attackDuration = attackDuration
        self.decayDuration = decayDuration
        self.sustainLevel = sustainLevel
        self.releaseDuration = releaseDuration
        self.detuningOffset = detuningOffset
        self.detuningMultiplier = detuningMultiplier
        
        _Self.register()
        
        super.init()
        AVAudioUnit.instantiate(with: _Self.ComponentDescription, options: []) {
            avAudioUnit, error in
            
            guard let avAudioUnitGenerator = avAudioUnit else { return }
            
            self.avAudioNode = avAudioUnitGenerator
            self.internalAU = avAudioUnitGenerator.auAudioUnit as? AKAudioUnitType
            
            AudioKit.engine.attach(self.avAudioNode)
            self.internalAU?.setupWaveform(Int32(waveform.count))
            for (i, sample) in waveform.enumerated() {
                self.internalAU?.setWaveformValue(sample, at: UInt32(i))
            }
        }
        
        guard let tree = internalAU?.parameterTree else { return }
        
        carrierMultiplierParameter    = tree["carrierMultiplier"]
        modulatingMultiplierParameter = tree["modulatingMultiplier"]
        modulationIndexParameter      = tree["modulationIndex"]
        
        attackDurationParameter     = tree["attackDuration"]
        decayDurationParameter      = tree["decayDuration"]
        sustainLevelParameter       = tree["sustainLevel"]
        releaseDurationParameter    = tree["releaseDuration"]
        detuningOffsetParameter     = tree["detuningOffset"]
        detuningMultiplierParameter = tree["detuningMultiplier"]
        
        token = tree.token (byAddingParameterObserver: {
            address, value in
            
            DispatchQueue.main.async {
                if address == self.carrierMultiplierParameter!.address {
                    self.carrierMultiplier = Double(value)
                } else if address == self.modulatingMultiplierParameter!.address {
                    self.modulatingMultiplier = Double(value)
                } else if address == self.modulationIndexParameter!.address {
                    self.modulationIndex = Double(value)
                } else if address == self.attackDurationParameter!.address {
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
                }
            }
        })
        
        internalAU?.carrierMultiplier = Float(carrierMultiplier)
        internalAU?.modulatingMultiplier = Float(modulatingMultiplier)
        internalAU?.modulationIndex = Float(modulationIndex)
        
        internalAU?.attackDuration = Float(attackDuration)
        internalAU?.decayDuration = Float(decayDuration)
        internalAU?.sustainLevel = Float(sustainLevel)
        internalAU?.releaseDuration = Float(releaseDuration)
        internalAU?.detuningOffset = Float(detuningOffset)
        internalAU?.detuningMultiplier = Float(detuningMultiplier)
    }
    
    // MARK: - AKPolyphonic
    
    /// Function to start, play, or activate the node, all do the same thing
    open override func play(noteNumber: MIDINoteNumber, velocity: MIDIVelocity) {
        self.internalAU!.startNote(noteNumber, velocity: velocity)
    }
    
    /// Function to stop or bypass the node, both are equivalent
    open override func stop(noteNumber: MIDINoteNumber) {
        self.internalAU!.stopNote(noteNumber)
    }
}
