import zmq
from RedPitayaSetup import TRedPitayaSetup

context = zmq.Context()
socket = context.socket(zmq.REQ)
#------------------------------------------------------------------------------
def main():
    run = True
    while run:
        cmd = input('Next Command ("h" for help) ').lower()
        hw = TRedPitayaSetup()
        if ((cmd == 'q') or (cmd == 'quit')):
            run = False
        elif ((cmd == 'h') or (cmd == 'help')):
            print_help()
        elif ((cmd == 'r') or (cmd == 'read')):
            read_setup()
            print_setup(hw)
        elif ((cmd == 'p') or (cmd == 'print')):
            print_setup()
        else:
            print("Command unnokwn")
#------------------------------------------------------------------------------
def print_help():
    print("h/help - this list")
    print("r/read - read setup from card")
    print("p/print - print setup")
    print("q/quit - quit program")
#------------------------------------------------------------------------------
def read_setup():
    print("Setup read")
#------------------------------------------------------------------------------
def print_setup(hw):
    print("Sample Rate: {hw.rate}")
    print("Decimation: {hw.decimation}")
    #print("Setup printed")

#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
    print("bye")
    exit(0)
