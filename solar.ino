#include <Servo.h>

// Solar Tracking System
#define LDR1 A0                 // LDR sensor pin 1
#define LDR2 A1                 // LDR sensor pin 2
#define TRACKING_ERROR 6        // Minimum error margin for adjustment
#define MAX_STEP 5              // Maximum step size for servo movement
int trackingPosition = 90;      // Starting position of tracking servo

// Dust Cleaning System
#define DUST_SENSOR_PIN A2      // Analog pin for dust sensor
#define LED_PIN 3               // LED control pin for dust sensor
#define DUST_THRESHOLD 100      // Dust threshold level
int dustLevel = 0;              // Variable for dust level

// Servo Motor Pins
#define TRACKING_SERVO_PIN 11   // Servo pin for solar tracking
#define CLEANING_SERVO_PIN 9    // Servo pin for dust cleaning

// Create servo objects
Servo trackingServo;
Servo cleaningServo;

void setup() {
  Serial.begin(9600);

  // Attach servo motors to their respective pins
  trackingServo.attach(TRACKING_SERVO_PIN);
  cleaningServo.attach(CLEANING_SERVO_PIN);

  // Set initial position for the tracking servo
  trackingServo.write(trackingPosition);
  delay(1000);

  // Set initial position for the cleaning servo
  cleaningServo.write(0);  // Start at 0° position for cleaning servo

  pinMode(LED_PIN, OUTPUT); // Initialize LED pin for dust sensor
  digitalWrite(LED_PIN, LOW);

  Serial.println("System Initialized");
}

void loop() {
  // Solar Tracking System
  int ldr1 = averageLDR(LDR1);  // Smoothed LDR1 value
  int ldr2 = averageLDR(LDR2);  // Smoothed LDR2 value

  // Calculate the light difference
  int difference = ldr1 - ldr2;

  // Adjust tracking position based on difference
  if (abs(difference) > TRACKING_ERROR) {
    int step = map(abs(difference), TRACKING_ERROR, 1023, 1, MAX_STEP);  // Map difference to step size
    if (difference > 0 && trackingPosition > 0) {
      trackingPosition -= step;  // Move left
    }
    if (difference < 0 && trackingPosition < 180) {
      trackingPosition += step;  // Move right
    }
    trackingServo.write(trackingPosition);  // Move tracking servo
    delay(100);  // Smooth motion
  }

  // Dust Cleaning System
  digitalWrite(LED_PIN, HIGH);            // Turn on LED for dust sensor
  delayMicroseconds(280);
  dustLevel = analogRead(DUST_SENSOR_PIN); // Read dust sensor
  digitalWrite(LED_PIN, LOW);             // Turn off LED

  Serial.print("Dust Level: ");
  Serial.println(dustLevel);

  // Check if dust level exceeds the threshold
  if (dustLevel > DUST_THRESHOLD) {
    Serial.println("Dust Detected! Initiating Cleaning...");
    cleanPanel();  // Call cleaning function
  }

  delay(1000);  // Delay for 1 second before next reading
}

// Cleaning function to move cleaning servo
void cleanPanel() {
  cleaningServo.write(90);   // Move to 90° (cleaning position)
  delay(1000);               // Hold for 1 second
  cleaningServo.write(0);    // Return to starting position
  delay(500);                // Short delay
}

// Function to calculate average value from LDR
int averageLDR(int pin) {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(pin);
    delay(10);
  }
  return sum / 5;  // Return the average
}