#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <PubSubClient.h>

// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// MQTT connection details
#define MQTT_HOST "broker.mqtt.cool"
#define MQTT_PORT  1883
#define MQTT_DEVICEID "d:hwu.esp8266.H00467490"
#define MQTT_USER "" // no need for authentication, for now
#define MQTT_TOKEN "" // no need for authentication, for now
#define MQTT_TOPIC_DISPLAY "d:hwu.esp8266.H00467490/cmd/display/fmt/json"
String color;
String control;
// Add new MQTT topics for temperature and humidity
#define MQTT_TOPIC_TEMP "d:hwu.esp8266.H00467490/evt/temperature/fmt/string"
#define MQTT_TOPIC_HUMIDITY "d:hwu.esp8266.H00467490/evt/humidity/fmt/string"
#define ledcontroltopic "led/control"
// Topics for RGB color control
#define MQTT_TOPIC_RGB_RED "d:hwu.esp8266.H00467490/cmd/color/red"
#define MQTT_TOPIC_RGB_GREEN "d:hwu.esp8266.H00467490/cmd/color/green"
#define MQTT_TOPIC_RGB_BLUE "d:hwu.esp8266.H00467490/cmd/color/blue"
#define MQTT_TOPIC_RGB "d:hwu.esp8266.H00467490/cmd/color"

// GPIO pins used to connect devices
#define RGB_PIN 5 // GPIO pin the data line of RGB LED is connected to
#define DHT_PIN 4 // GPIO pin the data line of the DHT sensor is connected to

// Specify DHT11 (Blue) or DHT22 (White) sensor
#define DHTTYPE DHT11

// Temperatures to set LED by (assume temp in C)
#define ALARM_COLD 0.0
#define ALARM_HOT 30.0
#define WARN_COLD 10.0
#define WARN_HOT 25.0

// WiFi credentials
char ssid[] = "Deepika";     // your network SSID (name)
char pass[] = "12345678";   // your network password

// LED Pins and PWM Constants
const int redPin = 23;    
const int greenPin = 22;  
const int bluePin = 21;
const int redChannel = 2;
const int greenChannel = 1;
const int blueChannel = 0;
const int pwmFreq = 5000;    
const int pwmResolution = 8;

// Initialize DHT sensor
DHT dht(DHT_PIN, DHTTYPE);

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// Initialize variables for temperature and humidity
float h = 0.0;
float t = 0.0;
void setRed() {
  ledcWrite(redChannel, 0);   // Set Red channel to max
  ledcWrite(greenChannel, 255);   // Green off
  ledcWrite(blueChannel, 255);    // Blue off
}
 
void setGreen() {
  ledcWrite(redChannel, 255);     // Red off
  ledcWrite(greenChannel, 0); // Set Green channel to max
  ledcWrite(blueChannel, 255);    // Blue off
}
 
void setBlue() {
  ledcWrite(redChannel, 255);     // Red off
  ledcWrite(greenChannel, 255);   // Green off
  ledcWrite(blueChannel, 0);  // Set Blue channel to max
}
void setup() {
    // Start serial console
    Serial.begin(115200);
    Serial.setTimeout(2000);
    
    while (!Serial) { }
    
    // Setup LED PWM Channels
    ledcSetup(redChannel, pwmFreq, pwmResolution);
    ledcSetup(greenChannel, pwmFreq, pwmResolution);
    ledcSetup(blueChannel, pwmFreq, pwmResolution);
    
    // Attach each PWM channel to its respective GPIO pin
    ledcAttachPin(redPin, redChannel);
    ledcAttachPin(greenPin, greenChannel);
    ledcAttachPin(bluePin, blueChannel);
    
    // Start connected devices
    dht.begin();
    
    // Start WiFi connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi Connected");

    // Connect to MQTT broker
    
}

 
void loop() {
    mqtt.loop();
    
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
            Serial.println("MQTT Connected");
            mqtt.subscribe(MQTT_TOPIC_DISPLAY);
            
            mqtt.subscribe(MQTT_TOPIC_RGB);
            mqtt.subscribe(ledcontroltopic);
        } else {
            Serial.println("MQTT Failed to connect!");
            delay(2000);
        }
    }
    
    h = dht.readHumidity();
    t = dht.readTemperature(); // Temperature in Celsius
    
        
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
    } else {
        // Publish temperature and humidity values
        char tempStr[10];
        char humStr[10];
        dtostrf(t, 6, 2, tempStr); // Convert temperature to string
        dtostrf(h, 6, 2, humStr); // Convert humidity to string

        // Publish temperature as a string
        if (!mqtt.publish(MQTT_TOPIC_TEMP, tempStr)) {
            Serial.println("MQTT Publish failed for temperature");
        }

        // Publish humidity as a string
        if (!mqtt.publish(MQTT_TOPIC_HUMIDITY, humStr)) {
            Serial.println("MQTT Publish failed for humidity");
        }
        
        // Set RGB LED Color Based on Temperature
        

        Serial.print("Temperature: ");
        Serial.println(t);
        Serial.print("Humidity: ");
        Serial.println(h);
        if(control=="ON"){
       if (color == "RED") {
            setRed();
        } else if (color == "GREEN") {
            setGreen();
        } else if (color == "BLUE") {
            setBlue();
        } }
        else{
            if(t > 25) setRed();
            else setGreen();
        }
    }

    // Pause - but keep polling MQTT for incoming messages
    delay(1000);
}

// Callback function for handling received messages
void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = 0; // Ensure valid content is zero terminated so can treat as C-string
    String message = String((char *)payload);

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(message);
    if(String(topic) == ledcontroltopic){
        control = message;
    }
    // Handle MQTT color messages
    if (String(topic) == MQTT_TOPIC_RGB) {
       color = message;
       Serial.println(color);
       
        
    }
    
}