import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5566")

while True:
    print("Waiting...")
    message = socket.recv()
    print("New message %s" % message)
    time.sleep(1)
    socket.send_string("Next, please")
    
