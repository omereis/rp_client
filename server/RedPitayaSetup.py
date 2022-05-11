from scpi_t import rp_channels,rp_decimation,rp_sampling

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
        cmd = {'setup':'read'}
        fRead = None
        try:
            socket.send_string(str(cmd))
            message = socket.recv()
            print(message)
            fRead = True
        except Exception as e:
            print("Runtime error in read_hardware: %s" % e)
            fRead = False
        return fRead
