import zmq
from flask import Flask,render_template, request
from RedPitayaSetup import TRedPitayaSetup
import sys, socket, json, time
from datetime import datetime
from multiprocessing import Process, Queue

app = Flask(__name__)
txtHostName = None

#------------------------------------------------------------------------------
@app.route("/")
def index():
    return render_template ("rp_gui.html")
   
#------------------------------------------------------------------------------
@app.route("/red_pitaya_setup")
def red_pitaya_setup():
    read_card_setup()
    return render_template ("rp_setup.html")
   
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
            strAddress = "tcp://localhost:5555"
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
        print("message sent")
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
        #socket = open_socket()
        #s = str(json.dumps(dictCommand))
        #socket.send_string(s)
        #print("message sent")

        #message = socket.recv()
        #print("message recieved:\n{}".format(message))
        #msg_str = message.decode('utf-8')
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
            print('Message:\n"{}"'.format(msg_str))
        else:
            prcSender.terminate()
            msg_str = 'timeout'
    except Exception as e:
        print("Runtime error opening socket:\n%s" % e)
    #finally:
        #socket.close()
    print('Reply: "{}"'.format(msg_str))
    return (msg_str)

#------------------------------------------------------------------------------
def client_setup_command (dictCommand):
    txtReply = ''
    try:
        if (dictCommand['setup'] == 'read'):
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
        print(res)
        dictCommand = json.loads(res)
        if 'setup' in dictCommand:
            txtReply = client_setup_command (dictCommand)
        if 'sampling' in dictCommand:
            txtReply = client_read_signal (dictCommand)
    except Exception as e:
        txtReply = "Runtime error in OnRedPitayaMessage:\n{}".format(e)
        print(txtReply)
    return (txtReply)

#------------------------------------------------------------------------------
def client_read_signal (dictCommand):
    txtReply = ''
    try:
        #if (dictCommand['setup'] == 'read'):
        txtReply = message_server(dictCommand)
        print('reading signal\n{}'.format(txtReply))
    except Exception as e:
        txtReply = "Runtime in client_setup_command:\n{}".format(e)
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
    print('Host Name {}'.format(txtHostName));
    app.run (host=txtHostName, port=5005, debug=True)
    #app.run (host='0.0.0.0', port=5005, debug=True)
    if (socket != None):
        socket.close()
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
