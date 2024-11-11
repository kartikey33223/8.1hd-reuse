from bluepy import btle
import time
from gpiozero import Buzzer
import RPi.GPIO

RPi.GPIO.setmode(RPi.GPIO.BOARD)

# Initialize Buzzer at GPIO pin 18 (change pin if needed)
buzzer = Buzzer(18)

def map_distance_to_buzz(distance, max_distance=60):
    """
    Map the distance to a buzzing pattern.
    If the distance is less than a threshold, the buzzer beeps.
    """
    if distance < max_distance:
        buzzer.on()
        time.sleep(0.1)  # Short beep
        buzzer.off()
        time.sleep(0.1)
    else:
        buzzer.off()

def connect_and_read_ble(device_mac, characteristic_uuid):
    try:
        # Connect to the BLE device
        print(f"Connecting to {device_mac}...")
        device = btle.Peripheral(device_mac, btle.ADDR_TYPE_PUBLIC)

        # Get the characteristic by UUID and read its value
        print(f"Reading characteristic {characteristic_uuid}...")
        
        while(True):
            characteristic = device.getCharacteristics(uuid=characteristic_uuid)[0]
            value = characteristic.read()
            number = int.from_bytes(value, byteorder='big')  # Convert to integer
            print(f"Distance from wall: {number} cm")

            
            # Trigger the buzzer based on distance
            map_distance_to_buzz(number, max_distance=60)
            
    except Exception as e:
        print(f"Failed to connect or read from {device_mac}: {str(e)}")
        device.disconnect()
        print("Disconnected")
    except KeyboardInterrupt:
        # Always disconnect after finishing
        print("Disconnecting...")
        device.disconnect()
        print("Disconnected")

if _name_ == "_main_":
    # BLE device's MAC address and characteristic UUID
    device_mac_address = "D3:72:9F:4C:11:67"  
    characteristic_uuid = "2A6E"  

    connect_and_read_ble(device_mac_address, characteristic_uuid)