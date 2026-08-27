import cv2
import time
from ultralytics import YOLO
import numpy as np

# Load model
model = YOLO("detection_model.onnx")

# Define class-specific colors
class_colors = {
    "Elephant": (0, 255, 0),
    "Tiger": (255, 0, 0),
    "Squirrel": (0, 0, 255),
    "Bat": (255, 255, 0),
    "Monkey": (255, 0, 255),
    "Human": (214, 255, 111)
}

# Define class-specific accuracy thresholds
class_accuracy_thresholds = {
    "Elephant": 0.80,
    "Tiger": 0.80,
    "Squirrel": 0.70,
    "Bat": 0.75,
    "Monkey": 0.80,
    "Human": 0.60
}

# Define font parameters for better text rendering
font = cv2.FONT_HERSHEY_SIMPLEX  # Using a standard font
font_scale = 0.5
font_thickness = 1  # Normal thickness
text_padding = 5
text_y_offset = 2  # Offset to prevent overlap

# Open webcam
cap = cv2.VideoCapture(1)

# Set frame rate limit
frame_rate = 24
last_frame_time = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Limit frame rate
    current_time = time.time()
    if current_time - last_frame_time < 1 / frame_rate:
        continue
    last_frame_time = current_time

    # Resize frame
    frame_resized = cv2.resize(frame, (640, 480))

    # Run YOLO model
    results = model(frame_resized)

    # Iterate through detections
    for result in results:
        if result.boxes is not None and len(result.boxes) > 0:
            for box in result.boxes:
                confidence = float(box.conf)
                class_id = int(box.cls)
                label = model.names[class_id]

                # Get accuracy threshold
                accuracy_threshold = class_accuracy_thresholds.get(label, 0.5)

                if confidence >= accuracy_threshold:
                    xyxy = box.xyxy[0].tolist()
                    x1, y1, x2, y2 = map(int, xyxy)

                    # Get color
                    color = class_colors.get(label, (255, 255, 255))

                    # Create label text
                    text = f"{label}: {confidence:.2f}"
                    text_size = cv2.getTextSize(text, font, font_scale, font_thickness)[0]
                    text_x = x1
                    text_y = y1 - text_size[1] - 2 * text_padding - text_y_offset # Adjusted y position

                    # Ensure text background is within the image
                    if text_y < 0:
                        text_y = y1 + text_size[1] + 2 * text_padding + text_y_offset

                    # Draw filled rectangle for label background
                    cv2.rectangle(frame_resized, (text_x, text_y),
                                  (text_x + text_size[0] + 2 * text_padding, text_y + text_size[1] + 2 * text_padding),
                                  color, cv2.FILLED)

                    # Draw label text
                    cv2.putText(frame_resized, text, (text_x + text_padding, text_y + text_size[1] + text_padding),
                                font, font_scale, (0, 0, 0), font_thickness, cv2.LINE_AA)

                    # Draw bounding box
                    cv2.rectangle(frame_resized, (x1, y1), (x2, y2), color, 2)

    # Display frame
    cv2.imshow("YOLOv11n Animal Detection", frame_resized)

    # Break on 'q' press
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()