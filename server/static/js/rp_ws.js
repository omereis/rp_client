/******************************************************************************\
|                                  rp_ws.js                                    |
|                           Red Pitaya Web Socket                              |
\******************************************************************************/

/******************************************************************************/

//-----------------------------------------------------------------------------
function onReadRedPitayaSetupClick () {
    var msg = new Object;
    //msg['setup'] = 'read';
    msg["sampling"] = "true";
    sendMesssageThroughFlask(msg, setupHandler);
}

var webSocket   = null;
var ws_protocol = null;
var ws_hostname = null;
var ws_port     = null;
var ws_endpoint = null;
/**
 * Event handler for clicking on button "Connect"
 */

//-----------------------------------------------------------------------------
function setupHandler (reply) {
    var txt = document.getElementById("txtReply");
    if (txt != null)
        txt.value = reply;
    try {
        dictSetup = JSON.parse(reply);
        DownloadRate (dictSetup.sampling.rate);
        DownloadDecimation (dictSetup.sampling.decimation);
        downloadTriggerLevel (dictSetup.trigger.level);
        downloadTriggerDir (dictSetup.trigger.dir);
        downloadTriggerSrc (dictSetup.trigger.src);
    }
    catch (err) {
        console.log(err);
    }
}

//-----------------------------------------------------------------------------
function DownloadRate (rate) {
    IntToCombo ("comboRate", parseInt(rate));
}

//-----------------------------------------------------------------------------
function DownloadDecimation (decimation)
{
    IntToCombo ("comboDecimation", parseInt(decimation));
}

//-----------------------------------------------------------------------------
function downloadTriggerDir (dir) {
    txtToCombo ("comboTriggerDir", dir);
}

//-----------------------------------------------------------------------------
function downloadTriggerSrc (src) {
    txtToCombo ("comboTriggerActivation", src);
}

//-----------------------------------------------------------------------------
function downloadTriggerSrc (src) {
    txtToCombo ("comboTriggerIn", src);
}

//-----------------------------------------------------------------------------
function downloadTriggerLevel (level) {
    var txtLevel = document.getElementById("txtTriggerLevel");
    if (txtLevel != null) {
        var fVal = parseFloat(level);
        txtLevel.value = fVal * 1e3;
    }
}

//-----------------------------------------------------------------------------
function txtToCombo (txtCombo, txtValue)
{
    var combo = document.getElementById(txtCombo);
    var idx = findOptionByTxt (combo, txtValue);
    if (idx >= 0)
        combo.selectedIndex = idx;
}

//-----------------------------------------------------------------------------
function IntToCombo (txtCombo, nValue)
{
    var combo = document.getElementById(txtCombo);
    var idx = FindOptionByInt (combo, nValue);
    if (idx >= 0)
        combo.selectedIndex = idx;
}

//-----------------------------------------------------------------------------
function FindOptionByInt (combo, nVal) {
    var n, idx=-1;

    for (n=0 ; (n < combo.options.length) && (idx < 0) ; n++) {
		var opt = combo.options[n].value;
		if (opt.length > 0)
        	if (nVal == parseInt(opt))
            	idx = n;
	}
    return (idx);
}

//-----------------------------------------------------------------------------
function findOptionByTxt (combo, txtValue) {
    var n, idx=-1;

    for (n=0 ; (n < combo.options.length) && (idx < 0) ; n++) {
		var opt = combo.options[n].value;
		if (opt.length > 0) {
        	if (txtValue.toLowerCase() == opt.toLowerCase())
            	idx = n;
        }
    }
    return (idx);
}
//-----------------------------------------------------------------------------
function sendMesssageThroughFlask(message, handler=null) {
    $.ajax({
        url: "/on_red_pitaya_message",
        type: "get",
        data: {message: JSON.stringify(message)},
        success: function(response) {
            try {
                var reply;
                if (typeof(response) != 'string')
                    reply = JSON.parse(response);
                else
                    reply = response;
                console.log(JSON.stringify(reply));
                if (handler != null)
                    handler(reply);
                //handle_reply(reply);
            }
            catch (err) {
                console.log(err);
            }
        },
        error: function(xhr) {
            alert('error:\n' + xhr.toString());
            console.log(xhr.toString());
        }
    });
}
//-----------------------------------------------------------------------------
function onConnectClick() {
    var ws_protocol = document.getElementById("protocol").value;
    var ws_hostname = document.getElementById("hostname").value;
    var ws_port     = document.getElementById("port").value;
    var ws_endpoint = document.getElementById("endpoint").value;
    openWSConnection(ws_protocol, ws_hostname, ws_port, ws_endpoint);
}
/**
 * Event handler for clicking on button "Disconnect"
 */
