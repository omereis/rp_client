import zmq
from RedPitayaSetup import TRedPitayaSetup

#------------------------------------------------------------------------------
def main():
    context = zmq.Context()
    socket = context.socket(zmq.REQ)
    socket.connect("tcp://localhost:5555")
    run = True
    while run:
        cmd = input('Next Command ("h" for help) ').lower()
        hw = TRedPitayaSetup()
        if ((cmd == 'q') or (cmd == 'quit')):
            run = False
        elif ((cmd == 'h') or (cmd == 'help')):
            print_help()
        elif ((cmd == 'r') or (cmd == 'read')):
            try:
                read_setup(socket, hw)
                print_setup(hw)
            except Exception as e:
                print ("Runtime error in main:\n{e}")
        elif ((cmd == 'p') or (cmd == 'print')):
            #print(hw.rate)
            #print("rate: %s" % hw.rate)
            print_setup(hw)
        else:
            print("Command unnokwn")
    socket.disconnect("tcp://localhost:5555")
#------------------------------------------------------------------------------
def print_help():
    print("h/help - this list")
    print("r/read - read setup from card")
    print("p/print - print setup")
    print("q/quit - quit program")
#------------------------------------------------------------------------------
def read_setup(socket, hw):
    hw.read_hardware(socket)
    print("Setup read")
#------------------------------------------------------------------------------
def print_setup(hw):
    #print("print_setup")
    #print("Hardware Setup: %s" % hw)
    #print(type(hw))
    #print(dir(hw))
    print("Sample Rate: %s" % hw.rate)
    print("Decimation: %s" % hw.decimation)
    print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
    #print("Sample Rate: %" % hw.rate)
    #print("Decimation: %" % hw.decimation)
    #print("Setup printed")

#------------------------------------------------------------------------------
def print_socket_status(socket):
    print(socket)
#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
    print("bye")
    exit(0)
