/******************************************************************************\
|                                  rp_ws.js                                    |
|                           Red Pitaya Web Socket                              |
\******************************************************************************/

/******************************************************************************/

//-----------------------------------------------------------------------------
function onReadRedPitayaSetupClick () {
    var msg = new Object, msgCommand = new Object;
	msgCommand['command'] = 'read';
    msg['setup'] = msgCommand;
    sendMesssageThroughFlask(msg, setupHandler);
	setTimeout (onReadStatusClick, 100);
}

//-----------------------------------------------------------------------------
function onUpdateRedPitayaSetupClick() {
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'update';
    msgTrigger = uploadTriggerSetup ();
    msgCmd['trigger'] = msgTrigger;
    msgCmd['sampling'] = uploadSampling();
	msgCmd['background'] = uploadBackground();
	msgCmd['package_size'] = uploadPackageSize();
    msgCmd['pre_trigger_ns'] = uploadPreTrigger();
    msg['setup'] = msgCmd;//'update';
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function onUpdateCardTriggerClick () {
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'update_trigger';
    msgTrigger = uploadTriggerSetup ();
    msgCmd['trigger'] = msgTrigger;
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function uploadTriggerSetup () {
    var msg=new Object;
	msg['dir'] = uploadTriggerDir ();
	msg['level'] = uploadTriggerLevel ();
	msg['src'] = uploadTriggerSrc ();
	msg['type'] = uploadTriggerType ();
	msg['enabled'] = uploadTriggerOnOff();
	msg['now'] = uploadTriggerNow();
	return (msg);
}

//-----------------------------------------------------------------------------
function uploadTriggerLevel () {
	var dRes;
	try {
		var txt = document.getElementById ('txtTriggerLevel');
		var combo = document.getElementById ('comboTriggerVoltage');
		dRes = parseFloat (txt.value.value) * parseFloat (combo.value);
	}
	catch (exception) {
		alert ('Runtime error in uploadTriggerLevel ' + exception);
		dRed = "";
	}
	return (dLevel);
}

//-----------------------------------------------------------------------------
function uploadTriggerDir () {
	var combo = document.getElementById ('comboTriggerDir');
	return (combo.value);
}

//-----------------------------------------------------------------------------
function uploadTriggerSrc () {
	var combo = document.getElementById ('comboTriggerIn');
	return (combo.value);
}

//-----------------------------------------------------------------------------
function downloadTriggerOnOff (fOnOff) {
	var cbox = document.getElementById ('cboxTriggerEnabled');
	if (cbox != null) {
		cbox.checked = fOnOff;
		enableItemsByTrigger(fOnOff)
	}
}

//-----------------------------------------------------------------------------
function downloadTriggerNow (fNow) {
	var cbox = document.getElementById ('cboxTriggerNow');
	if (cbox != null)
		cbox.checked = fNow;
}

//-----------------------------------------------------------------------------
function uploadTriggerType () {
	var combo = document.getElementById ('comboTriggerType');
	return (combo.value);
}

var webSocket   = null;
var ws_protocol = null;
var ws_hostname = null;
var ws_port     = null;
var ws_endpoint = null;
var g_data = null;

//-----------------------------------------------------------------------------
function setupHandler (reply) {
/*
    var txt = document.getElementById("txtReply");
    if (txt != null)
        txt.value = reply;
*/
    try {
        dictSetup = JSON.parse(reply).setup;
		if (dictSetup.hasOwnProperty('trigger'))
			downloadTrigger(dictSetup.trigger);
		if (dictSetup.hasOwnProperty('sampling')) {
        	DownloadRate (dictSetup.sampling.rate);
        	DownloadDecimation (dictSetup.sampling.decimation);
			downloadPulseDir (dictSetup.sampling.pulse_dir);
		}
		if (dictSetup.hasOwnProperty('background'))
			downloadBackground (dictSetup.background);
		if (dictSetup.hasOwnProperty('package_size'))
			downloadPagckageSize (dictSetup.package_size);
		if (dictSetup.hasOwnProperty('mca'))
			downloadMca (dictSetup.mca);
        if (dictSetup.hasOwnProperty('pre_trigger_ns'))
            downloadPreTrigger(dictSetup.pre_trigger_ns);
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
function downloadPulseDir (txtPulseDir) {
	var rb, txt = txtPulseDir.toLowerCase().trim();
	var rbPositive = document.getElementById ("radioPulsePositive");
	var rbNegative = document.getElementById ("radioPulseNegative");
	if ((rbPositive != null) && (rbNegative != null)) {
		if (txt == "positive")
			rb = rbPositive;
		else
			rb = rbNegative;
		rb.checked = true;
	}
	
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
function downloadBackground (dBackground) {
	var txt = document.getElementById ('txtBackground');
	if (txt != null)
		txt.value = dBackground;
}

//-----------------------------------------------------------------------------
function downloadPagckageSize (package_size) {
	var txt = document.getElementById ('txtPackageSize');
	if (txt != null)
		txt.value = package_size;
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
		var factor=1;
		if (fVal < 1) {
			factor=1e-3;
			fVal *= 1e3;
		}
		selectTriggerVoltageCombo (factor);
        txtLevel.value = fVal;
    }
}

//-----------------------------------------------------------------------------
function selectTriggerVoltageCombo (factor) {
	var combo = document.getElementById('comboTriggerVoltage');
	if (combo != null) {
		if (factor == 1)
			combo.selectedIndex = 1;
		else
			combo.selectedIndex = 0;
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
function measureDataLength () {
    var msg = new Object, msgSignal = new Object;
    msgSignal['signal'] = uploadSignalLength();
    msg['read_data'] = {measure_signal:parseFloat(1e-6)};
    sendMesssageThroughFlask(msg, handleSignalMeasure);
}

//-----------------------------------------------------------------------------
function handleSignalMeasure (reply) {
    try {
        var samples = JSON.parse(reply);
        var aPulseData = samples.pulses.signal;
        console.log(aPulseData.length);
    }
    catch (exception) {
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function onReadSignalClick() {
    var msg = new Object, msgSignal = new Object;
    if (uploadSignalRead())
        msgSignal['signal'] = uploadSignalLength();
    //if (uploadMcaRead())
        msgSignal['mca'] = uploadMcaRead();
    //if (uploadPsdRead())
        msgSignal['psd'] = uploadPsdRead();
    msg['read_data'] = msgSignal;
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
    //return (uploadCheckBox ("cboxStartMCA"));
    return (uploadCheckBox ("cboxReadMCA"));
}

//-----------------------------------------------------------------------------
function uploadPsdRead() {
    return (uploadCheckBox ("cboxStartPSD"));
    //return (uploadCheckBox ("cboxReadPSD"));
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

//-----------------------------------------------------------------------------
function setupReadSignal (reply) {
    var cell = document.getElementById("cellSignal");
    try {
        var txt, n, i, samples = JSON.parse(reply);
        var yData=[], yRaw=[], xData=[], t=0, yTrigger=[], yBackground=[];
        var dTrigger = uploadTriggerLevel (), dBackground=uploadBackground();
		var aPulseData = null;//samples.pulses.signal;//.pulse;
        var aMcaData = null;//samples.pulses.mca;

        var layout = {};
        if (samples.pulses.hasOwnProperty("mca"))
            aMcaData = samples.pulses.mca;
        if (samples.pulses.hasOwnProperty('signal')) {
			console.log('signal accepted');
            aPulseData = samples.pulses.signal;
		}
        if (samples.pulses.hasOwnProperty('buffer_length'))
			downloadBufferLength(samples.pulses.buffer_length);
        if (aPulseData != null)
            plotSignal (aPulseData);
        if (aMcaData != null)
            plotMca (aMcaData);
    }
    catch (exception) {
        var txt = cell.innerText;
        txt = exception;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function downloadBufferLength(value) {
	var txtbx = document.getElementById("txtCardBuffer");
	txtbx.value = value;
}

//-----------------------------------------------------------------------------
function plotSignal (aPulseData) {
	var layout = {}, yData=[], t, yTrigger=[], yBackground=[];
    var yData=[], yRaw=[], xData=[], t=0, yTrigger=[], yBackground=[];
    var t, dTrigger = uploadTriggerLevel (), dBackground=uploadBackground();

	layout["title"] = "Signal";
	layout["xaxis"] = {};
	layout["yaxis"] = {};
	layout['autosize'] = true;
		//layout.xaxis["title"] = "Time [uSec]";
	layout.yaxis["title"] = "Voltage";
	var mrgn = {};
	var left_title = {};
	left_title ['text'] = "Time [uSec]";
	left_title ['font'] = {size:12};
	layout.xaxis["title"] = left_title;//"Time [uSec]";
	mrgn['l'] = 50;
	mrgn['r'] = 50;
	mrgn['b'] = 25;
	mrgn['t'] = 25;
	mrgn['pad'] = 1;
	layout['margin'] = mrgn;
    for (var n=0 ; n < aPulseData.length ; n++, t += 8e-9) {
        yData[n] = parseFloat (aPulseData[n]);
            //yRaw[n] = parseFloat (aPulseRaw[n]);
        xData[n] = t;
        yTrigger[n] = dTrigger;
		yBackground[n] = dBackground;
    }
        //var cbox = document.getElementById('cboxTrigger');
        //var fShowTrigger=false;
	var fShowTrigger = uploadCheckbox ('cboxTrigger');
	var fShowBackground = uploadCheckbox ('cboxBackground');
        //if (cbox)
            //fShowTrigger = cbox.checked;
    var dataPulse = {x:xData, y:yData, name: "Filtered"};
        //var dataRaw = {x:xData, y:yRaw, name: "Raw"};
    var dataTrigger = {x:xData, y:yTrigger, name: "Trigger"};
    var dataBackground = {x:xData, y:yBackground, name: "Background"};
    var data=[];
        //data[0] = dataRaw;//dataPulse;
    data[0] = dataPulse;
		//data.push(dataPulse);
    if (fShowTrigger)
        data.push(dataTrigger);
    if (fShowBackground)
        data.push(dataBackground);
    var chart = document.getElementById("chartSignal");
    Plotly.newPlot(chart, data, layout);
}

//-----------------------------------------------------------------------------
function uploadBackground() {
	var txt = document.getElementById ('txtBackground');
	return (parseFloat(txt.value));
}

//-----------------------------------------------------------------------------
function uploadPackageSize() {
	var txt = document.getElementById ('txtPackageSize');
	return (parseFloat(txt.value));
}

//-----------------------------------------------------------------------------
function uploadCheckbox (cboxId) {
	var f = false;
	var cbox = document.getElementById (cboxId);
	if (cbox != null)
		f = cbox.checked;
	return (f);
}

//-----------------------------------------------------------------------------
function uploadTriggerLevel () {
    var dTrigger = parseFloat(document.getElementById("txtTriggerLevel").value);
	var combo = document.getElementById ('comboTriggerVoltage');
	var factor = parseFloat(combo.item(combo.selectedIndex).value);
    if (isNaN(dTrigger))
        dTrigger = 0;
	else
		dTrigger *= factor;
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
		var t=document.getElementById("txtBufferLength");
		t.value = jReply.sampling.buffer;
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

//-----------------------------------------------------------------------------
function uploadSampling() {
    var msgSampling = new Object;
    msgSampling["rate"] = uploadRate();
    msgSampling["decimation"] = uploadDecimation();
    msgSampling["pulse_dir"] = uploadPulseDir();
    return (msgSampling);
}

//-----------------------------------------------------------------------------
function uploadCombo(txtComboId) {
    var val='';
    var combo = document.getElementById(txtComboId);
    if (combo != null)
        val = combo.value;
    return (val);
}

//-----------------------------------------------------------------------------
function uploadRate()
{
    return (uploadCombo('comboRate'));
}

//-----------------------------------------------------------------------------
function uploadDecimation() {
    return (uploadCombo('comboDecimation'))
}

//-----------------------------------------------------------------------------
function uploadPulseDir() {
	var txtDir = "negative";
	var rad = document.getElementById ("radioPulsePositive");
	if (rad != null)
		if (rad.checked)
			txtDir = "positive";
	return (txtDir);
}

//-----------------------------------------------------------------------------
function onMcaUpdateClick() {
    var msg = new Object, msgMca = new Object, msgCmd = new Object;
    //msgCmd['command'] = 'update';
    msgCmd['mca'] = uploadMcaParams ();
    msgCmd['command'] = 'update';
    msg['setup'] = msgCmd;//'update';
	console.log(JSON.stringify(msg));
    sendMesssageThroughFlask(msg, mcaSetupHandler);
}

//-----------------------------------------------------------------------------
function uploadMcaParams () {
    var msgMca = new Object;
    msgMca['channels'] = uploadMcaChannels();
    msgMca['min_voltage'] = uploadMcaMinVoltage();
    msgMca['max_voltage'] = uploadMcaMaxVoltage();
    return (msgMca);
}

//-----------------------------------------------------------------------------
function uploadMcaChannels() {
    return (uploadTextReal ('txtMcaChannels'));
}

//-----------------------------------------------------------------------------
function uploadMcaMinVoltage() {
    return (uploadTextReal ('txtMcaMin'));
}

//-----------------------------------------------------------------------------
function uploadMcaMaxVoltage() {
    return (uploadTextReal ('txtMcaMax'));
}

//-----------------------------------------------------------------------------
function uploadTextReal (txtId) {
    var val = null;
    var txt = document.getElementById (txtId);
    if (txt != null)
        val = txt.value;
    return (val);
}

//-----------------------------------------------------------------------------
function mcaSetupHandler (reply) {
/*
    var txt = document.getElementById("txtReply");
    if (txt != null)
        txt.value = reply;
*/
    try {
        dictSetup = JSON.parse(reply).setup.mca;
		downloadMca (dictSetup.mca);
    }
    catch (err) {
        console.log(err);
    }
}

//-----------------------------------------------------------------------------
function downloadMca (dictMca) {
	try {
        donwloadText ('txtMcaChannels', dictMca.channels);
        donwloadText ('txtMcaMin', dictMca.min_voltage);
        donwloadText ('txtMcaMax', dictMca.max_voltage);
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function donwloadText (txtId, val) {
    var txt = document.getElementById (txtId);
    if (txt != null)
        txt.value = val;
}

//-----------------------------------------------------------------------------
function plotMca (aMca) {
	try {
    	var xData=[], yData=[]

        for (var n=0 ; n < aMca.length ; n++) {
            xData[n] = n + 1;
            yData[n] = aMca[n];
        }
		//console.log('Maximum: ' + fMax + ', at ' + iMax);
        var dataMca = {x:xData, y:yData, type: 'bar'};
	    var layout = {};
        var data=[];
        data[0] = dataMca;
    	layout["title"] = "MCA";
    	layout["xaxis"] = {};
    	layout["yaxis"] = {};
    	layout.xaxis["title"] = "Energy";
    	layout.yaxis["title"] = "Count";
    	var chart = document.getElementById("chartMca");
    	Plotly.newPlot(chart, data, layout);
	}
	catch (exception) {
		console.log(exception);
		console.log(aMca.length);
	}
}

//-----------------------------------------------------------------------------
function uploadTriggerOnOff() {
    return (uploadCheckBox ("cboxTriggerEnabled"));
}

//-----------------------------------------------------------------------------
function uploadTriggerNow() {
    return (uploadCheckBox ("cboxTriggerNow"));
}

//-----------------------------------------------------------------------------
function onTriggerEnabledClick() {
	try {
    	var fTriggerEnabled = uploadCheckBox ("cboxTriggerEnabled");
		enableItemsByTrigger(fTriggerEnabled);
/*
		enableItem ("comboTriggerDir", fTriggerEnabled);
		enableItem ("comboTriggerIn", fTriggerEnabled);
		enableItem ("comboTriggerType", fTriggerEnabled);
		enableItem ("txtTriggerLevel", fTriggerEnabled);
		enableItem ("comboTriggerVoltage", fTriggerEnabled);
		enableItem ("btnUpdateTrigger", fTriggerEnabled);
		enableItem ("btnTriggerNow", fTriggerEnabled);
*/
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function enableItemsByTrigger(fTriggerEnabled) {
	try {
		enableItem ("comboTriggerDir", fTriggerEnabled);
		enableItem ("comboTriggerIn", fTriggerEnabled);
		enableItem ("comboTriggerType", fTriggerEnabled);
		enableItem ("txtTriggerLevel", fTriggerEnabled);
		enableItem ("comboTriggerVoltage", fTriggerEnabled);
		//enableItem ("btnUpdateTrigger", fTriggerEnabled);
		//enableItem ("btnTriggerNow", fTriggerEnabled);
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function enableItem (txtItem, fEnabled) {
	var item = document.getElementById (txtItem);
	item.disabled = fEnabled ? false : true;
}

//-----------------------------------------------------------------------------
function onReadCardTriggerClick () {
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'read_trigger';
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function downloadTrigger(dictTriggerSetup) {
	try {
        downloadTriggerLevel (dictTriggerSetup.level);
        downloadTriggerDir (dictTriggerSetup.dir);
        downloadTriggerSrc (dictTriggerSetup.src);
		downloadTriggerOnOff (dictTriggerSetup.enabled);
		downloadTriggerNow (dictTriggerSetup.now);
		//enableItemsByTrigger(dictTriggerSetup.enabled);
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function onTriggerNowClick () {
/*
	onUpdateCardTriggerClick ();
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'trigger_now';
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, setupHandler);
*/
}
//-----------------------------------------------------------------------------
function sendBackgroundCommand(bkgnd_cmd) {
    var msg = new Object, msgCmd = new Object;
    msgCmd['background'] = bkgnd_cmd;
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, handlerBackground);
}

//-----------------------------------------------------------------------------
function onSetBackgroundClick() {
    var bkgnd = uploadBackground();
    sendBackgroundCommand ({'set' : bkgnd});
}

//-----------------------------------------------------------------------------
function onReadBackgroundClick() {
    sendBackgroundCommand ('read');
}

//-----------------------------------------------------------------------------
function onMeasureBackgroundClick() {
    sendBackgroundCommand ('measure');
}

//-----------------------------------------------------------------------------
function handlerBackground (reply) {
/*
    var txt = document.getElementById("txtReply");
    if (txt != null)
        txt.value = reply;
*/
    try {
        var dictSetup = JSON.parse(reply).setup;
		if (dictSetup.hasOwnProperty('background'))
			downloadBackground (dictSetup.background);
    }
    catch (err) {
        console.log(err);
    }
}
//-----------------------------------------------------------------------------
function onCardBufferLength () {
    var msg = new Object, msgSignal = new Object;
	msgSignal['buffer'] = 'read';
    msg['read_data'] = msgSignal;
	sendMesssageThroughFlask(msg, setupCardBuffer);
}

//-----------------------------------------------------------------------------
function onCardBufferClear () {
    var msg = new Object, msgSignal = new Object;
	msgSignal['buffer'] = 'reset';
    msg['read_data'] = msgSignal;
	sendMesssageThroughFlask(msg, setupReadSignal );
}

//-----------------------------------------------------------------------------
function setupCardBuffer (reply) {
    var txtbx = document.getElementById("txtCardBuffer");
    try {
		var jReply = JSON.parse (reply);
		txtbx.value = jReply.pulses.buffer;//sampling.buffer
 	}
    catch (exception) {
        var txt = cell.innerText;
        txt = exception;
        console.log(exception);
    }
}


//-----------------------------------------------------------------------------
function onPreTriggerSet() {
}

//-----------------------------------------------------------------------------
function onPreTriggerGet() {
}

//-----------------------------------------------------------------------------
function uploadPreTrigger() {
    var txtbx = document.getElementById("txtPreTriggerNS");
    var pre;
    try {
        pre = parseInt (txtbx.value);
    }
    catch (exception) {
        pre = 0;
    }
    return (pre);
}

//-----------------------------------------------------------------------------
function downloadPreTrigger(val) {
    var txtbx = document.getElementById("txtPreTriggerNS");
    txtbx.value = val;
}

//-----------------------------------------------------------------------------
