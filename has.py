import cv2
from cvzone.HandTrackingModule import HandDetector
import serial
import time

ser = serial.Serial(port='COM4', baudrate=9600)

cap = cv2.VideoCapture(0)
detector = HandDetector(detectionCon=0.8, maxHands=2)

while True:
    success, img = cap.read()
    hands, img = detector.findHands(img)

    if hands:
        hand1 = hands[0]
        lmList1 = hand1["lmList"]
        bbox1 = hand1["bbox"]
        centerPoint1 = hand1["center"]
        handType1 = hand1["type"]

        fingers1 = detector.fingersUp(hand1)

        if len(hands) == 2:
            hand2 = hands[1]
            lmList2 = hand2["lmList"]
            bbox2 = hand2["bbox"]
            centerPoint2 = hand2["center"]
            handType2 = hand2["type"]

            fingers2 = detector.fingersUp(hand2)
            print(fingers1)
            length, info, img = detector.findDistance(centerPoint1, centerPoint2, img)
        else:
            print(fingers1)
            fingers_str = '$' + ''.join(map(str, fingers1))
            print(fingers_str)
            ser.write(fingers_str.encode())

            # Receive FSR Data
            if ser.in_waiting > 0:
                fsr_data = ser.readline().decode('utf-8').strip() 
                print("Received FSR data:", fsr_data)  # Debug statement
                fsr_values = fsr_data.split(',')
                print("FSR data length:", len(fsr_data))  # Debug statement

                # Print FSR Values Nicely
                print("FSR Sensor Values:")
                for i in range(len(fsr_values)):
                    print(f"FSR {i+1}: {fsr_values[i]}")

            cv2.imshow("Image", img)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    cv2.imshow("Image", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
