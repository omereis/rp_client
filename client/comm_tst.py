import zmq
from rp_flask import open_socket
import threading, queue, time
import multiprocessing, subprocess
from datetime import datetime,timedelta

#------------------------------------------------------------------------------
def send_recieve_message (cmd, qData):
    try:
        socket = None
        #socket = open_socket()
        #socket.send_string(cmd)
        print('waiting for reply')
        txt = 'socket.recv()'
        qData.put(txt)
        print('reply recieved:\n{}'.format(txt))
        print('Queue size:\n{}'.format(qData.qsize()))
        print('dir(qData): {}'.format(dir(qData)))
        #qData.close()
    except Exception as e:
        print ("Runtime error in 'send_recieve_message':\n{}".format(e))
    finally:
        if (socket != None):
            socket.close()

#------------------------------------------------------------------------------
def main():
    run = True
    while run:
        cmd = input('Next Command ("h" for help) ').lower()
        if ((cmd == 'q') or (cmd == 'quit')):
            run = False
        else:
            qData = queue.Queue()
            pRecv = multiprocessing.Process(target=send_recieve_message, args=(cmd, qData, ))
            pRecv.start()
            fTimeout = th_limit_to (pRecv, qData, 1.0)
            if fTimeout :
                print('Timeout')
            else:
                print('Message sent')
            #qData.join_thread()
            print('Threads ended, queue size: {}'.format(qData.qsize()))

#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
    print("bye")
    #exit(0)
