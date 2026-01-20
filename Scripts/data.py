import serial
import csv
import time
import sys
PORT = "COM5"
BAUD_RATE = 38400
FILE_NAME = "sensor_acquisition_v1.csv"
HEADERS = ["Timestamp", "Distance_cm", "Servo_Angle_deg", "Current_A"]

def start_logger():
    """Main logging loop for sensor data acquisition."""
    ser = None
    try:
        ser = serial.Serial(PORT, BAUD_RATE, timeout=0.1) 
        time.sleep(2) 
        
        print(f">>> Initialization successful on {PORT}")
        print(f">>> Recording to: {FILE_NAME}")
        print(">>> Press Ctrl+C to stop recording safely.")

        with open(FILE_NAME, mode="w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(HEADERS)
            
            while True:
                line = ser.readline().decode("utf-8", errors="ignore").strip()
                
                if line and "," in line:
                    raw_data = line.split(',')
                    if len(raw_data) == 3:
                        timestamp = time.strftime("%H:%M:%S")
                        writer.writerow([timestamp] + raw_data)
                        f.flush() 
                        print(f"[{timestamp}] DATA: {raw_data}")
                        
    except serial.SerialException as e:
        print(f"\n[ERROR] Port Connection Error: {e}")
    except KeyboardInterrupt:
        print("\n[INFO] Manual termination by user. Finishing session...")
    except Exception as e:
        print(f"\n[CRITICAL] Unexpected error: {e}")
        
    finally:
        if ser and ser.is_open:
            ser.close()
            print(">>> Interface closed. Data integrity secured.")

if __name__ == "__main__":
    start_logger()