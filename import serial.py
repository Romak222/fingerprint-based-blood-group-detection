import serial
import numpy as np
from PIL import Image

# Open the serial port (change 'COM3' to the correct port for your system)
ser = serial.Serial('COM7', 57600, timeout=1)

try:
    while True:
        line = ser.readline().decode('utf-8').strip()  # Read a line from the serial port
        if line == "IMAGE_READY":
            print("Image data is ready to be captured...")
            image_data = ser.read(512)  # Read the expected number of bytes for the fingerprint image
            
            # Check if any data was received
            if len(image_data) == 512:
                # Create an array from the image data
                image_array = np.frombuffer(image_data, dtype=np.uint8).reshape((112, 92))  # Adjust dimensions based on your sensor
                
                # Convert to PIL Image and save as PNG
                img = Image.fromarray(image_array)
                img.save("fingerprint.png")
                print("Fingerprint image saved as fingerprint.png")
            else:
                print("Received unexpected image data size:", len(image_data))
except ValueError as ve:
    print("ValueError:", ve)
except KeyboardInterrupt:
    print("Stopped by user.")
finally:
    ser.close()  # Close the serial port
