import cv2
import time
import serial
from ultralytics import YOLO
import numpy as np

# Open serial connection to ESP32 on COM4 (adjust baudrate if needed)
esp_serial = serial.Serial('COM4', 115200, timeout=1)
time.sleep(2)  # Give time for ESP32 to reset
print("Serial communication established with ESP32 on COM4")


# Load model
model = YOLO("detection_model.onnx")

# Class to character mapping
class_to_char = {
    "Elephant": "E",
    "Tiger": "T",
    "Squirrel": "S",
    "Bat": "B",
    "Monkey": "M",
}

# Track last sent time per class
last_sent_time = {cls: 0 for cls in class_to_char}

# Thresholds and colours
class_colors = {
    "Elephant": (0, 255, 0),
    "Tiger": (255, 0, 0),
    "Squirrel": (0, 0, 255),
    "Bat": (255, 255, 0),
    "Monkey": (255, 0, 255),
    "Human": (214, 255, 111)
}

class_accuracy_thresholds = {
    "Elephant": 0.80,
    "Tiger": 0.80,
    "Squirrel": 0.70,
    "Bat": 0.75,
    "Monkey": 0.80,
    "Human": 0.60
}

# Font parameters
font = cv2.FONT_HERSHEY_SIMPLEX
font_scale = 0.5
font_thickness = 1
text_padding = 5
text_y_offset = 2

# Open webcam
cap = cv2.VideoCapture(1)
frame_rate = 24
last_frame_time = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    current_time = time.time()
    if current_time - last_frame_time < 1 / frame_rate:
        continue
    last_frame_time = current_time

    frame_resized = cv2.resize(frame, (640, 480))
    results = model(frame_resized)

    for result in results:
        if result.boxes is not None and len(result.boxes) > 0:
            for box in result.boxes:
                confidence = float(box.conf)
                class_id = int(box.cls)
                label = model.names[class_id]
                accuracy_threshold = class_accuracy_thresholds.get(label, 0.5)

                if confidence >= accuracy_threshold:
                    xyxy = box.xyxy[0].tolist()
                    x1, y1, x2, y2 = map(int, xyxy)
                    color = class_colors.get(label, (255, 255, 255))

                    text = f"{label}: {confidence:.2f}"
                    text_size = cv2.getTextSize(text, font, font_scale, font_thickness)[0]
                    text_x = x1
                    text_y = y1 - text_size[1] - 2 * text_padding - text_y_offset
                    if text_y < 0:
                        text_y = y1 + text_size[1] + 2 * text_padding + text_y_offset

                    cv2.rectangle(frame_resized, (text_x, text_y),
                                  (text_x + text_size[0] + 2 * text_padding, text_y + text_size[1] + 2 * text_padding),
                                  color, cv2.FILLED)

                    cv2.putText(frame_resized, text, (text_x + text_padding, text_y + text_size[1] + text_padding),
                                font, font_scale, (0, 0, 0), font_thickness, cv2.LINE_AA)

                    cv2.rectangle(frame_resized, (x1, y1), (x2, y2), color, 2)

                    # SERIAL SEND CONDITION
                    if label in class_to_char:
                        if current_time - last_sent_time[label] > 10:
                            send_char = class_to_char[label]
                            esp_serial.write(send_char.encode())
                            print(f"Sent to ESP32: {send_char}")
                            last_sent_time[label] = current_time

    cv2.imshow("YOLOv11n Animal Detection", frame_resized)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
esp_serial.close()
