import socket
import threading
import sys
import time
import signal

host = "localhost"
port = 5000

is_stop = False

def sig_handler(signum, frame):
	exit(0)

signal.signal(signal.SIGTERM, sig_handler)
signal.signal(signal.SIGINT, sig_handler)

try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
except:
    print("Could not make a connection to the server")
    sys.exit(0)


while True:
    message = "test message"
    sock.sendall(str.encode(message))
    data = sock.recv(100)
    print(str(data.decode("utf-8")))
    time.sleep(1)
