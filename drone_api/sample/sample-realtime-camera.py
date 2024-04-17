import threading
import sys
import hakosim
import time
import math
import numpy as np
import cv2

def image_display_thread(client, fps=30):
    interval = 1.0 / fps
    while True:
        start_time = time.time()

        response = client.simGetImage("0", hakosim.ImageType.Scene)
        if response:
            img_np = np.frombuffer(response, dtype=np.uint8)
            img_rgb = cv2.imdecode(img_np, cv2.IMREAD_COLOR)
            cv2.imshow("Camera View", img_rgb)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        #elapsed_time = time.time() - start_time
        #wait_time = max(0, interval - elapsed_time)
        #time.sleep(wait_time)

    cv2.destroyAllWindows()


def transport(client, baggage_pos, transfer_pos):
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 3, 5)
    time.sleep(3)
    client.moveToPosition(baggage_pos['x'], baggage_pos['y'], 0.7, 0.01)
    time.sleep(3)
    client.grab_baggage(True)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], 3, 5)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], transfer_pos['z'], 0.01)
    time.sleep(3)
    client.grab_baggage(False)
    time.sleep(3)
    client.moveToPosition(transfer_pos['x'], transfer_pos['y'], 3, 0.01)

def debug_pos(client):
    pose = client.simGetVehiclePose()
    print(f"POS  : {pose.position.x_val} {pose.position.y_val} {pose.position.z_val}")
    roll, pitch, yaw = hakosim.hakosim_types.Quaternionr.quaternion_to_euler(pose.orientation)
    print(f"ANGLE: {math.degrees(roll)} {math.degrees(pitch)} {math.degrees(yaw)}")

def operation_sequence(client):
    client.takeoff(3)

    baggage_pos = { "x": 0, "y": 3 }
    transfer_pos = { "x": 0, "y": -1, "z": 0.7 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    baggage_pos = { "x": 0, "y": 4 }
    transfer_pos = { "x": 0, "y": -1, "z": 1.2 }
    transport(client, baggage_pos, transfer_pos)
    debug_pos(client)

    client.moveToPosition(-2, -1, 3, 5)
    debug_pos(client)
    time.sleep(3)
    client.moveToPosition(-2, -1, 0.7, 5)
    debug_pos(client)
    time.sleep(3)

    png_image = client.simGetImage("0", hakosim.ImageType.Scene)
    if png_image:
        with open("scene.png", "wb") as f:
            f.write(png_image)
    client.land()
    debug_pos(client)

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    client = hakosim.MultirotorClient(sys.argv[1])
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)

    # Run operation sequence in main thread
    operation_thread = threading.Thread(target=operation_sequence, args=(client,))
    operation_thread.start()

    # Create and start image display thread
    image_display_thread(client)


    # Wait for display thread to finish (on 'q' press)
    operation_thread.join()

    return 0

if __name__ == "__main__":
    sys.exit(main())
