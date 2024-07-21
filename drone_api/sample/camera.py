import threading
import sys
import hakosim
import time
import math
import numpy as np
import cv2

def image_display_thread(client, fps=15):
    interval = 1.0 / fps
    while True:
        start_time = time.time()

        try:
            response = client.simGetImage("0", hakosim.ImageType.Scene)
            if response:
                img_np = np.frombuffer(response, dtype=np.uint8)
                img_rgb = cv2.imdecode(img_np, cv2.IMREAD_COLOR)

                if img_rgb is None or img_rgb.size == 0:
                    print("Error: Failed to decode image")
                else:
                    cv2.imshow("Camera View", img_rgb)
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        break
            else:
                print("Error: No image received from client")

        except Exception as e:
            print(f"Error: {e}")

        elapsed_time = time.time() - start_time
        wait_time = max(0, interval - elapsed_time)
        time.sleep(wait_time)

    cv2.destroyAllWindows()


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <config_path>")
        return 1

    client = hakosim.MultirotorClient(sys.argv[1])
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)

    image_display_thread(client)


    return 0

if __name__ == "__main__":
    sys.exit(main())
