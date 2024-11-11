 #include <ArduinoBLE.h>
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

BLEService ledService("180F"); // Updated BLE Service for LED

// BLE LED Lux Characteristic - updated UUID to match the Python code
BLEByteCharacteristic luxCharacteristic("2A6E", BLERead | BLEWrite);

// List of allowed central MAC addresses
const String allowedMACs[] = {"D3:72:9F:4C:11:67"}; // Updated with actual allowed MAC address

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  // Set advertised local name and service UUID:
  BLE.setLocalName("Nano 33 IoT");
  BLE.setAdvertisedService(ledService);

  // Add the characteristic to the service
  ledService.addCharacteristic(luxCharacteristic);

  // Add service
  BLE.addService(ledService);

  // Set the initial value for the characteristic
  luxCharacteristic.writeValue(0);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE device active, waiting for connection...");

  // Initialize I2C for BH1750 light sensor
  Wire.begin();
  lightMeter.begin();
  
}

void loop() {
  // Listen for BLE peripherals to connect
  BLEDevice central = BLE.central();

  // If a central is connected to peripheral:
  if (central) {
    String centralMAC = central.address();
   
    Serial.println(centralMAC);

    // Check if the connecting central is in the allowed list
    bool isAllowed = false;
    for (int i = 0; i < sizeof(allowedMACs) / sizeof(allowedMACs[0]); i++) {
      if (centralMAC.equalsIgnoreCase(allowedMACs[i])) {
        isAllowed = true;
        break;
      }
    }

    // If the central is allowed, proceed with connection
    if (isAllowed) {
      Serial.println("Connected to central:");

      // While the central is still connected to peripheral
      while (central.connected()) {
        Serial.print(">> ");
        
        // Read the lux level from the light sensor
        float lux = lightMeter.readLightLevel();
        Serial.print("Light Intensity: ");
        Serial.print(lux);
        Serial.println(" lx");

        // Delay between readings
        delay(500);

        // Map the lux to an appropriate analog value and write to BLE characteristic
        uint8_t analogLux = map(lux, 0, 1500, 0, 255); // Mapping lux to 0-255
        luxCharacteristic.writeValue(analogLux);
      }

      // When the central disconnects, print it out:
      Serial.print(F("Disconnected from central: "));
      Serial.println(central.address());
    } else {
      Serial.println("Connection rejected. Central not allowed.");
      BLE.disconnect(); // Disconnect the unauthorized central
    }
  }

