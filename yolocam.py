import cv2
import time
from ultralytics import YOLO

# Load model (YOLOv8 Nano for fast processing)
model = YOLO("my_model.pt")

# Open webcam (source=0 for default webcam)
cap = cv2.VideoCapture(1)  # or try 0 if 1 doesn't work

# Set frame rate limit (e.g., 30 FPS)
frame_rate = 30
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

    # Resize the frame to a smaller size for faster processing (e.g., 640x480)
    frame_resized = cv2.resize(frame, (640, 480))

    # Run YOLO model on the resized frame
    results = model(frame_resized)

    # Render results (bounding boxes, labels, etc.)
    frame = results[0].plot()

    # Display the frame
    cv2.imshow("YOLOv8 Detection", frame)

    # Break loop on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
