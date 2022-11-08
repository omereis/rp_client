import zmq
import json
from RedPitayaSetup import TRedPitayaSetup

#------------------------------------------------------------------------------
def main():
    strJson = ''
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
        elif ((cmd == 'f') or (cmd == 'file')):
            strJson = send_from_file(socket, strJson)
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
def send_from_file(socket, strJson):
    strFile = ''
    f = None
    strPrompt = ''
    if (len(strJson) == 0):
        strPrompt = 'Enter JSON file name [.json] '
    else:
        strPrompt = 'Enter JSON file name [%s] ' % strJson
    strFile = input(strPrompt).lower()
    if (len(strFile) == 0):
        strFile = strJson
    else:
        if (strFile.find('.') < 0):
            strFile += '.json'
    #else:
        #strFile = strJson
    try:
        print(strFile)
        f = open(strFile)
        print("file opened")
        j = json.load(f)
        print("JSON loaded")
        #socket.send_string(s)
        socket.send_string(str(j))
        print("message sent")
            #socket.send_string(str(cmd))
        message = socket.recv()
        print(message)
        strJson = strFile
    except Exception as e:
        strJson = ''
        print ("Runtime error in 'send_from_file' %s" % e)
    finally:
        if (f != None):
            f.close()
    return (strJson)
#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
    print("bye")
    exit(0)
