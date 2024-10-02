#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // RX, TX for the fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600);  // Set up the baud rate for the R307

  // Initialize the fingerprint sensor
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
  } else {
    Serial.println("Fingerprint sensor not found :(");
    while (1); // Halt the program
  }
}

void loop() {
  // Capture the fingerprint image
  if (finger.getImage() == FINGERPRINT_OK) {
    Serial.println("Fingerprint image captured!");

    // Send 256 bytes of simulated image data
    for (int i = 0; i < 256; i++) { // Send dummy data representing image
      Serial.write(i % 256); // Simulating image data
    }
    Serial.println("END");  // End of data signal
  } else {
    Serial.println("Failed to capture image.");
  }
  
  delay(5000);  // Wait before the next attempt
}
