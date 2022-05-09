
class TRedPitayaTrigger():
    level = None
    Input = None

class TRedPitayaSetup():
    rate = None
    decimation = None
    trigger = None
    #socket = None

    #def __init__(self):
        #context = zmq.Context()
        #self.socket = context.socket(zmq.REQ)
        #self.socket.connect("tcp://localhost:5555")

    def read_hardware(self, socket):
        cmd = {'setup':'read'}
        fRead = None
        try:
            socket.send(str(cmd))
            message = self.socket.recv()
            print(message)
            fRead = True
        except Exception as e:
            print("Runtime error in read_hardware: {e}")
            fRead = False
        return fRead
