//: ## Oscillator Bank
import PlaygroundSupport
import AudioKit

let flute = AKFluteInstrument()

let trackedAmp = AKAmplitudeTracker(flute)

var velocity : Double = 0.0

var currentNote: Int = 0

AudioKit.output = trackedAmp
AudioKit.start()

class PlaygroundView: AKPlaygroundView, AKKeyboardDelegate {
    
    var keyboard: AKKeyboardView?
    
    override func setup() {
        addTitle("Flute Instrument")
        
        addSubview(AKPropertySlider(
            property: "Note On Velocity",
            format: "%0.f",
            value:  velocity, minimum: 0, maximum: 127,
            color: AKColor.blue
        ) { multiplier in
            velocity = multiplier
        })
        addSubview(AKPropertySlider(
            property: "Aftertouch",
            format: "%0.f",
            value:  velocity, minimum: 0, maximum: 127,
            color: AKColor.green
        ) { multiplier in
            flute.afterTouch(noteNumber: currentNote, velocity: velocity)
        })
        
        keyboard = AKKeyboardView(width: 440, height: 100)
        keyboard!.polyphonicMode = false
        keyboard!.delegate = self
        addSubview(keyboard!)
        
        addSubview(AKButton(title: "Go Polyphonic") {
            self.keyboard?.polyphonicMode = !self.keyboard!.polyphonicMode
            if self.keyboard!.polyphonicMode {
                return "Go Monophonic"
            } else {
                return "Go Polyphonic"
            }
        })
    }
    
    func noteOn(note: MIDINoteNumber) {
        currentNote = note
        flute.play(noteNumber: note, velocity: Int(velocity))
    }
    
    func noteOff(note: MIDINoteNumber) {
        flute.stop(noteNumber: note)
    }
}

PlaygroundPage.current.needsIndefiniteExecution = true
PlaygroundPage.current.liveView = PlaygroundView()
