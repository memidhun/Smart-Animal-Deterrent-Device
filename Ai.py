from keras.models import load_model
import cv2
import numpy as np
import serial
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
        cv2.rectangle(image, (0, 0), (image.shape[1], image.shape[0]), (0, 255, 0), 2)
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

    # Control OLED display based on the detected class
    if class_name == "0 ELEPHANT":
        arduino.write(b"1")
    # display_text = "DETECTED!"
    else:
        arduino.write(b"0")
    # display_text = "NOT DETECTED"

    # # Send display_text to Arduino
    # arduino.write(
    #     display_text.encode() + b"\n"
    # )  # Add newline character to mark end of message

    # Listen to the keyboard for presses
    keyboard_input = cv2.waitKey(1)

    # Check if the ESC key is pressed
    if keyboard_input == 27:
        break

# Release the camera
camera.release()
cv2.destroyAllWindows()
