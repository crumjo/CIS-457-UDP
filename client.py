import socket
import sys
import os.path
import threading
import pickle
import json
def client():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    while True:
        try:
            ip = input("Enter IP: ")
            port = input("Enter Port: ")
            s.connect((ip, int(port)))
            break
        except:
            print("Unable to establish connection to server")

    print("Connected to server")

    while True:
        file = input("Enter filename: ")
        s.send(file.encode())

        fileExist = s.recv(1024).decode()
        if fileExist == "y":
            file = open("COPY_" + file, "wb")
            size = int(s.recv(1024).decode())
            s.send("ready".encode())
            print("Receiving file...")

            packet_buffer = {}
            for i in range(0,5):
                packet_buffer[i] = {i:""}

            next_packet_received = 0
            count = 0
            while count*1024 < size:
                # size of dict entry + 1024 bytes
                # Currently have issue with unpickling - order of bytearray is screwy
                try:
                    dataP = pickle.loads(s.recv(1073))
                except:
                    print ("error unpickling")

                # print(dataP)
                for i in dataP:
                    packet_buffer[i] = dataP[i]
                # if there is order in the buffer (1,2,3) print parts to buffer
                # if there is not order in buffer (1,3) print ordered part to buffer(1)
                for i in packet_buffer:
                    if i == next_packet_received:
                        count += 1
                        next_packet_received += 1
                        file.write(packet_buffer[i])
                        # if window at end, reset to start of array
                        if next_packet_received == 5:
                            next_packet_received = 0
                # Send ack of the lowest packet in array to shift
                s.send(str(next_packet_received).encode())

            file.close()
            print("Done receiving")
        else:
            print("File does not exist! Check file name.");


if __name__ == '__main__':
    client()
