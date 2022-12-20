import zmq
from flask import Flask,render_template, request
from RedPitayaSetup import TRedPitayaSetup
import sys, socket, json, time
from datetime import datetime
from multiprocessing import Process, Queue

app = Flask(__name__)
txtHostName = None
g_txtTarget = 'Red Pitaya'
g_fIsRedPitaya = True
g_fIsontainer = False
global g_nPackageSize

#------------------------------------------------------------------------------
@app.route("/")
def index():
    #if (txtHostName != None):
    print('g_fIsontainer : {}'.format(g_fIsontainer ))
    if (g_fIsontainer == True):
        print("Running in a container");
        g_txtTarget = 'Docker Container'
        g_fIsRedPitaya = "false"
    else:
        g_txtTarget = 'Red Pitaya Hardware'
        g_fIsRedPitaya = "true"
        print("Running on a card");
    print ('hostname: {}'.format(txtHostName))
    #print ('Target: {}'.format(g_txtTarget ))
    print('g_fIsRedPitaya: {}'.format(g_fIsRedPitaya))
    print('g_txtTarget: {}'.format(g_txtTarget))
    return render_template ("rp_gui.html", target=g_txtTarget, is_red_pitaya=g_fIsRedPitaya )
   
#------------------------------------------------------------------------------
@app.route("/red_pitaya_setup")
def red_pitaya_setup():
    read_card_setup()
    if (txtHostName == None):
        g_txtTarget = 'Docker Container'
    print ('Target: {}'.format(g_txtTarget ))
    return render_template ("rp_setup.html", target=g_txtTarget)
   
#------------------------------------------------------------------------------
def open_socket(port=5555):
    socket = None
    try:
        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        strAddress = ""
        try:
            strAddress = "tcp://localhost:" % port#5555"
        except Exception as e:
            strAddress = "tcp://127.0.0.1:5555"
            #strAddress = "tcp://localhost:5555"
        #socket.connect("tcp://localhost:5555")
        socket.connect(strAddress)
    except Exception as e:
        print("Runtime error opening socket:\n%s" % e)
    return socket

#------------------------------------------------------------------------------
def message_to_rp (dictCommand, qData):
    try:
        socket = open_socket()
        s = str(json.dumps(dictCommand))
        socket.send_string(s)
        print("\nmessage_to_rp:  message sent\n")
        print(s)
        print("\n")
        message = socket.recv()
        #print("message recieved:\n{}".format(message))
        msg_str = message.decode('utf-8')
        qData.put(msg_str)
    except Exception as e:
        print("Runtime error opening socket:\n%s" % e)
    finally:
        socket.close()

#------------------------------------------------------------------------------
def message_server(dictCommand):
    try:
        msg_str=''
        qData = Queue()
        prcSender = Process(target=message_to_rp, args=(dictCommand, qData,))
        prcSender.start()
        fProcessAlive = prcSender.is_alive()
        dtStart = datetime.now()
        dtDelta = 0
        while ((fProcessAlive == True) & (dtDelta < 1)):
            fProcessAlive = prcSender.is_alive()
            time.sleep(0.1)
            dtDelta = (datetime.now() - dtStart).total_seconds()
            print('Waiting {}'.format(dtDelta))
        if (dtDelta < 1):
            prcSender.join()
            while qData.qsize() > 0:
                msg_str += qData.get()
        else:
            prcSender.terminate()
            msg_str = 'timeout'
    except Exception as e:
        print("Runtime error opening socket:\n%s" % e)
    if (len(msg_str) <= 20):
        print('Message:\n"{}"'.format(msg_str))
    else:
        print('Message length:\n"{}"'.format(len(msg_str)))
    #print('=======================================================')
    #print(msg_str)
    #print('=======================================================')
    return (msg_str)

#------------------------------------------------------------------------------
def client_setup_command (dictCommand):
    txtReply = ''
    try:
        #if (dictCommand['setup'] == 'read'):
        txtReply = message_server(dictCommand)
    except Exception as e:
        txtReply = "Runtime in client_setup_command:\n{}".format(e)
        print(txtReply)
    return (txtReply)

