# source: https://zeromq.org/languages/python/
import zmq

context = zmq.Context()

#  Socket to talk to server
print("Connecting to hello world server…")
socket = context.socket(zmq.REQ)
#socket.connect("tcp://localhost:5555")
socket.connect("tcp://0.0.0.0:5566")
#socket.connect("tcp://10.0.0.4:5566")

#  Do 10 requests, waiting each time for a response
for request in range(10):
    print("Sending request %s …" % request)
    socket.send(b"Hello")

    #  Get the reply.
    message = socket.recv()
    print("Received reply %s [ %s ]" % (request, message))
