from keras.models import load_model
import cv2
import numpy as np

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("keras_model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# Define camera object (0 for default camera)
camera = cv2.VideoCapture(1)

# Define colors for bounding box and text
bounding_box_color = (0, 255, 0)  # Green
text_color = (0, 0, 255)        # White

# Define font size and thickness for text
font_size = 0.7
font_thickness = 2

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

    # Draw a rectangle around the identified object if it's an elephant
    if class_name == '0 ELEPHANT':
        # Get a random thickness for the bounding box (within a range)
        box_thickness = np.random.randint(2, 5)

        # Draw the bounding box with a slight transparency
        cv2.rectangle(image, (10, 10), (image.shape[1] - 10, image.shape[0] - 10), bounding_box_color, box_thickness)

        # Calculate text width to position it correctly within the box
        text_width, _ = cv2.getTextSize("Elephant", cv2.FONT_HERSHEY_SIMPLEX, font_size, font_thickness)[0]

        # Draw the label with a black background for better visibility
        cv2.rectangle(image, (10, 10), (10 + text_width + 10, 30 + font_thickness), (0, 0, 0), -1)
        cv2.putText(image, f"Elephant  {confidence_score:.2f}", (15, 25), cv2.FONT_HERSHEY_SIMPLEX, font_size, text_color, font_thickness)

    # Display class and confidence score on the image (optional)
    # cv2.putText(image, f"Class: {class_name}, Confidence: {confidence_score:.2f}", (10, image.shape[0] - 10),
    #             cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Show the image with predictions
    cv2.imshow("Webcam Image", image)

    # Listen to the keyboard for presses
    keyboard_input = cv2.waitKey(1)

    # Check if the ESC key is pressed
    if keyboard_input == 27:
        break

# Release the camera and close all windows
camera.release()
cv2.destroyAllWindows()
