#include <WiFi.h>
#include <Adafruit_Fingerprint.h>
#include <WebServer.h>
#include <mbedtls/base64.h>  // Base64 encoder
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // I2C library for 16x2 LCD

// Wi-Fi credentials
const char* ssid = "POCO F1";
const char* password = "12345678";

// Initialize Web Server on port 80
WebServer server(80);

// Connect fingerprint sensor to hardware serial (change if needed)
HardwareSerial mySerial(2);  // Use Serial2 (GPIO16 RX, GPIO17 TX)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// I2C address for 16x2 LCD (usually 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

String fingerprintImageBase64; // Placeholder for fingerprint image data

// HTML page with JavaScript to fetch and display fingerprint image
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fingerprint Capture</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        img { max-width: 100%; height: auto; margin-top: 20px; }
    </style>
</head>
<body>
    <h1>Fingerprint Capture</h1>
    <button onclick="getFingerprint()">Capture Fingerprint</button>
    <img id="fingerprintImage" src="" alt="Fingerprint Image" />
    
    <script>
        function getFingerprint() {
            fetch('/capture').then(response => response.json()).then(data => {
                if (data.success) {
                    document.getElementById('fingerprintImage').src = "data:image/png;base64," + data.image;
                } else {
                    alert('Failed to capture fingerprint');
                }
            });
        }
    </script>
</body>
</html>
)rawliteral";

// Function to encode a placeholder image to Base64 (replace with actual image capture if possible)
String base64EncodePlaceholder() {
    // Placeholder: A simple 1x1 white image
    uint8_t placeholderImage[3] = { 255, 255, 255 }; // RGB for white
    return base64Encode(placeholderImage, sizeof(placeholderImage));
}

// Function to encode byte array to Base64
String base64Encode(uint8_t* data, size_t length) {
    size_t encodedLen = 4 * ((length + 2) / 3);  // Calculate Base64 encoded length
    char* encoded = (char*)malloc(encodedLen + 1); // +1 for null terminator
    size_t actualEncodedLen;

    if (mbedtls_base64_encode((unsigned char*)encoded, encodedLen + 1, &actualEncodedLen, data, length) == 0) {
        String encodedStr = String(encoded);
        free(encoded); // Free memory
        return encodedStr;
    }

    free(encoded);  // Free memory if encoding fails
    return String(); // Return empty string if encoding fails
}

// Function to handle fingerprint capture
void handleCapture() {
    bool captureSuccess = false;

    // Try to capture a fingerprint image
    if (finger.getImage() == FINGERPRINT_OK) {
        // Convert the captured image to a template
        if (finger.image2Tz()) {
            captureSuccess = true;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Fingerprint OK");
        }
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Capture Failed");
    }

    // For now, return a placeholder image encoded in Base64
    fingerprintImageBase64 = base64EncodePlaceholder();

    // Respond with JSON containing the result
    String jsonResponse = "{\"success\": ";
    jsonResponse += captureSuccess ? "true" : "false";
    jsonResponse += ", \"image\": \"";
    jsonResponse += fingerprintImageBase64;
    jsonResponse += "\"}";
    server.send(200, "application/json", jsonResponse);
}

// Function to serve the HTML page
void handleRoot() {
    server.send_P(200, "text/html", htmlPage);
}

void setup() {
    Serial.begin(115200);

    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint Based");
    lcd.setCursor(0, 1);
    lcd.print("Blood Group Det.");
    delay(3000); // Display startup message for 3 seconds
    lcd.clear();

    // Start WiFi connection
    lcd.setCursor(0, 0);
    lcd.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    // Once connected, print the IP address and start the web server
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(3000); // Display the IP for 3 seconds
    lcd.clear();

    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    // Web server routes
    server.on("/", handleRoot);        // Serve HTML page
    server.on("/capture", handleCapture);  // Handle fingerprint capture
    server.begin();
    Serial.println("Web server started");

    lcd.setCursor(0, 0);
    lcd.print("Server Started");

    // Initialize fingerprint sensor
    mySerial.begin(57600);  // Baud rate for R307 fingerprint sensor

    // Check fingerprint sensor after the server is initialized
    if (finger.verifyPassword()) {
        Serial.println("Fingerprint sensor found!");
        lcd.setCursor(0, 1);
        lcd.print("Sensor Ready");
    } else {
        Serial.println("Fingerprint sensor not found!");
        lcd.setCursor(0, 1);
        lcd.print("Sensor Failed");
    }
}

void loop() {
    server.handleClient();
}
