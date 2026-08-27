import cv2
import time
from ultralytics import YOLO

# Load model
model = YOLO("detection_model.pt")

# Define class-specific colors (you can customize these)
class_colors = {
    "Elephant": (0, 255, 0),      # Green
    "Tiger": (255, 0, 0),         # Blue
    "Squirrel": (0, 0, 255),        # Red
    "Bat": (255, 255, 0),         # Yellow
    "Monkey": (255, 0, 255),      # Magenta
    "Human": (214, 255,111)      # Gwww
    # Add more classes and their colors as needed
}

# Define class-specific accuracy thresholds
class_accuracy_thresholds = {
    "Elephant": 0.60,
    "Tiger": 0.50,
    "Squirrel": 0.50,
    "Bat": 0.50,
    "Monkey": 0.50,
    "Human":0.40
    # Add thresholds for all your classes
}

# Open webcam
cap = cv2.VideoCapture(2)  # or try 0 if 1 doesn't work

# Set frame rate limit
frame_rate = 24
last_frame_time = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Limit the frame rate
    current_time = time.time()
    if current_time - last_frame_time < 1 / frame_rate:
        continue
    last_frame_time = current_time

    # Resize the frame for faster processing
    frame_resized = cv2.resize(frame, (640, 480))

    # Run YOLO model on the resized frame
    results = model(frame_resized)

    # Iterate through the detected objects
    for result in results:
        if result.boxes is not None and len(result.boxes) > 0:
            for box in result.boxes:
                confidence = float(box.conf)
                class_id = int(box.cls)
                label = model.names[class_id]

                # Get the accuracy threshold for the current class
                accuracy_threshold = class_accuracy_thresholds.get(label, 0.5) # Default to 50% if class not in thresholds

                if confidence >= accuracy_threshold:
                    xyxy = box.xyxy[0].tolist()
                    x1, y1, x2, y2 = map(int, xyxy)

                    # Get the color for the current class
                    color = class_colors.get(label, (255, 255, 255)) # Default to white if class not in colors

                    # Draw bounding box and label with confidence
                    cv2.rectangle(frame_resized, (x1, y1), (x2, y2), color, 2)
                    text = f"{label}: {confidence:.2f}"
                    cv2.putText(frame_resized, text, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

    # Display the frame with filtered and colored bounding boxes
    cv2.imshow("YOLOv8 Detection (Class-Specific Accuracy)", frame_resized)

    # Break loop on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()