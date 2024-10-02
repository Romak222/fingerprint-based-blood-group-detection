import serial
import numpy as np
from PIL import Image, ImageDraw
import matplotlib.pyplot as plt

# Set up the serial connection (keeping 'COM3' for your Arduino)
ser = serial.Serial('COM7', 115200)
ser.flushInput()

# Function to create a more realistic fingerprint pattern with circular ridges
def generate_circular_fingerprint(width, height):
    # Create an empty canvas with the given width and height
    image = Image.new('L', (width, height), 0)  # 'L' mode for grayscale
    draw = ImageDraw.Draw(image)
    
    # Define the center and number of rings
    center_x, center_y = width // 2, height // 2
    num_rings = 30  # Number of circular ridges

    # Draw concentric circles with random thickness variations
    for i in range(num_rings):
        # Radius of each circle
        radius = 5 + i * 8  # Start with a radius of 5 and increment each ring
        thickness = np.random.randint(1, 5)  # Random thickness for variation
        draw.ellipse(
            (center_x - radius, center_y - radius, center_x + radius, center_y + radius),
            outline=255, width=thickness
        )
    
    # Convert to NumPy array for further processing
    fingerprint_data = np.array(image)

    # Adding small Gaussian noise to simulate realistic fingerprints
    noise = np.random.normal(0, 10, (height, width)).astype(np.uint8)
    fingerprint_data = np.clip(fingerprint_data + noise, 0, 255)

    return fingerprint_data

# Function to save the image as BMP
def save_as_bmp(image_data, filename):
    image = Image.fromarray(image_data, 'L')  # 'L' mode for grayscale
    image.save(filename)
    print(f"Fingerprint image saved as {filename}")

# Function to display the generated fingerprint pattern
def display_fingerprint(image_data):
    plt.imshow(image_data, cmap='gray')
    plt.title("Generated Circular Fingerprint Pattern")
    plt.axis('off')  # Hide axis for better visualization
    plt.show()

# Function to simulate serial reading (replace with actual serial read if needed)
def simulate_serial_read():
    print("Reading fingerprint data from Arduino...")
    # Simulate a wait time for data reading
    import time
    time.sleep(2)  # Simulate a delay for data reading

# Main loop
if __name__ == "__main__":
    simulate_serial_read()  # Simulate reading data from the Arduino

    # Generate a more realistic circular fingerprint pattern
    fingerprint_image = generate_circular_fingerprint(128, 128)  # Adjust size as needed

    # Save the generated fingerprint image
    save_as_bmp(fingerprint_image, 'circular_fingerprint_pattern.bmp')

    # Display the generated fingerprint image
    display_fingerprint(fingerprint_image)

    # Close the serial connection
    ser.close()  # Close the serial connection if opened
