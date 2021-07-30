#include <Servo.h>

/* 
	Double Thruster ESC Control for Ship steering
*/

#define DEBUG_OUTPUT 0

// pin settings
#define TRHUSTER_LEFT_PIN 9 	// D9
#define TRHUSTER_RIGHT_PIN 8	// D8
#define DRIVE_DIRECTION_PIN 2	// D2
#define POT_1_PIN 0				// A0
#define POT_2_PIN 1				// A1

// max min values
#define PWM_FWD_MAX 180
#define PWM_STOP 90
#define PWM_BCK_MAX 0
#define PWM_FULLSTOP_OFFSET 85
// defines reverse-direction ragen used for full stop

#define ANALOG_MAX 1022
#define ANALOG_MIN 2
#define ANALOG_STEER_MAX 511
#define ANALOG_STEER_MIN -512

#define THRUSTER_LEFT 1
#define THRUSTER_RIGHT 2

Servo thrusterLeft;
Servo thrusterRight;

void setup() {
	blink();

	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(DRIVE_DIRECTION_PIN, INPUT);

	// attaches the servos to pins
	thrusterLeft.attach(TRHUSTER_LEFT_PIN, 1000, 2000);
	thrusterRight.attach(TRHUSTER_RIGHT_PIN, 1000, 2000);
	
	Serial.println("#### inizialized");
	blink();
	
	armESCs();
	blink();

	blink();
	blink();
	blink();
}

void blink() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
}

void armESCs() {
	Serial.println("#### arming ESCs");
	
	delay(300);
	updateSpeed(PWM_FWD_MAX, PWM_FWD_MAX);
	delay(200);
	updateSpeed(PWM_BCK_MAX, PWM_BCK_MAX);
	delay(200);
	updateSpeed(PWM_STOP, PWM_STOP);

	Serial.println("#### ESCs schould be armed and set to stop");
}

/** 
 * set new speed of thrusters 
 */
void updateSpeed(int thrustLeft, int thrustRight) {
	digitalWrite(LED_BUILTIN, HIGH);

	if (thrustLeft >= PWM_STOP)
		thrustLeft = map(thrustLeft, PWM_STOP, PWM_FWD_MAX, PWM_FULLSTOP_OFFSET, PWM_FWD_MAX);
	if (thrustRight >= PWM_STOP)
		thrustRight = map(thrustRight, PWM_STOP, PWM_FWD_MAX, PWM_FULLSTOP_OFFSET, PWM_FWD_MAX);
	printThrustSettings(thrustLeft, thrustRight);
	thrusterLeft.write(thrustLeft);
	thrusterRight.write(thrustRight);
	
	digitalWrite(LED_BUILTIN, LOW);
}

int oldTValueLeft = 0;
int oldTValueRight = 0;
int maxBars = 50;
/** Print debug thrust values as bars in serial Monitor */
void printThrustSettings(int thrustLeft, int thrustRight) {
	if (DEBUG_OUTPUT != 1) return;

	int barValLeft = map(thrustLeft, PWM_BCK_MAX, PWM_FWD_MAX, 0, maxBars);
	int barValRight = map(thrustRight, PWM_BCK_MAX, PWM_FWD_MAX, 0, maxBars);
	
	if ((barValLeft != oldTValueLeft) || (barValRight != oldTValueRight)) {
		String sliderDebugOut = "L[";
	
		// create thrust bar-graphics for left thruster
		for (int i = 0; i < maxBars; i++) {
			if (i == maxBars / 2) sliderDebugOut.concat("║");
			sliderDebugOut.concat(i >= maxBars / 2 && i < barValLeft 
					|| i < maxBars / 2 && i > barValLeft 
						? "#" : " ");
		}
		// create thrust bar-graphics for right thruster
		sliderDebugOut.concat("] R[");
		for (int i = 0; i < maxBars; i++) {
			if (i == maxBars / 2) sliderDebugOut.concat("║");
			sliderDebugOut.concat(i >= maxBars / 2 && i < barValRight 
					|| i < maxBars / 2 && i > barValRight 
						? "#" : " ");
		}
		sliderDebugOut.concat("]");
		
		// add thruster values
		sliderDebugOut.concat(thrustLeft);
		sliderDebugOut.concat("|");
		sliderDebugOut.concat(thrustRight);
		Serial.println(sliderDebugOut);

		// remember values to only print when changed
		oldTValueLeft = barValLeft;
		oldTValueRight = barValRight;
	}
}

void loop() {
	boolean driveDirForward = digitalRead(DRIVE_DIRECTION_PIN) == HIGH;
	int steering = analogRead(POT_1_PIN);
	int thrust = analogRead(POT_2_PIN);
	// adapt value to steering direction
	steering = map(steering, ANALOG_MIN, ANALOG_MAX, ANALOG_STEER_MIN, ANALOG_STEER_MAX);
	double steeringFactor = (double) abs(steering) / ANALOG_STEER_MAX;

	int thrustLeft = thrust;
	int thrustRight = thrust;
	
	// apply steering relation between left and right thruster
	if (steering < 0) {
		thrustLeft = thrustLeft - thrust * steeringFactor * 2;
	}
	if (steering > 0) {
		thrustRight = thrustRight - thrust * steeringFactor * 2;
	}

	// write either forward or backward values to thrusters
	if (driveDirForward) {
		thrustLeft = map(thrustLeft, -ANALOG_MAX, ANALOG_MAX, PWM_BCK_MAX, PWM_FWD_MAX);
		thrustRight = map(thrustRight, -ANALOG_MAX, ANALOG_MAX, PWM_BCK_MAX, PWM_FWD_MAX);
		updateSpeed(thrustLeft, thrustRight);
	} else {
		thrustLeft = map(-thrustLeft, -ANALOG_MAX, ANALOG_MAX, PWM_BCK_MAX, PWM_FWD_MAX);
		thrustRight = map(-thrustRight, -ANALOG_MAX, ANALOG_MAX, PWM_BCK_MAX, PWM_FWD_MAX);
		updateSpeed(thrustLeft, thrustRight);
	}

	// when there is no slow serial output add delay as constant updates are simply unnecessary
	if (DEBUG_OUTPUT != 1) delay(30);
}
