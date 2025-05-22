import serial

ser = serial.Serial('/dev/ttyUSB0', 1250000)  # Adjust the port and baud rate as needed

while True:
    if ser.in_waiting > 0:
        print(ser.readline().decode('utf-8', errors='replace').strip())
