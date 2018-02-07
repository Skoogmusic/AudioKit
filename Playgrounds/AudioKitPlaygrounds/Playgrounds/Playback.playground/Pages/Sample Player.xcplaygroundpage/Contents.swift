//: ## Sample Player
//: An alternative to AKSampler or AKAudioPlayer, AKSamplePlayer is a player that
//: doesn't rely on an as much Apple AV foundation/engine code as the others.
import AudioKitPlaygrounds
import AudioKit

let file = try AKAudioFile(readFileName: "mixloop.wav")

let samplePlayer = AKSamplePlayer(file: file) {
    print("Playback completed.")
}

AudioKit.output = samplePlayer
try AudioKit.start()

import AudioKitUI

class LiveView: AKLiveViewController {
    var playingPositionSlider: AKSlider?
    var current = 0
    override func viewDidLoad() {
        addTitle("Sample Player")
        
        AKPlaygroundLoop(every: 1 / 60.0) {
            if samplePlayer.endPoint > 0 {
                self.playingPositionSlider?.value = samplePlayer.normalizedPosition
            }
            
        }
        
        addView(AKButton(title: "Play") { _ in
            samplePlayer.play()
        })
        
        addView(AKButton(title: "Play Reversed") { _ in
            let start = Sample(44_100 * (self.current % 26))
            samplePlayer.play(from: start + 44_100, to: start)
        })
        
        addView(AKButton(title: "Next") { _ in
            self.current += 1
            samplePlayer.play(from: Sample(44_100 * (self.current % 26)),
                              length: Sample(44_100))
        })
        
        addView(AKButton(title: "Previous") { _ in
            self.current -= 1
            if self.current < 0 {
                self.current += 26
            }
            samplePlayer.play(from: Sample(44_100 * (self.current % 26)),
                              length: Sample(40_000))
        })
        
        addView(AKSlider(property: "Rate", value: 1, range: -1.0 ... 1.0) { sliderValue in
            samplePlayer.rate = sliderValue
        })
        
        playingPositionSlider = AKSlider(property: "Position",
                                         value: samplePlayer.normalizedPosition,
                                         range: 0 ... 1.0,
                                         format: "%0.2f s"
        ) { _ in
            // Can't do player.playhead = sliderValue
        }
        addView(playingPositionSlider!)
    }
}

import PlaygroundSupport
PlaygroundPage.current.needsIndefiniteExecution = true
PlaygroundPage.current.liveView = LiveView()

