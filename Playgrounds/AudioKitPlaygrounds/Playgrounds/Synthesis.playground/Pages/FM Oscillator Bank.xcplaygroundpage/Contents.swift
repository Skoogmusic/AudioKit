//: ## FM Oscillator Bank
//: Open the timeline view to use the controls this playground sets up.
//:

import AudioKitPlaygrounds
import AudioKit
import AudioKitUI

let fmBank = AKFMOscillatorBank()
var delay = AKDelay()
var delayMixer: AKDryWetMixer
var masterVolume = AKMixer()
var filter: AKMoogLadder
var trackedAmp: AKAmplitudeTracker
var tick = 0

fmBank.attackDuration = 0.01
fmBank.decayDuration = 0.01
fmBank.sustainLevel = 0.3
fmBank.releaseDuration = 0.01
fmBank.modulatingMultiplier = 4
fmBank.modulationIndex = 10

filter = AKMoogLadder(fmBank)
delay = AKDelay(filter)
delay.lowPassCutoff = 8000
delay.time = 0.4
delay.feedback = 0.4
delayMixer = AKDryWetMixer(filter, delay, balance: 0.4)
masterVolume = AKMixer(delayMixer)
trackedAmp = AKAmplitudeTracker(masterVolume)
AudioKit.output = trackedAmp
AudioKit.start()

class LiveView: AKLiveViewController, AKKeyboardDelegate {

    var keyboard: AKKeyboardView!

    override func viewDidLoad() {
        addTitle("FM Oscillator Bank")

        addView(AKSlider(property: "Carrier Multiplier",
                         value: fmBank.carrierMultiplier,
                         range: 0 ... 2
        ) { multiplier in
            fmBank.carrierMultiplier = multiplier
        })

        addView(AKSlider(property: "Modulating Multiplier",
                         value: fmBank.modulatingMultiplier,
                         range: 0 ... 4
        ) { multiplier in
            fmBank.modulatingMultiplier = multiplier
        })

        addView(AKSlider(property: "Modulation Index",
                         value: fmBank.modulationIndex,
                         range: 0 ... 20
        ) { index in
            fmBank.modulationIndex = index
        })

        let adsrView = AKADSRView { att, dec, sus, rel in
            fmBank.attackDuration = att
            fmBank.decayDuration = dec
            fmBank.sustainLevel = sus
            fmBank.releaseDuration = rel
        }
        adsrView.attackDuration = fmBank.attackDuration
        adsrView.decayDuration = fmBank.decayDuration
        adsrView.releaseDuration = fmBank.releaseDuration
        adsrView.sustainLevel = fmBank.sustainLevel
        addView(adsrView)

        addView(AKSlider(property: "Pitch Bend",
                         value: fmBank.pitchBend,
                         range: -12 ... 12,
                         format: "%0.2f semitones"
        ) { sliderValue in
            fmBank.pitchBend = sliderValue
        })

        addView(AKSlider(property: "Vibrato Depth",
                         value: fmBank.vibratoDepth,
                         range: 0 ... 2,
                         format: "%0.2f semitones"
        ) { sliderValue in
            fmBank.vibratoDepth = sliderValue
        })

        addView(AKSlider(property: "Vibrato Rate",
                         value: fmBank.vibratoRate,
                         range: 0 ... 1,
                         format: "%.2f"
        ) { sliderValue in
            
            tick = tick + 1
            
            if (tick <= 20){
                fmBank.modulatingMultiplier = 0.5 + Double(tick) * 0.025
            }
            else {
                fmBank.modulatingMultiplier = 1.0 + 0.05 * sliderValue * sin(Double(tick) * 0.14 * sliderValue * Double.pi)
            }
            filter.cutoffFrequency = 20 + (sliderValue * (10000))
            filter.resonance = 0.4 + (sliderValue * 0.3)
        })

        keyboard = AKKeyboardView(width: 440, height: 100)
        keyboard.polyphonicMode = false
        keyboard.delegate = self
        addView(keyboard)

        addView(AKButton(title: "Go Polyphonic") { button in
            self.keyboard.polyphonicMode = !self.keyboard.polyphonicMode
            if self.keyboard.polyphonicMode {
                button.title = "Go Monophonic"
            } else {
                button.title = "Go Polyphonic"
            }
        })
    }

    func noteOn(note: MIDINoteNumber) {
         tick = 0
        fmBank.play(noteNumber: note, velocity: 80)
    }

    func noteOff(note: MIDINoteNumber) {
         tick = 0
        fmBank.stop(noteNumber: note)
    }
}

import PlaygroundSupport
PlaygroundPage.current.needsIndefiniteExecution = true
PlaygroundPage.current.liveView = LiveView()
