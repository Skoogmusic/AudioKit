//: ## Flute
//: Physical model of a Flute
import AudioKitPlaygrounds
import AudioKit

let playRate = 2.0

let flute = AKFluteInstrument()

let reverb = AKReverb(flute)
var count = 0
let scale = [0, 2, 4, 5, 7, 9, 11, 12]
var frequency = 60

let performance = AKPeriodicFunction(frequency: playRate) {
    flute.stop(noteNumber: MIDINoteNumber(frequency))
    var note = scale.randomElement()
    let octave = (4..<7).randomElement() * 12
//    random(in: 0, 10)


    frequency = (scale[count] + octave)
//    if random(0, 6) > 1.0 {
    
    flute.play(noteNumber: MIDINoteNumber(frequency), velocity: 100)
    count += 1
    if count > 6 {
        count = 0
    }
//        flute.trigger(frequency: frequency, amplitude: 0.1)
//    } else {
        flute.stop()
//    }
}

AudioKit.output = flute
AudioKit.start(withPeriodicFunctions: performance)
performance.start()

import PlaygroundSupport
PlaygroundPage.current.needsIndefiniteExecution = true
