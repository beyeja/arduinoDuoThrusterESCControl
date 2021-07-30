# arduinoDuoThrusterESCControl

Arduino script to controls differential thrust of two brushless motors.

## features

- program will execute ESC arming steps on startup
- it then takes two analogue inputs, "overall throttle" and "drive direction" and translates them to differential speed of two PWM controlled ESC/Motors. Meaning the "drive direction" input will affect the difference in thrust between the left and right ESC-PWM signal. The difference will be increased up to the point where one motor will turn as fast in reverse as the other will turn forward
- it offers also a drive direction input to control general drive direction
- enable debug_output flag to see values on serial monitor; this is slow though

### example behavior

| Throttle |     Direction      | Motor Thrust Left | Motor Thrust Right |
| -------- | :----------------: | ----------------: | -----------------: |
| 50%      |  0% (= full left)  |              -50% |                50% |
| 50%      |  50% (= straight)  |               50% |                50% |
| 100      | 75% (= half right) |              100% |                 0% |

## pin-out

The pins are configured to work with Arduino Nano boards (tested with Atmega328).

see constants for pin out and adjust when desired

## dependencies

- <Servo.h> https://www.arduino.cc/reference/en/libraries/servo/
