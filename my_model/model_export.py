from ultralytics import YOLO

# Load the smallest YOLOv8n model
model = YOLO('detection_model.pt')  # you can use 'yolov8n.pt', not yolo11n (YOLOv11 not officially released)

# Export to optimized ONNX
model.export(
    format='onnx',
    imgsz=320,         # Smaller image size for speed
    half=False,         # FP16 for faster, smaller model (use only if your runtime supports it)
    dynamic=True,     # Fixed input size improves performance
    simplify=True,     # Simplify the graph
    opset=12,          # Set a compatible ONNX opset version
    nms=True,          # Include NMS
    batch=1,           # Single image batch size
    device='cpu'       # For Raspberry Pi CPU
)
