#include "../drive.pb.h"
#include "../utils/BURT_timer.h"

#include "FastLED.h"

const int blinkInterval = 250;  // ms

constexpr uint8_t red = 2;
constexpr uint8_t blue = 3;
constexpr uint8_t green = 36;

/// The RGB LED strip that goes around the rover.
class LedStrip {
  private:
    /// Whether the LED strip is currently lit during this phase of blinking.
    ///
    /// When #shouldBlink is true, #update will alternate this value to be on and off
    /// and light the LED strip accordingly.
    
  public:
    /// The current status of the LED strip.
    DriveData data;

    /// Sets up the LED strip.
    void setup();

    /// Updates the LED strip according to the latest commands and #shouldBlink.
    void update();

    /// Turns the LED strip red.
    void red();

    /// Turns the LED strip red.
    void green();

    /// Turns the LED strip red.
    void blue();

    /// Turns the LED strip off.
    void off();

    /// Whether the LED strip should be blinking.
    bool shouldBlink = false;

    /// Handles commands to change the LED strip.
    void handleCommand(DriveCommand command);
};
