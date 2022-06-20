/******************************************************************************\
|                                  rp_ws.js                                    |
|                           Red Pitaya Web Socket                              |
\******************************************************************************/

/******************************************************************************/

//-----------------------------------------------------------------------------
function onReadRedPitayaSetupClick () {
    var msg = new Object;
    msg['setup'] = 'read';
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function onUpdateRedPitayaSetupClick () {

}

var webSocket   = null;
var ws_protocol = null;
var ws_hostname = null;
var ws_port     = null;
var ws_endpoint = null;
var g_data = null;

//-----------------------------------------------------------------------------
function setupHandler (reply) {
    var txt = document.getElementById("txtReply");
    if (txt != null)
        txt.value = reply;
    try {
        dictSetup = JSON.parse(reply).setup;
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
    var txt = document.getElementById("txtBuffer");
    var msg = new Object, msgSignal = new Object, msgRead = new Object;
    if (uploadSignalRead()) {
        msgSignal['signal'] = uploadSignalLength();
    }
    if (uploadMcaRead()) {
        msgSignal['mca'] = uploadMcaRead();
    }
    if (uploadPsdRead()) {
        msgSignal['psd'] = uploadPsdRead();
    }
    //msgRead['mca'] = uploadMcaRead();
    //msgRead['psd'] = uploadPsdRead();
    //msgSignal['buffer_length'] = txt.value;//"100";
    //msgSignal['units'] = "microseconds";
    //msgSignal['signal'] = uploadSignalRead();
    //msg['signal'] = msgSignal
    msg['read_data'] = msgSignal;
    //msg['read_pulse'] = msgBuf;
    //var msg_mca = new Object;
    //msg_mca['mca_op'] = uploadMcaOp();
    //msg['mca'] = msg_mca;
    if (Object.keys(msgSignal).length > 0)
        sendMesssageThroughFlask(msg, setupReadSignal);
}

//-----------------------------------------------------------------------------
function uploadSignalLength() {
    var dBufLen=100e-6;

    try {
        var dLen = parseFloat(document.getElementById("txtBuffer").value);
        var combo = document.getElementById("comboSignalBufferUnits");
        var dCoef = parseFloat(combo.item(combo.selectedIndex).value);
        dBufLen = dLen * dCoef;
    }
    catch (exception) {
        console.log(exception);
    }
    return (dBufLen);
}

//-----------------------------------------------------------------------------
function uploadSignalRead() {
    return (uploadCheckBox ("cboxReadSignal"));
}

//-----------------------------------------------------------------------------
function uploadMcaRead() {
    return (uploadCheckBox ("cboxReadMCA"));
}

//-----------------------------------------------------------------------------
function uploadPsdRead() {
    return (uploadCheckBox ("cboxReadPSD"));
}

//-----------------------------------------------------------------------------
function uploadCheckBox (txtCbox) {
    var fSignal=false;
    try {
        fSignal = document.getElementById(txtCbox).checked;
    }
    catch (exception) {
        console.log(exception);
    }
    return (fSignal);
}

//-----------------------------------------------------------------------------
function uploadMcaOp() {
    var mca_op='';
    try {
        var cbox = document.getElementById(cboxShowMca);
        if (cbox != null)
            mca_op = cbox.checked ? 'start' : 'stop';
        else
            mca_op = 'stop';
    }
    catch (exception) {
        console.log(exception);
        mca_op = "stop";
    }
    return (mca_op);
}

//-----------------------------------------------------------------------------
function onReadStatusClick () {
    var msgStatus = new Object;
    msgStatus['op'] = 'status';
    sendSamplingCommand (msgStatus);
}

//-----------------------------------------------------------------------------
function onSamplingOn() {
    sendSamplingCommand ('true');
}

//-----------------------------------------------------------------------------
function onSamplingOff() {
    sendSamplingCommand ('false');
}

//-----------------------------------------------------------------------------
function onSamplingUpdate () {
    var msgSignal = new Object;
    msgSignal['signal'] = uploadSignalOnOff ();
    msgSignal['mca'] = uploadMcaOnOff ();
    msgSignal['psd'] = uploadPsdOnOff ();
    sendSamplingCommand (msgSignal);
}

//-----------------------------------------------------------------------------
function uploadSignalOnOff () {
    return (uploadCheckBox ("cboxStartSignal"));
}

//-----------------------------------------------------------------------------
function uploadMcaOnOff () {
    return (uploadCheckBox ("cboxStartMCA"));
}

//-----------------------------------------------------------------------------
function uploadPsdOnOff () {
    return (uploadCheckBox ("cboxStartPSD"));
}

//-----------------------------------------------------------------------------
function sendSamplingCommand (cmd) {
    var msg = new Object;
    var msgSignal = new Object;
    msgSignal['signal'] = cmd;
    msg['sampling'] = cmd;
    sendMesssageThroughFlask(msg, readSamplingStatus);
}
/*
*/
//-----------------------------------------------------------------------------
function setupReadSignal (reply) {
    var cell = document.getElementById("cellSignal");
    //if (cell != null)
        //cell.innerText = reply;
    try {
        var txt, n, i, samples = JSON.parse(reply);
        var yData=[], xData=[], t=0, yTrigger=[];
        var dTrigger = uploadTriggerLevel ();//parseFloat(document.getElementById("txtTriggerLevel").value);
        var nPulses = samples.pulses.signal.length;//samples.sampling.pulse_count.toString();
		var layout = {};
		layout["title"] = "Signal";
		layout["xaxis"] = {};
		layout["yaxis"] = {};
		layout.xaxis["title"] = "Time [uSec]";
		layout.yaxis["title"] = "Voltage";
        for (var n=0 ; n < nPulses ; n++, t += 8e-9) {
            yData[n] = parseFloat (samples.pulses.signal[n]);
            xData[n] = t;
            yTrigger[n] = dTrigger;
        }
        var cbox = document.getElementById('cboxTrigger');
        var fShowTrigger=false;
        if (cbox)
            fShowTrigger = cbox.checked;
        //var data = [[{x:xData, y:yData}], [{x:xData, y:yTrigger}]];
        var dx = {x:xData, y:yData, name: "Signal"};
        var dy = {x:xData, y:yTrigger, name: "Trigger"};
        //var data = [[{x:xData, y:yData}]];//, [{x:xData, y:yTrigger}]];
        var data=[];
        data[0] = dx;
        if (fShowTrigger)
            data[1] = dy;
        var chart = document.getElementById("chartSignal");
        Plotly.newPlot(chart, data, layout);
        }
    catch (exception) {
		var txt = cell.innerText;
		txt = exception;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function uploadTriggerLevel () {
    var dTrigger = parseFloat(document.getElementById("txtTriggerLevel").value);
    if (isNaN(dTrigger))
        dTrigger = 0;
    return (dTrigger);
}

//-----------------------------------------------------------------------------
function readSamplingStatus (reply) {
    var p = document.getElementById("cellStatus");
    try {
		var jReply = JSON.parse(reply);//.sampling;
		downloadCheckBox (jReply.sampling.status.signal, "cboxStartSignal");
		downloadCheckBox (jReply.sampling.status.mca, "cboxStartMCA");
		downloadCheckBox (jReply.sampling.status.psd, "cboxStartPSD");
        var cl, status = jReply.sampling.status.signal;
        if (status == true) {
            cl = 'green';
        }
        else {
            cl = 'red';
        }
        p.style.backgroundColor = cl;
		//if (jReply.hasOwnProperty("pulse_count"))
			//txt += " (" + jReply.pulse_count.toString() + ")";
        //p.innerText = txt;
    }
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function downloadCheckBox (fStatus, txtId) {
	var combo = document.getElementById (txtId);
	if (combo != null)
		combo.checked = fStatus;
}

//-----------------------------------------------------------------------------
function drawSignal(data) {
    var chart = new CanvasJS.Chart("chartContainer", {
        animationEnabled: true,
        zoomEnabled: true,
        title:{
            text: "Try Zooming and Panning" 
        },
        data: data  // random generator below
    });
    chart.render();
}

//-----------------------------------------------------------------------------
function onChartClick() {
	try {
		var chartTESTER = document.getElementById('divChart');
		Plotly.newPlot( chartTESTER, [{
						x: [1, 2, 3, 4, 5],
						y: [1, 2, 4, 8, 16] }], {
						margin: { t: 0 } } );
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function onSignalStartStopClick(id) {
    try {
        var btn = document.getElementById(id);
        if (btn != null) {
            var fStart = (btn.value.toLowerCase() == "start");
            if (fStart) {
                var interval = parseInt(document.getElementById("txtInterval").value);
                btn.value = "Stop";
                var id = setInterval (onReadSignalClick, interval);
                localStorage.setItem("SignalInterval", id);
            }
            else {
                btn.value = "Start";
                var id = localStorage.getItem ("SignalInterval");
                clearInterval (id);
            }
        }
    }
    catch (exception) {
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function onQuitSampling() {
    var msgStatus = new Object;
    msgStatus['op'] = 'quit';
    onReadStatusClick();
}

//-----------------------------------------------------------------------------
function onMcaResetClick() {

}
