import socket
import sys
import os.path


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
        file = input("Enter command: ")
        s.send(file.encode())
        if file == 'exit':
            s.close()
            break
        elif file == 'ls':
            print("File directory printing...")
            print(s.recv(1024).decode())
        else:
            fileExist = s.recv(1024).decode()
            if fileExist == "y":
                file = open("COPY_" + file, "wb")
                size = int(s.recv(1024).decode())
                s.send("ready".encode())
                print("Receiving file...")
                count = 0
                while count*1024 < size:
                    line = s.recv(1024)
                    file.write(line)
                    count += 1
                    print(file.tell())
                file.close()
                print("Done receiving")
            else:
                print("File does not exist! Check file name.");


client()