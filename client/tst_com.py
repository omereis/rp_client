import zmq
from rp_flask import open_socket
import time
import multiprocessing, subprocess, Queue
from datetime import datetime,timedelta

#------------------------------------------------------------------------------
def th_limit_to (pTest, qData, timeout):
    fProcessAlive = pTest.is_alive()
    print (fProcessAlive)
    dtStart = datetime.now()
    dtDelta = 0
    #print('pTest: {}'.format(pTest))
    #print('type(pTest): {}'.format(type(pTest)))
    while ((fProcessAlive == True) & (dtDelta < timeout)):
        fProcessAlive = pTest.is_alive()
        time.sleep(0.1)
        dtDelta = (datetime.now() - dtStart).total_seconds()
        #print('total seconds: {}'.format(dtDelta))
    #print('\n\n')
    #print('pTest: {}'.format(pTest))
    #print('type(pTest): {}'.format(type(pTest)))
    if (dtDelta >= timeout):
        pTest.terminate()
    #else:
        #pTest.join()
    pTest.join()
    print('qData.qsize(): {}'.format(qData.qsize()))
    return (dtDelta >= timeout)
    #time.sleep(timeout)
    #if (pTest.is_alive()):
        #pTest.terminate()
    #else:
        #pTest.join()

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
            qData = Queue()
            pRecv = multiprocessing.Process(target=send_recieve_message, args=(cmd, qData, ))
            pRecv.start()
            pRecv.join()
            print('qData.qsize(): {}'.format(qData.qsize()))
            #fTimeout = th_limit_to (pRecv, qData, 1.0)
            #if fTimeout :
                #print('Timeout')
            #else:
                #print('Message sent')
            #qData.join_thread()
            #print('Threads ended, queue size: {}'.format(qData.qsize()))

#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
    print("bye")
    #exit(0)
