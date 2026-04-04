# STASH Sprayer Assembly

Provides functionality to physical electronics for STASH project, does the catnip spraying and button press detection. [Main project repository here](https://github.com/ethan-pt/STASH)

## What It Does

In practice, it looks for a button press, sends a POST request*, and sprays catnip, but here's a more in-depth description if you're interested :)

- Initializes hardware components
  - Servo(s)
    - Sets servo(s) to standardized neutral position upon startup
  - A button
  - LEDs
    - Indicade startup and WiFi connection status
- Connects to WiFi (skipped if wifiBypass constant = true)
  - Or at least attempt to. The WiFi antenna on this ESP32 is a little wonky
  - Exponential backoff, hardware restart after some amount of attempts
  - LEDs indicate connection status
- Waits for button press
  - actuates servo(s) immediately if wifiBypass. Otherwise it sends a request to server ([repo link here](https://github.com/ethan-pt/catnip-dispenser-api)) and activates or doesn't based on the response
- Constantly checks for WiFi drops, attempts to reconnect using same logic as initialization if drop detected

## Components Used

- Bread board
- ESP32-S3-N16R8
- LEDs
- Keyboard keyswitch
- SG90 servos
- Anker power bank
- A bunch of solderless jumper wires, many of which I've soldered

## Planned Features

For its current purposes, this aspect of the project is largely done, but here are some features I currently plan to add

- [ ] **FIGURE OUT HOW TO ATTACH THE SERVOS TO THE CATNIP BOTTLE SUCH THAT THEY SPRAY IT WHEN ACTUATED** (God I wish I had access to a 3D printer)
- [ ] Rebuild with RPi Zero 2 W/RPi camera module (might do, might not. I have other ideas for this and current hardware, so likely not but could be interesting to try, maybe measure differences)
- [ ] Train my cats to press the button when they want catnip
- [ ] Add switch to turn on/off