#------------------------------------------------------------------------------
@app.route('/on_red_pitaya_message')
def OnRedPitayaMessage():
    txtReply = 'Red Pitaya Reply'
    res = request.args['message']
    print(res)
    print('type(res): {}'.format(type(res)))
    try:
        res = request.args['message'].lower()
        print('on_red_pitaya_message')
        print('+++++++++++++++++++++++++++++++++++++++++++++')
        print(res)
        print('+++++++++++++++++++++++++++++++++++++++++++++')
        dictCommand = json.loads(res)
        txtReply = message_server(dictCommand)
        dictReply = json.loads(txtReply)
        #d = dictReply
        print(dictReply.keys())
        """
        """
        if 'pulses' in dictReply.keys():
            dictPulses = dictReply['pulses']
            if ('signal' in dictPulses.keys()):
                nSignalLength = int (dictReply['pulses']['signal']['signal_length'])
                nPackageSize = int (dictReply['pulses']['signal']['package_size'])
                aSignal = get_signal_from_hw (nSignalLength, nPackageSize)
                #print('===============================================')
                dictReply['pulses']['signal'] = aSignal#[0:5]
                #print('===============================================')
                txtReply = json.dumps(dictReply)#str(dictReply)
                #print(dictReply['pulses']['signal'])
                #print(txtReply)
                #print('===============================================')
                #print('===============================================')
        """
        """
        #txtReply = str(d)
        #print('+++++++++++++++++++++++++++++++++++++++++++++')
    except Exception as e:
        txtReply = "Runtime error in OnRedPitayaMessage:\n{}".format(e)
        print(txtReply)
    #print('----------------------------------------------')
    #print(txtReply)
    #print('----------------------------------------------')
    return (txtReply)

#------------------------------------------------------------------------------
def get_signal_from_hw (nSignalLength, nPackageSize):
    try:
        dictCommand = dict()
        aSignal = []# * nSignalLength
        socket = open_socket()
        nStart = nRecieved = 0
        dictCommand['signal'] = nPackageSize
        while (nRecieved < nSignalLength):
            dictCommand['start'] = nRecieved#nStart
            s = str(json.dumps(dictCommand))
            #print('===============================================')
            #print(s)
            #print('===============================================')
            socket.send_string(s)
            message = socket.recv()
            msg_str = message.decode('utf-8')
            #print('===============================================')
            #print('Recieved message: {}'.format(len(msg_str)))
            #print('===============================================')
            dictReply = json.loads(msg_str)
            #print(dictReply)
            if ('length' in dictReply.keys()):
            #input('Message Recieved. Enter <CR> to proceed ')
                nRecieved += int(dictReply['length'])
                aTmp = dictReply['signal']
                for n in range(len(aTmp)):
                    aSignal.append(aTmp[n])
                format('Signal length so far: {}'.format(len(aSignal)))
            else:
                nRecieved = nSignalLength
            #input('Added buffer. Enter <CR> to proceed ')
        #print('===============================================')
        #if (len(aSignal) > 1):
            #print('First element: {}'.format(aSignal[0]))
        #print('===============================================')
    except Exception as e:
        print("Runtime error opening socket:\n%s" % e)
        aSignal = None
    finally:
        socket.close()
    return (aSignal)

#------------------------------------------------------------------------------
def client_read_signal (dictCommand):
    txtReply = ''
    try:
        #if (dictCommand['setup'] == 'read'):
        print('client_read_signal')
        txtReply = message_server(dictCommand)
        if (len(txtReply) > 20):
            txtPrintout = txtReply[0:20]
        else:
            txtPrintout = txtReply
        print('reading signal: "{}"'.format(txtPrintout))
    except Exception as e:
        txtReply = "Runtime in client_read_signal :\n{}".format(e)
        print(txtReply)
    return (txtReply)

#------------------------------------------------------------------------------
def read_card_setup():
    socket = open_socket(5555)
    hw = TRedPitayaSetup()
    hw.read_hardware(socket)

#------------------------------------------------------------------------------
@app.route("/websockets")
def load_ws():
    print("Websockets Handler")
    return render_template("ws.html")

#------------------------------------------------------------------------------
@app.route("/other_links")
def other_links():
    return render_template ("rp_setup.html")

#------------------------------------------------------------------------------
if (__name__ == '__main__'):
    #print('argc={}'.format(len(sys.argv)))
    txtHostName = socket.gethostname() + ".local"
    if (len(sys.argv) > 1):
        if (sys.argv[1].lower() == 'docker'):
            txtHostName = '0.0.0.0'
            g_fIsontainer = True
    print('Host Name {}'.format(txtHostName));
    app.run (host=txtHostName, port=5005, debug=True)
    #app.run (host='0.0.0.0', port=5005, debug=True)
    if (socket != None):
        if (hasattr(socket,'close')):
            socket.close()
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
