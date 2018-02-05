import socket
import sys
import os
import subprocess
from threading import Thread


class multiClient(Thread):
    def __init__(self, connection, address):
        Thread.__init__(self)
        self.connection = connection
        self.address = address

    def run(self):
        client_ip, client_port = str(self.address[0]), str(self.address[1])
        print("Client connection from " + client_ip + ":" + client_port)

        while True:
            data = self.connection.recv(1024).decode()
            if data == 'ls':
                print("Directory listing requested")
                files = subprocess.check_output("ls", shell=True)
                self.connection.send(files)
            elif data == 'exit':
                print("Closing client connection")
                self.connection.close()
                break
            else:
                print("File requested: " + data[0:])
                # sends y if file exists, n otherwise
                if os.path.isfile(data[0:]):
                    data = data[0:]
                    self.connection.send("y".encode())
                    file = open(data, "rb");
                    print("Sending file...")
                    self.connection.send(str(os.path.getsize(data)).encode())
                    if self.connection.recv(1024).decode() == 'ready':
                        while True:
                            packet = file.read(1024)
                            if not packet:
                                break
                            self.connection.send(packet)

                        file.close()
                        print("Finished sending")
                else:
                    self.connection.send("n".encode())


def server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        port = input("Enter Port: ")
        try:
            server.bind(("", int(port)))
            break
        except:
            print("Unable to create server - invalid port")

    server.listen(10)

    while True:
        conn, addr = server.accept()
        thread1 = multiClient(conn, addr)
        thread1.start()


server()