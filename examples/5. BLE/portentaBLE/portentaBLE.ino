#include <ArduinoBLE.h>

BLEService ledService("19b10000-e8f2-537e-4f6c-d104768a1214");

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19b10000-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // Pin to use for the LED

void setup() {
  Serial.begin(9600);
  //while (!Serial);   // Uncomment to wait for serial port to connect.

  // Set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    digitalWrite(LEDR, LOW);
    delay(1000);
    digitalWrite(LEDR, HIGH);

    // Stop if BLE couldn't be initialized.
    while (1);
  }

  // Set advertised local name and service UUID:
  BLE.setLocalName("LED-Portenta-01");
  BLE.setAdvertisedService(ledService);

  // Add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // Add service
  BLE.addService(ledService);

  // Set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  digitalWrite(LEDB, LOW);
  delay(1000);
  digitalWrite(LEDB, HIGH);
  Serial.println("BLE LED Control ready");
}

void loop() {
  // Listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // If a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // Print the central's MAC address:
    Serial.println(central.address());
    digitalWrite(LEDB, HIGH);
    delay(100);
    digitalWrite(LEDB, LOW);
    delay(100);
    digitalWrite(LEDB, HIGH);

    // While the central is still connected to peripheral:
    while (central.connected()) {
      // If the remote device wrote to the characteristic,
      // Use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // Any value other than 0
          Serial.println("LED on");
          digitalWrite(ledPin, LOW);          // Will turn the Portenta LED on
        } else {
          Serial.println("LED off");
          digitalWrite(ledPin, HIGH);         // Will turn the Portenta LED off
        }
      }
    }

    // When the central disconnects, print it out:
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
    digitalWrite(LEDB, HIGH);
    delay(100);
    digitalWrite(LEDB, LOW);
    delay(100);
    digitalWrite(LEDB, HIGH);
  }
}
