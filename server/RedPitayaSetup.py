from scpi_t import rp_channels,rp_decimation,rp_sampling
import json

rp_trig_type = ['EDGE', 'LEVEL']

class TRedPitayaTrigger:
    def __init__(self, init_level=5e-3):
        self.level = init_level
        self.Input = rp_channels[0]
        self.trig_type = rp_trig_type[0]

class TRedPitayaSetup():
    rate = rp_sampling[len(rp_sampling)-1]
    decimation = rp_decimation[0]
    #decimation = rp_decimation[len(rp_decimation)-1]
    trigger = TRedPitayaTrigger(5e-3)
    #socket = None

    #def __init__(self):
        #context = zmq.Context()
        #self.socket = context.socket(zmq.REQ)
        #self.socket.connect("tcp://localhost:5555")

    def read_hardware(self, socket):
        cmd = {"setup":"read"}
        fRead = None
        print(str(cmd))
        try:
            s = str(json.dumps(cmd))
            print("before send: %s " % s)
            #print(str(cmd))
            socket.send_string(s)
            print("message sent")
            #socket.send_string(str(cmd))
            message = socket.recv()
            print("message recieved")
            print("type(message) %s" % type(message))
            msg_str = str(message)
            print("string message: %s" % msg_str)
            msg_s = message.decode('utf-8')
            print("msg_s %s" % msg_s)
            print("message %s" % message)
            fRead = True
        except Exception as e:
            print("Runtime error in read_hardware: %s" % e)
            fRead = False
        return fRead
