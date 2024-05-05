from keras.models import load_model
import cv2
import numpy as np
import serial
import threading
import time

# Load the model
model = load_model("keras_Model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# Define camera object (0 for default camera)
camera = cv2.VideoCapture(0)

# Open serial connection to Arduino
arduino = serial.Serial("COM12", 9600)  # Change 'COM8' to your Arduino's port
time.sleep(2)  # Wait for Arduino to initialize

# Variable to keep track of last sent time
last_sent_time = time.time()


# Function to send data to Arduino
def send_to_arduino(class_name):
    if class_name == "0 ELEPHANT":
        arduino.write(b"1")
    else:
        arduino.write(b"0")


# Function to capture and process frames from the camera
def capture_frames():
    global last_sent_time
    while True:
        # Grab the webcam image
        ret, image = camera.read()

        # Resize the image to model input shape
        image_resized = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)

        # Normalize the image
        image_norm = (image_resized / 255.0).reshape(1, 224, 224, 3)

        # Predict the class probabilities
        prediction = model.predict(image_norm)

        # Get the predicted class index and confidence score
        index = np.argmax(prediction)
        class_name = class_names[index].strip()
        confidence_score = prediction[0][index]

        # Draw a rectangle around the detected elephant
        if class_name == "0 ELEPHANT":
            cv2.rectangle(
                image, (0, 0), (image.shape[1], image.shape[0]), (0, 255, 0), 2
            )
            cv2.putText(
                image,
                "ELEPHANT DETECTED",
                (20, 30),
                cv2.FONT_HERSHEY_DUPLEX,
                0.5,
                (0, 0, 255),
                2,
            )
        else:
            cv2.putText(
                image,
                "ELEPHANT NOT DETECTED",
                (20, 30),
                cv2.FONT_HERSHEY_DUPLEX,
                0.5,
                (0, 255, 0),
                2,
            )

        # Show the live view from the camera
        cv2.imshow("Live View", image)

        # Check if 1 second has elapsed since the last sent time
        if time.time() - last_sent_time >= 0.5:
            # Call the function to send data to Arduino
            send_to_arduino(class_name)
            # Update the last sent time
            last_sent_time = time.time()

        # Listen to the keyboard for presses
        keyboard_input = cv2.waitKey(1)

        # Check if the ESC key is pressed
        if keyboard_input == 27:
            break


# Create and start the thread for capturing frames
frame_thread = threading.Thread(target=capture_frames)
frame_thread.start()

# Main thread continues to handle other tasks or can be idle
# Wait for the frame thread to finish
frame_thread.join()

# Release the camera
camera.release()
cv2.destroyAllWindows()
