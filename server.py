import socket
import sys
import os
import subprocess
from threading import Thread
import pickle
import time
import json
from struct import *

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
                    size = os.path.getsize(data)
                    self.connection.send(str(os.path.getsize(data)).encode())
                    if self.connection.recv(1024).decode() == 'ready':
                        packet_buffer = ["","","","",""]
                        count = 0
                        next_packet_send = 0  # position in array for next packet
                        # gives first 5 packets
                        for i in range (0,5):
                            packet_buffer[i] = {i: file.read(1024)}
                            print(packet_buffer[i])
                        # Error check for files below size of 1024 -> direct send complete full with pickle.dump
                        # Send first 5 before setting up loop
                        for i in range (0,5):
                            # print(sys.getsizeof(pickle.dumps(packet_buffer[i])))
                            # size of dict in pickle = 1073
                            dataP = pickle.dumps(packet_buffer[i], protocol=-1)
                            print(dataP)
                            self.connection.send(dataP)

                        while count*1024 < size:
                            # sets the tmp value to the ack from client -
                            # if loss/corruption/out of order resend as needed
                            # otherwise clear packet of the previous value
                            # and set next 1024 bytes
                            tmp = self.connection.recv(1024).decode()
                            #if the ack is the expected value, draw new packet
                            # increment overall packet count
                            # send new packet (if packet 0 is acked, replace it with packet 5 and send
                            if next_packet_send == tmp:
                                packet_buffer[next_packet_send] = {next_packet_send : file.read(1024)}
                                count += 1
                                dataP = pickle.dumps(packet_buffer[next_packet_send], protocol=-1)
                                self.connection.send(dataP)
                                next_packet_send += 1
                                if next_packet_send == 5:
                                    next_packet_send = 0
                            #No ack for packet received
                            else:
                                dataP = pickle.dumps(packet_buffer[next_packet_send], protocol=-1)
                                self.connection.send(dataP)

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


if __name__ == '__main__':
    server()