function onDisconnectClick() {
    webSocket.close();
}
/**
 * Open a new WebSocket connection using the given parameters
 */
function openWSConnection(protocol, hostname, port, endpoint) {
    var webSocketURL = null;
    webSocketURL = protocol + "://" + hostname + ":" + port + endpoint;
    console.log("openWSConnection::Connecting to: " + webSocketURL);
    try {
        webSocket = new WebSocket(webSocketURL);
        webSocket.onopen = function(openEvent) {
            console.log("WebSocket OPEN: " + JSON.stringify(openEvent, null, 4));
            document.getElementById("btnSend").disabled       = false;
            document.getElementById("btnConnect").disabled    = true;
            document.getElementById("btnDisconnect").disabled = false;
        };
        webSocket.onclose = function (closeEvent) {
            console.log("WebSocket CLOSE: " + JSON.stringify(closeEvent, null, 4));
            document.getElementById("btnSend").disabled       = true;
            document.getElementById("btnConnect").disabled    = false;
            document.getElementById("btnDisconnect").disabled = true;
        };
        webSocket.onerror = function (errorEvent) {
            console.log(webSocketURL);
            console.log("WebSocket ERROR: " + JSON.stringify(errorEvent, null, 4));
        };
        webSocket.onmessage = function (messageEvent) {
            var wsMsg = messageEvent.data;
            console.log("WebSocket MESSAGE: " + wsMsg);
            if (wsMsg.indexOf("error") > 0) {
                document.getElementById("incomingMsgOutput").value += "error: " + wsMsg.error + "\r\n";
            } else {
                document.getElementById("incomingMsgOutput").value += "message: " + wsMsg + "\r\n";
            }
        };
    } catch (exception) {
        console.error(exception);
    }
}
/**
 * Send a message to the WebSocket server
 */
function onSendClick() {
    if (webSocket.readyState != WebSocket.OPEN) {
        console.error("webSocket is not open: " + webSocket.readyState);
        return;
    }
    var msg = document.getElementById("message").value;
    webSocket.send(msg);
}

//-----------------------------------------------------------------------------
function onReadSignalClick() {
    var msg = new Object;
    var msgBuf = new Object;
    msg['sampling'] = 'True';
    msgBuf['buffer_length'] = "5";
    msg['read_pulse'] = msgBuf;
    sendMesssageThroughFlask(msg, setupReadSignal);
}

//-----------------------------------------------------------------------------
function onReadStatusClick () {
    var msg = new Object;
    msg['sampling'] = 'status';
    sendMesssageThroughFlask(msg, readSamplingStatus);
}

//-----------------------------------------------------------------------------
function onSamplingOn() {
    var msg = new Object;
    msg['sampling'] = 'true';
    sendMesssageThroughFlask(msg, readSamplingStatus);
}

//-----------------------------------------------------------------------------
function onSamplingOff() {
    var msg = new Object;
    msg['sampling'] = 'false';
    sendMesssageThroughFlask(msg, readSamplingStatus);
}

//-----------------------------------------------------------------------------
function setupReadSignal (reply) {
    var cell = document.getElementById("cellSignal");
    //if (cell != null)
        //cell.innerText = reply;
    try {
        var txt, n, i, samples = JSON.parse(reply);
        var nPulses = samples.sampling.pulse_count.toString();
        var aPulses = samples.pulses;
        var aKeys = Object.keys(aPulses);
        for (n=0 ; n < aKeys.length ; n++) {
            txt = '';
            var pulse = aPulses[aKeys[n]];
            for (i=0 ; i < pulse.length ; i++) {
                txt += parseFloat (pulse[i]).toString();
                if (i < pulse.length-1)
                    txt += ",";
            }
            cell.innerHTML += txt + "<br>";
        }
    }
    catch (exception) {
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function readSamplingStatus (reply) {
    var p = document.getElementById("cellStatus");
    try {
		var jReply = JSON.parse(reply).sampling;
        var txt, cl, status = jReply.sampling;
        if (status == true) {
            cl = 'green';
            txt = 'On';
        }
        else {
            cl = 'red';
            txt = 'Off';
        }
        p.style.backgroundColor = cl;
		if (jReply.hasOwnProperty("pulse_count"))
			txt += " (" + jReply.pulse_count.toString() + ")";
        p.innerText = txt;
    }
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}
//-----------------------------------------------------------------------------
