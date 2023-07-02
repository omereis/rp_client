/******************************************************************************\
|                                  rp_ws.js                                    |
|                           Red Pitaya Web Socket                              |
\******************************************************************************/

/******************************************************************************/

const RawBuffer       = 1;
const ProcessedBuffer = 2;
//-----------------------------------------------------------------------------
function onPageLoad () {
	try {
		localStorage.removeItem ('MCA_Data');
		localStorage.removeItem ("update_handle");
		var hUpdate = localStorage.getItem ("update_handle");
		document.getElementById('cboxReadPSD').disabled = true;
		//var FileSaver = require('file-saver');
	}
	catch (exception) {
		alert ('Runtime error in uploadTriggerLevel ' + exception);
	}
	//onReadRedPitayaSetupClick ();
	//onReadStatusClick();
}

//-----------------------------------------------------------------------------
function onReadRedPitayaSetupClick () {
    var msg = new Object, msgCommand = new Object;
	msgCommand['command'] = 'read';
    msg['setup'] = msgCommand;
	document.body.style.cursor = 'wait';
    sendMesssageThroughFlask(msg, setupHandler);
	//setTimeout (onReadStatusClick, 100);
}

//-----------------------------------------------------------------------------
function onUpdateRedPitayaSetupClick() {
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'update';
    msgTrigger = uploadTriggerSetup ();
    msgCmd['trigger'] = msgTrigger;
	msgCmd['mca'] = uploadMcaParams();
    msgCmd['sampling'] = uploadSampling();
	msgCmd['background'] = uploadBackground();
	msgCmd['package_size'] = uploadPackageSize();
    msgCmd['pre_trigger_ns'] = uploadPreTrigger();
	msgCmd['trapez'] = uploadTrapezParams();
	msgCmd['remote_processing'] = uploadRemoteProcessing();
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
		document.body.style.cursor = 'default';
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
			downloadMcaParams (dictSetup.mca);
        if (dictSetup.hasOwnProperty('pre_trigger_ns'))
            downloadPreTrigger(dictSetup.pre_trigger_ns);
		if (dictSetup.hasOwnProperty('trapez'))
            downloadTrapez(dictSetup.trapez);
		if (dictSetup.hasOwnProperty('version'))
            downloadVersion(dictSetup.version);
		if (dictSetup.hasOwnProperty('remote_processing'))
            downloadRemoteProceesing(dictSetup.remote_processing);
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
		txt.value = toDisplayVolt (dBackground);
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
                //console.log(JSON.stringify(reply));
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
    var msg = new Object, msgRead=new Object, msgSignal = new Object;
    if (uploadSignalRead()) {
        msgSignal['length'] = uploadSignalLength();
		msgSignal['debug'] = uploadCheckBox ("cboxDebug");
		msgSignal['kernel'] = uploadCheckBox ("cboxKernel");
		msgSignal['filtered'] = uploadCheckBox ("cboxFiltered");
		msgSignal['deriv'] = uploadCheckBox ("cboxDeriv");
	}
    //if (uploadMcaRead())
	msgRead['signal'] = msgSignal;
	msgRead['mca'] = uploadMcaRead();
    //if (uploadPsdRead())
    msgRead['psd'] = uploadPsdRead();
    msg['read_data'] = msgRead;
    if (Object.keys(msgSignal).length > 0) {
		localStorage.removeItem ("chart_debug");
        sendMesssageThroughFlask(msg, setupReadSignal);
	}
}

//-----------------------------------------------------------------------------
function GetDebugChecked()
{
	var fChecked = false;
	var cbox = document.getElementById ("cboxDebug");
	if (cbox != null)
		fChecked = cbox.checked;
	return (fChecked);
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
    //var msgStatus = new Object;
    //msgStatus['op'] = 'status';
    //sendSamplingCommand (msgStatus);
    var msgSignal = new Object;
	msgSignal['status'] = true;

    //msgSignal['signal'] = uploadSignalOnOff ();
    //msgSignal['mca'] = uploadMcaOnOff ();
    //msgSignal['mca_time'] = uploadTextReal ('txtMcaTimeLimit');
    //msgSignal['psd'] = uploadPsdOnOff ();
    sendSamplingCommand (msgSignal);
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
function sendSamplingUpdate (fSampling, fMca, txtMcaTime, fPsd) {
    var msgSignal = new Object;
    msgSignal['signal'] = fSampling;//uploadSignalOnOff ();
    msgSignal['mca'] = fMca;//uploadMcaOnOff ();
    msgSignal['mca_time'] = txtMcaTime;//uploadTextReal ('txtMcaTimeLimit');
    msgSignal['psd'] = fPsd;//uploadPsdOnOff ();
    sendSamplingCommand (msgSignal);
}

//-----------------------------------------------------------------------------
function onSamplingUpdate () {
    var msgSignal = new Object;
    msgSignal['signal'] = uploadSignalOnOff ();
    msgSignal['mca'] = uploadMcaOnOff ();
    msgSignal['mca_time'] = uploadTextReal ('txtMcaTimeLimit');
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
function periodicStatus() {
	onReadStatusClick();
	console.log (new Date());
}

//-----------------------------------------------------------------------------
function setReadPeriodicCardStatus (fOnOff) {
	try {
		if (fOnOff) {
			var hUpdate = localStorage.getItem ("update_handle");
			if (hUpdate == null) {
				var nRate = uploadTextAsFloat ('txtUpdateRate');
				var hUpdate = setInterval (periodicStatus, 1000);
				localStorage.setItem ("update_handle", hUpdate);
			}
	
			//txt.value = timeStart;
		}
		else {
			var hUpdate = localStorage.getItem ("update_handle");
			if (hUpdate != null)
				clearInterval (hUpdate);
			localStorage.removeItem ("update_handle");
		}
	}
	catch (e) {
		console.log(e);
	}
}

//-----------------------------------------------------------------------------
function sendSamplingCommand (cmd) {
    var msg = new Object;
    var msgSignal = new Object;
    msgSignal['signal'] = cmd;
    msg['sampling'] = cmd;
	
/*
	if (!cmd.signal) {
		setReadPeriodicCardStatus (false);
		var hUpdate = localStorage.getItem ("update_handle");
		if (hUpdate == null) {
			var nRate = uploadTextAsFloat ('txtUpdateRate');

			var hUpdate = setInterval (periodicStatus, 1000);
			localStorage.setItem ("update_handle", hUpdate);
		}
		//txt.value = timeStart;
	}
	else {
		var hUpdate = localStorage.getItem ("update_handle");
		if (hUpdate != null)
			clearInterval (hUpdate);
		localStorage.removeItem ("update_handle");

	}
*/
	//console.log(JSON.stringify(msg));
    sendMesssageThroughFlask(msg, readSamplingStatus);
}

//-----------------------------------------------------------------------------
function updateOnTime () {
	var tStart = localStorage.getItem ("start_time");
	if (tStart != null) {
		var tNow = new Date();
		var tDiff = (tNow.getTime() - tStart) / 1000;
		var txt = document.getElementById ("txtOnTime");
		txt.value = tDiff;
	}
}

//-----------------------------------------------------------------------------
function setupReadSignal (reply) {
    var cell = document.getElementById("cellSignal");
    try {
        var txt, n, i, samples = JSON.parse(reply);
        var yData=[], yRaw=[], xData=[], t=0, yTrigger=[], yBackground=[];
        var dTrigger = uploadTriggerLevel (), dBackground=uploadBackground();
		var aPulseData = null, aPulsesIndices=null;//samples.pulses.signal;//.pulse;
		var aFiltered = [];//null;
        var aMcaData = null;//samples.pulses.mca;

        var layout = {};
        if (samples.pulses.hasOwnProperty("mca"))
            aMcaData = samples.pulses.mca;
        if (samples.pulses.hasOwnProperty('signal')) {
			console.log('signal accepted');
            aPulseData = samples.pulses.signal.data;
			downloadSignalMinMax (samples.pulses.signal);
            if (samples.pulses.signal.hasOwnProperty ('detector_pulse'))
				aPulsesIndices = samples.pulses.signal.detector_pulse;
		}
		if (samples.pulses.signal.hasOwnProperty('filtered'))
			aFiltered = samples.pulses.signal.filtered;
        if (samples.pulses.hasOwnProperty('buffer_length'))
			downloadBufferLength(RawBuffer, samples.pulses.buffer_length);
        //if (samples.pulses.hasOwnProperty('mca_length'))
			//downloadMcaLength(samples.pulses.mca_length);
        if (samples.pulses.hasOwnProperty('background'))
			downloadBackground (samples.pulses.background);
			//downloadMeasuredBackground(samples.background);
        if (aPulseData != null)
			plotSignal (samples.pulses.signal);
			//plotSignal (aPulseData, aFiltered, aPulsesIndices);
		if (aMcaData != null)
			downloadMca (aMcaData);
			//plotMca (aMcaData);
	}
	catch (exception) {
		var txt = cell.innerText;
		txt = exception;
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function downloadBufferLength(target, value) {
	var txtbx;

	if (target == ProcessedBuffer)
		txtbx = document.getElementById("txtCardProcessBuffer");
	else
		txtbx = document.getElementById("txtCardRawBuffer");
	txtbx.value = value;
	txtbx.style.background = 'Yellow';
	setInterval (clearBufferBkgnd, 1000);
}

//-----------------------------------------------------------------------------
function pad(num, size) {
// source: https://stackoverflow.com/questions/2998784/how-to-output-numbers-with-leading-zeros-in-javascript
	num = num.toString();
	while (num.length < size)
		num = "0" + num;
	return (num);
}

//-----------------------------------------------------------------------------
function formatSSecondsAsTime(sec_num) {
    //var sec_num = parseInt(this, 10); // don't forget the second param
    var hours   = Math.floor(sec_num / 3600);
    var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
    var seconds = sec_num - (hours * 3600) - (minutes * 60);

	hours = pad (hours, 2);
	minutes = pad (minutes, 2);
	seconds = pad (seconds , 2);
	var txtTime;
	try {
		var txtSeconds = parseFloat(seconds).toFixed(1);
		txtSeconds = txtSeconds.padStart(4, '0');
    	txtTime = hours+':'+ minutes+':'+ txtSeconds;
	}
	catch (exception) {
    	txtTime = exception;
	}
	return (txtTime);
}

//-----------------------------------------------------------------------------
function downloadMcaLength(txtBuffer, txtMcaTime) {
	var txtbx = document.getElementById("txtMcaBuffer");
	txtbx.value = txtBuffer;
	document.getElementById('txtMcaRuntime').value = formatSSecondsAsTime (txtMcaTime);
}

//-----------------------------------------------------------------------------
function clearBufferBkgnd () {
	var txtbx = document.getElementById("txtCardRawBuffer");
	txtbx.style.background = 'White';
}

//-----------------------------------------------------------------------------
function plotSignal (sample_signal){
//function plotSignal (aPulseData, aFiltered, aPulsesIndices=null){
	if (sample_signal.hasOwnProperty ('data'))
		aPulseData = sample_signal.data;
	if (sample_signal.hasOwnProperty ('filtered'))
		aFiltered = sample_signal.filtered;

	if (sample_signal.hasOwnProperty ('detector_pulse'))
		aPulsesIndices = sample_signal.detector_pulse;
	//if (sample_signal.hasOwnProperty ('aPulsesIndices'))
		//aPulsesIndices = sample_signal.aPulsesIndices;
	else
		aPulsesIndices = null;

	var yData=[], t, yTrigger=[], yBackground=[];
    var yRaw=[], xData=[], xBackground=[], t=0, yTrigger=[], yBackground=[], yPulses=null;
    var t, dTrigger = uploadTriggerLevel (), dBackground=uploadBackground(), dMin;
	var fIsMilli = uploadRadio ('radioMillivolts');

	var idxPulses=0;
    for (var n=0 ; n < aPulseData.length ; n++) {
		var y = parseFloat (aPulseData[n]);
		if (fIsMilli)
			y *= 1000;
        yData[n] = y;
	}
	if (aPulsesIndices != null) {
		yPulses = []
		dMin = vector_min (yData);
	}
	var dt = uploadSamplingRate();
    for (var n=0 ; n < aPulseData.length ; n++, t += 8e-9) {
        xData[n] = t;
        yTrigger[n] = dTrigger;
		if (yPulses != null) {
			yPulses[n] = dMin * IsIndexInPulse (n, aPulsesIndices);
		}
    }
	xBackground[0] = xData[0];
	xBackground[1] = xData[xData.length - 1];
	yBackground[0] = yBackground[1] = dBackground;

	var fShowTrigger = uploadCheckbox ('cboxTrigger');
	var fShowBackground = uploadCheckbox ('cboxBackground');
    var dataPulse = {x:xData, y:yData, name: "Signal"};

    var dataTrigger = {x:xData, y:yTrigger, name: "Trigger"};
    var dataBackground = {x:xBackground, y:yBackground, name: "Background"};

    var data=[];
    data[0] = dataPulse;
	if (uploadCheckbox ('cboxFiltered')) {
		if (uploadCheckbox ('cboxNormFilt')) {
			var dFiltMin = arrayMin (aFiltered);
			var dDataMin = arrayMin (yData);
			var n, dFactor;
			dFactor = dDataMin / dFiltMin;
			for (n=0 ; n < aFiltered.length ; n++)
				aFiltered[n] *= dFactor;
			//dMin = yData[0];
		}
		if (aFiltered.length > 0) {
    		var dataFiltered = {x:xData, y:aFiltered, name: "Filtered"};
    		data[1] = dataFiltered;
		}
	}
    if (fShowTrigger)
        data.push(dataTrigger);
    if (fShowBackground)
        data.push(dataBackground);
	if (yPulses != null) {
    	var dataPulse = {x:xData, y:yPulses, name: "Debug"};
        data.push(dataPulse);
	}
	if (uploadCheckbox ('cboxDeriv')) {
		if (sample_signal.hasOwnProperty('filt_deriv')) {
			if (sample_signal.filt_deriv.length > 0) {
    			var dataDerive = {x:xData, y:sample_signal.filt_deriv, name: "Derivative"};
    			data.push(dataDerive);
			}
		}
	}
/*
*/
	//data.push(dataPulse);
	//data.push(dataFiltered);
	localStorage.setItem ("chart_debug", yPulses);
    var chart = document.getElementById("chartSignal");
	saveDataToLocal (data);
    Plotly.newPlot(chart, data, GetSignalChartLayout(fIsMilli));
}

//-----------------------------------------------------------------------------	
function arrayMin (arr) {
	var n, dMin;

	try {
		dMin = arr[0];
		for (n=1 ; n < arr.length ; n++)
			if (dMin > arr[n])
				dMin = arr[n];
			//dMin = Math.min(arr[n]);
	}
    catch (exception) {
		dMin = 0;
        console.log(exception);
	}
	return (dMin);
}

//-----------------------------------------------------------------------------	
function saveDataToLocal (data) {
	var n, strNames, item;

	strNames = "";
	for (n=0 ; n < data.length ; n++) {
		strNames += data[n].name;
		if (n < data.length - 1)
			strNames += ",";
		item = data[n];
		localStorage.setItem (data[n].name, data[n].y);
	}
	localStorage.setItem("chart_names", strNames);
	strNames += ""; // for breakpoint
}

//-----------------------------------------------------------------------------
function onSaveFileDataClick() {
	try {
		var strNames = localStorage.getItem("chart_names");
		var ay=[], y, astr = strNames.split(","), astrData=[];

		for (var n=0 ; n < astr.length ; n++) {
			y = localStorage.getItem(astr[n]);
			ay[n] = y.split(',');
			localStorage.removeItem(astr[n]);
		}
		for (n=0 ; n < ay[0].length ; n++) {
			str = "";
			for (y=0 ; y < ay.length ; y++) {
				str += ay[y][n];
				if (y < ay.length - 1)
					str += ",";
			}
			astrData[n] = str + "\n";
		}
// source: https://code-boxx.com/create-save-files-javascript/

		download("chart.csv", astrData.toString());
		localStorage.removeItem("chart_names");
	}
    catch (exception) {
        console.log(exception);
	}
	console.log(astr.length);
	console.log(astrData.length);
}

//-----------------------------------------------------------------------------
function download(filename, text) {
// Source:
// https://stackoverflow.com/questions/3665115/how-to-create-a-file-in-memory-for-user-to-download-but-not-through-server
	var element = document.createElement('a');
	element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
	element.setAttribute('download', filename);
  
	element.style.display = 'none';
	document.body.appendChild(element);
  
	element.click();
  
	document.body.removeChild(element);
}

//-----------------------------------------------------------------------------
function createBlob(data) {
  return new Blob([data], { type: "text/plain" });
}

//-----------------------------------------------------------------------------
function plotKernal (vKernel) {
	var div = document.getElementById ('chartKernel');
	if (div == null) {
		var divParent = document.getElementById ('chartKernalParent');
		div = document.createElement("div");
		divParent.appendChild(div);
	}
	var vx=[vKernel.length];
	var vy=[vKernel.length];
	for (var n=0 ; n < vKernel.length ; n++) {
		vx[n] = n;
		//vy[n] = vKernel[n];
	}
	var trace2 = {
		x: vx,
		//y: vy,
		y: vKernel,
		type: 'Kernel'
	};
	var data = [trace2];
	Plotly.newPlot('chartKernel', data);
}

//-----------------------------------------------------------------------------
function deleteKernelPlot() {
	var div = document.getElementById ('chartKernel');
	div.remove();
	//Plotly.deleteTraces (dic, 0);
}

//-----------------------------------------------------------------------------
function GetSignalChartLayout(fIsMilli) {
	var layout = {};

	if (fIsMilli)
		strLeft = "milli";
	else
		strLeft = "";
	strLeft += "Volts";
	layout["title"] = "Signal";
	layout["xaxis"] = {};
	layout["yaxis"] = {};
	layout['autosize'] = true;
		//layout.xaxis["title"] = "Time [uSec]";
	layout.yaxis["title"] = "Voltage [" + strLeft + "]";
	//layout.yaxis["title"] = "Voltage [Volts]";
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
	return (layout);
}

//-----------------------------------------------------------------------------
function vector_min (yData)
{
	var n, dMin=0;
	if (yData.length > 0) {
		dMin = yData[0];
		for (n=1 ; n < yData.length ; n++)
			if (yData[n] < dMin)
				dMin = yData[n];
	}
	return (dMin);
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
function setStartStop (fVal, idCheckbox, idTxt) {
	downloadCheckBox (fVal, idCheckbox);
	var txtbx = document.getElementById (idTxt);
	if (txtbx)
		txtbx.value = (fVal ? "Stop" : "Start");
}

//-----------------------------------------------------------------------------
function readSamplingStatus (reply) {
    var p = document.getElementById("cellStatus");
    try {
		var jReply = JSON.parse(reply);//.sampling;
		setStartStop (jReply.sampling.status.signal, 'cboxStartSignal', 'btnSamplingStartStop');
		setStartStop (jReply.sampling.status.mca, 'cboxStartMCA', 'btnMCAStartStop');

		downloadCheckBox (jReply.sampling.status.psd, "cboxStartPSD");
		if (jReply.sampling.hasOwnProperty('process_buffer'))
			downloadBufferLength(ProcessedBuffer , jReply.sampling.process_buffer);
		if (jReply.sampling.hasOwnProperty('raw_buffer'))
			downloadBufferLength(RawBuffer, jReply.sampling.raw_buffer);
		if (jReply.sampling.hasOwnProperty('mca_buffer'))
			downloadMcaLength(jReply.sampling.mca_buffer, jReply.sampling.mca_time);
        var cl, status = jReply.sampling.status.signal;
        if (status == true) {
            cl = 'green';
        }
        else {
            cl = 'red';
        }
        p.style.backgroundColor = cl;
		//if (jReply.sampling.status.signal)
			//setReadPeriodicCardStatus (true);
    }
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function toggleButtonStartStop (idBtn) {
	var id = document.getElementById (idBtn);
	var fNewVal;
	if (id != null) {
		var txtCaption = id.value.toLowerCase();
		if (txtCaption == "start")
			fNewVal = true;
		else
			fNewVal = false;
		txtCaption = (fNewVal ? "Stop" : "Start");
		id.value = txtCaption;
	}
	return (fNewVal);
}

//-----------------------------------------------------------------------------
function onSamplingStartStop() {
	var fStartStop = toggleButtonStartStop ('btnSamplingStartStop');
	setReadPeriodicCardStatus (fStartStop);
	sendSamplingUpdate (fStartStop, uploadMcaOnOff(), uploadMcaTimeLimit(), uploadPsdOnOff ());
}

//-----------------------------------------------------------------------------
function onMcaStartStop() {
	var fStartStop = toggleButtonStartStop ('btnMCAStartStop');
	setReadPeriodicCardStatus (fStartStop);
	sendSamplingUpdate (fStartStop, fStartStop, uploadMcaTimeLimit(), uploadPsdOnOff ());
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
async function getNewFileHandle() {
//async function getNewFileHandle() {
	const opts = {
		types: [{
			description: 'Text file',
			accept: {'text/plain': ['.txt']},
		}],
	};
	return await window.showSaveFilePicker(opts);
}

//-----------------------------------------------------------------------------
function download(filename, text) {
	var element = document.createElement('a');
	element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
	element.setAttribute('download', filename);

	element.style.display = 'none';
	document.body.appendChild(element);

	element.click();

	document.body.removeChild(element);
}

//-----------------------------------------------------------------------------
function onQuitSampling() {
    var msgStatus = new Object;
    msgStatus['op'] = 'quit';
    onReadStatusClick();
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
function uploadTextReal (txtId) {
    var val = null;
    var txt = document.getElementById (txtId);
    if (txt != null)
        val = txt.value;
    return (val);
}

//-----------------------------------------------------------------------------
function mcaSetupHandler (reply) {
    try {
        dictSetup = JSON.parse(reply).setup.mca;
		downloadMca (dictSetup.mca);
    }
    catch (err) {
        console.log(err);
    }
}

//-----------------------------------------------------------------------------
function downloadMcaParams (dictMca) {
	try {
        donwloadText ('txtMcaChannels', dictMca.channels);
		downloadTextVoltage ('txtMcaMin', parseFloat(dictMca.min_voltage));
		downloadTextVoltage ('txtMcaMax', parseFloat(dictMca.max_voltage));
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
function downloadMca (dictMca) {
	try {
		downloadRealValue ('txtSignalMin', dictMca.mca_min, 3);
		downloadRealValue ('txtSignalMax', dictMca.mca_max, 3);
		downloadRealValue ('txtSignalsCount', dictMca.mca_count);
		if (dictMca.hasOwnProperty ('mca_data'))
			plotMca (dictMca.mca_data, dictMca.mca_count);
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function plotMca (aMca, nCount) {
	try {
    	var xData=[], yData=[]
		var dMax, nMax;

        for (var n=0 ; n < aMca.length ; n++) {
			if (n == 0) {
				nMax = n;
				dMax = aMca[n];
			}
			else {
				if (dMax < aMca[n]) {
					nMax = n;
					dMax = aMca[n];
				}
			}
            xData[n] = n + 1;
            yData[n] = aMca[n];
        }
		console.log ('Maximum: ' + dMax + ', @index ' + nMax);
        var dataMca = {x:xData, y:yData, type: 'bar'};
	    var layout = {};
        var data=[];
        data[0] = dataMca;
		var dMcaChannels = uploadTextReal ('txtMcaChannels');
		var txtTitle = uploadTextValue('txtChartTitle');
		if (txtTitle.length > 0)
    		layout["title"] = txtTitle + " (" + nCount.toString() + " Counts, " + dMcaChannels.toString() + " Channels)";
		else
    		layout["title"] = "MCA (" + dMcaChannels.toString() + ")";
    	layout["xaxis"] = {};
    	layout["yaxis"] = {};
    	layout.xaxis["title"] = "Channel";
    	layout.yaxis["title"] = "Count";
		//layout.xaxis.autorange = false;
		//layout.xaxis.range[0] = 0;
		//layout.xaxis.range[1] = dMcaChannels;// 2050;
    	var chart = document.getElementById("chartMca");
    	Plotly.newPlot(chart, data, layout);
		chart.layout.xaxis.autorange = false;
		chart.layout.xaxis.range[0] = 0;
		chart.layout.xaxis.range[1] = dMcaChannels;// 2050;
		Plotly.redraw('chartMca');
		localStorage.setItem ('MCA_Data', aMca.toString());
	}
	catch (exception) {
		console.log(exception);
		console.log(aMca.length);
	}
}

//-----------------------------------------------------------------------------
function IsIndexInPulse (idx, aPulsesIndices) {
	var n, res=0;

	if (aPulsesIndices != null) {
		for (n=0 ; (n < aPulsesIndices.length) && (res == 0) ; n++)
			if ((idx >= aPulsesIndices[n].start) && (idx < aPulsesIndices[n].start + aPulsesIndices[n].length))
				res = 1;
	}
	return (res);
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
		downloadTriggerType (dictTriggerSetup.type);
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
function findComboItem (txtValue, txtCombo) {
	var n, nFound=-1, txtItem;
	txtValue = txtValue.toLowerCase();
	var combo = document.getElementById (txtCombo);
	if (combo != null) {
		n = 0;
		while (nFound < 0) {
			txtItem = combo.item(n);
			if (txtItem == null)
				break;
			else {
				txtItem = txtItem.value.toLowerCase();
				if (txtItem == txtValue)
					nFound = n;
			}
			n++;
		}
	}
	return (nFound);
}

//-----------------------------------------------------------------------------
function downloadTriggerType (txtType) {
	var combo = document.getElementById ('comboTriggerType');
	var idx = findComboItem (txtType, 'comboTriggerType');
	if (idx >= 0)
		combo.selectedIndex = idx;
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
function onCardRawBufferLength () {
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
	sendMesssageThroughFlask(msg, setupReadSignal);
}

//-----------------------------------------------------------------------------
function setupCardBuffer (reply) {
    var txtbx = document.getElementById("txtCardRawBuffer");
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
function uploadTextAsFloat (idTxt) {
    var txtbx = document.getElementById(idTxt);
	var val;
	if (txtbx != null)
		val = parseFloat (txtbx.value);
	return (val);
}

//-----------------------------------------------------------------------------
function uploadTextAsInt (idTxt) {
    var txtbx = document.getElementById(idTxt);
	var val;
	if (txtbx != null)
		val = parseInt (txtbx.value);
	return (val);
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
function onDownloadMcaParams() {
    var msg = new Object, msgMca = new Object;
    msgMca['mca'] = uploadMcaParams();
    msgMca['command'] = 'update';
    msg['setup'] = msgMca
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function onUploadMcaParams() {
    var msg = new Object, msgTrigger=new Object, msgCmd = new Object;
    msgCmd['command'] = 'read_mca_params';
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function onResetMca() {
	onDownloadMcaParams();
}

//-----------------------------------------------------------------------------
function uploadTextValue(txtInputId) {
    var txt = document.getElementById(txtInputId);
    var val;
    if (txt != null)
        val = txt.value;
    return (val);
}
//-----------------------------------------------------------------------------
function uploadMcaParams() {
    var msgMca = new Object;
    //msgMca['max_voltage'] = uploadTextValue('txtMcaMax');
    msgMca['max_voltage'] = uploadTextBoxVoltage ('txtMcaMax');
    msgMca['min_voltage'] = uploadTextBoxVoltage ('txtMcaMin');
    //msgMca['min_voltage'] = uploadTextValue('txtMcaMin');
    msgMca['channels'] = uploadTextValue('txtMcaChannels');
	return (msgMca);
}

//-----------------------------------------------------------------------------
function uploadSignalChart() {
	var n, ar=[];
	for (n=0 ; n < 100 ; n++) {
		var line = String (n+1) + "," + String(Math.random() * 100) + "\n";
		ar.push(line);
	}
	return (ar);
}

//-----------------------------------------------------------------------------
function uploadChartData(txtChartName) {
    var chart = document.getElementById(txtChartName);
	var n, nChart=0, line = '', aLines=[];

	if (chart.hasOwnProperty('data')) {
        aLines.push('x,y\n');
	    for (n=0 ; n < chart.data[nChart].x.length ; n++) {
		    line = String (chart.data[nChart].x[n]) + "," + String(chart.data[nChart].y[n]) + "\n";
			aLines.push(line);
		}
	}
	return (aLines);
}

//-----------------------------------------------------------------------------
function onSaveFileClick() {
	saveChartToFile('chartSignal');
}

//-----------------------------------------------------------------------------
function saveChartToFile(txtChart) {
/*
Source:
https://code.tutsplus.com/tutorials/how-to-save-a-file-with-javascript--cms-41105
*/
	var tempLink = document.createElement("a");
	var value = new Blob (uploadChartData(txtChart));
	//var value = new Blob (uploadChartData('chartSignal'));
	tempLink.setAttribute('href', URL.createObjectURL(value));
	tempLink.setAttribute('download', 'signal.csv');
	tempLink.click();
	URL.revokeObjectURL(tempLink.href);
}

//-----------------------------------------------------------------------------
function onDebugCboxClick() {
    var chart = document.getElementById("chartSignal");
    var fDebug = uploadCheckBox ("cboxDebug");
	if (fDebug) {
		var yPulses = localStorage.getItem ("chart_debug");
		if (yPulses != null) {
			//var x=[];
			//for (var n=0 ; n < chart.data[0].x.length ; n++)
				//x[n] = chart.data[0].x[n];
    		var dataPulses = {x:chart.data[0].x, y:yPulses, name: "Debug"};
			chart.data.push(dataPulses);
			Plotly.redraw('chartSignal');
		}
	}
	else {
		if (chart.hasOwnProperty ('data')) {
			var idx = findLineChartByTitle (chart.data, "Debug");
			if (idx >= 0) {
				chart.data.splice(idx,1);
				Plotly.redraw('chartSignal');
			}
		}
	}
}

//-----------------------------------------------------------------------------
function findLineChartByTitle (chart_data, strTitle) {
	var n, idx=-1;

	try {
		for (n=0 ; (n < chart_data.length) && (idx < 0) ; n++) {
			var plot = chart_data[n];
			if (plot.name == strTitle)
				idx = n;
		}
	}
    catch (exception) {
		console.log(exception);
    }
	return (idx);
}

//-----------------------------------------------------------------------------
function onWindowResize() {
	Plotly.redraw('chartSignal');
}

//-----------------------------------------------------------------------------
function getBackgroundLine() {
	var xBackground=[], yBackground=[], dataBackground=null;
	var dBackground = uploadBackground();
    var chart = document.getElementById("chartSignal");
	if (chart.hasOwnProperty('data')) {
		var data_x = chart.data[0].x;
		xBackground[0] = data_x[0];
		xBackground[1] = data_x[data_x.length - 1];
		yBackground[0] = yBackground[1] = dBackground;
    	dataBackground = {x:xBackground, y:yBackground, name: "Background"};
	}
	return (dataBackground);
}

//-----------------------------------------------------------------------------
function onBackgroundClick() {
    var chart = document.getElementById("chartSignal");
    var fBkgnd = uploadCheckBox ("cboxBackground");

	if (fBkgnd) {
		var dataBackground = getBackgroundLine();
		if (dataBackground != null) {
			chart.data.push(dataBackground);
			Plotly.redraw('chartSignal');
		}
	}
	else {
		var idx = findLineChartByTitle (chart.data, "Background");
		if (idx >= 0) {
			chart.data.splice(idx,1);
			Plotly.redraw('chartSignal');
		}
	}
}

//-----------------------------------------------------------------------------
function uploadRadio (idRadio) {
	var radio = document.getElementById (idRadio);
	var fRadio = false;

	if (radio != null)
		fRadio = radio.checked;
	return (fRadio);
}
			
//-----------------------------------------------------------------------------
function downloadSignalMinMax (dictSignal) {
	if (dictSignal.hasOwnProperty('data_min'))
		downloadTextByFunction (dictSignal.data_min, 'txtSignalMin', Math.min);
	if (dictSignal.hasOwnProperty('data_max'))
		downloadTextByFunction (dictSignal.data_max, 'txtSignalMax', Math.max);
}

//-----------------------------------------------------------------------------
function toDisplayVolt (dValue) {
	var dDisplay;

	if (isMillivolts ())
		dDisplay = (dValue * 1000).toFixed(3);
	else
		dDisplay = dValue;
	return (dDisplay);
}

//-----------------------------------------------------------------------------
function isMillivolts () {
	return (uploadRadio ('radioMillivolts'));
}
			
//-----------------------------------------------------------------------------
function downloadTextByFunction (value, idTxt, func) {
	try {
		var dNew = parseFloat(value);
		var txt = document.getElementById (idTxt);
		if (txt != null) {
			var d, val = uploadTextVoltage (txt);//txt.value;
			if (val.length > 0)
				d = func (dNew, parseFloat(val));
			else
				d = dNew;
			txt.value = toDisplayVolt (d);
		}
	}
    catch (exception) {
		console.log('Runtime error in "downloadTextByFunction":\n' + exception);
    }
}

//-----------------------------------------------------------------------------
function uploadTextBoxVoltage (idTxt) {
	var txtbx = document.getElementById (idTxt);
	if (txtbx != null)
		return (uploadTextVoltage(txtbx));
	else
		return (null);
}

//-----------------------------------------------------------------------------
function uploadTextVoltage (txt) {
	var val = parseFloat (txt.value);
	if (isMillivolts ())
		val /= 1000;
	return (val);
}

//-----------------------------------------------------------------------------
function downloadTextVoltage (idTxt, dValue) {
	var txt = document.getElementById (idTxt);
	if (txt != null)
		txt.value = toDisplayVolt (dValue);
}

//-----------------------------------------------------------------------------
function onVoltsMilliClick(idClickedRadio) {
	convertVoltageUnits (idClickedRadio, 'txtSignalMin');
	convertVoltageUnits (idClickedRadio, 'txtSignalMax');
	convertVoltageUnits (idClickedRadio, 'txtBackground');
	updateSignalPlot (idClickedRadio == 'radioVolts');
	convertVoltageUnits (idClickedRadio, 'txtMcaMax');
	convertVoltageUnits (idClickedRadio, 'txtMcaMin');
}

//-----------------------------------------------------------------------------
function convertVoltageUnits (idClickedRadio, idTxt) {
	var dValue = uploadTextReal (idTxt);
	if (idClickedRadio == 'radioVolts')
		dValue /= 1000;
	downloadTextVoltage (idTxt, dValue);
}

//-----------------------------------------------------------------------------
function updateSignalPlot (fToVolts)
{
    var chart = document.getElementById("chartSignal");
	var yNew=[], n, d;

	if (chart.hasOwnProperty('data')) {
	    for (n=0 ; n < chart.data[0].x.length ; n++) {
		    d = chart.data[0].y[n];
			if (fToVolts)
				d /= 1000;
			else
				d *= 1000;
			yNew[n] = d;
		}
		chart.data[0].y = yNew;
		chart.layout.yaxis.title.text = getVoltageTitle (!fToVolts);
		Plotly.redraw('chartSignal');
	}
	console.log(yNew.length);
}

//-----------------------------------------------------------------------------
function getVoltageTitle (fToMilliVolts) {
	var sz = "Voltage [";
	if (fToMilliVolts)
		sz += "milli";
	sz += "volts]";
	return (sz);
}

//-----------------------------------------------------------------------------
function onMcaStartClick() {
    var msgSignal = new Object;
    msgSignal['signal'] = true;//uploadSignalOnOff ();
    msgSignal['mca'] = true;//uploadMcaOnOff ();
    msgSignal['mca_time'] = uploadMcaTimeLimit();
    //msgSignal['mca_time'] = uploadTextReal ('txtMcaTimeLimit');
    sendSamplingCommand (msgSignal);
}

//-----------------------------------------------------------------------------
function onSetMcaTimeLimit() {
	var fMcaUnlimited = uploadRadio ('radioMcaTimeLimit');
	enableItem ("txtMcaTimeLimit", fMcaUnlimited);
	enableItem ("radioMcaTimeMin", fMcaUnlimited);
	enableItem ("radioMcaTimeSec", fMcaUnlimited); 
}

//-----------------------------------------------------------------------------
function uploadMcaTimeLimit() {
	var fMcaUnlimited = uploadRadio ('radioMcaTimeLimit');
    var dSec = uploadTextReal ('txtMcaTimeLimit');
	try {
		if (fMcaUnlimited) {
			var radio = document.getElementById ('radioMcaTimeMin');
			if (radio != null)
				if (radio.checked)
					dSec *= 60;
		}
		else
			dSec = -1;
	}
    catch (exception) {
		console.log('Runtime error in "uploadMcaTimeLimit":\n' + exception);
    }
	return (dSec);
	consol.log(dSec);
}

//-----------------------------------------------------------------------------
function onMcaStopClick() {
    var msgSignal = new Object;
    msgSignal['signal'] = false;//uploadSignalOnOff ();
    msgSignal['mca'] = false;//uploadMcaOnOff ();
    sendSamplingCommand (msgSignal);
}

//-----------------------------------------------------------------------------
function uploadTrapezoidOnOff() {
	var val="";
	var btn = document.getElementById("btnTrapezOnOff");
	if (btn != null)
		val = btn.value.toLowerCase();
	if (val == "on")
		val = true;
	else
		val = false;
	return (val);
}

		
//-----------------------------------------------------------------------------
function downloadTrapezOnOff (txtOnOff) {
	var btn = document.getElementById("btnTrapezOnOff");
	var val;// = txtOnOff.toLowerCase();
	if (btn != null) {
		if (txtOnOff)
			val = "On";
		else
			val = "Off";
		btn.value = val;
	}
}

//-----------------------------------------------------------------------------
function onTrapezoidOnOff() {
	var btn = document.getElementById("btnTrapezOnOff");
	if (btn != null) {
		var val = uploadTrapezoidOnOff();
		if (val)// == "on")
			btn.value = "Off";
		else
			btn.value = "On";
	}
}

//-----------------------------------------------------------------------------
function onTrapezoidApply() {
    var msg = new Object, msgCmd = new Object;
    msgCmd['command'] = 'update';
	msgCmd["trapez"] = uploadTrapezParams ();
    msg['setup'] = msgCmd;//'update';
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function onTrapezoidLoad() {
    var msg = new Object, msgCmd = new Object;
    msgCmd['command'] = 'read_trapez';
    msg['setup'] = msgCmd;
    sendMesssageThroughFlask(msg, setupHandler);
}

//-----------------------------------------------------------------------------
function uploadTrapezParams () {
	var msgTrapez = new Object;
	msgTrapez["rise"]   = uploadRiseTime ();
	msgTrapez["fall"]   = uploadFallTime ();
	msgTrapez["on"]     = uploadOnTime ();
	msgTrapez["on_off"] = uploadTrapezoidOnOff();
	msgTrapez["height"] = uploadHeight ();
	msgTrapez["factor"] = uploadTrapezFactor();
	return (msgTrapez);
}

//-----------------------------------------------------------------------------
function uploadDouble (id_txt) {
	var d, txtbx = document.getElementById(id_txt);
	if (txtbx != null)
		d = parseFloat (txtbx.value);
	else
		d = 0;
	return (d);
}

//-----------------------------------------------------------------------------
function uploadRiseTime () {
	return (uploadDouble ("txtbxTrapezRise") * 1e-9);
}

//-----------------------------------------------------------------------------
function uploadFallTime () {
	return (uploadDouble ("txtbxTrapezFall") * 1e-9);
}

//-----------------------------------------------------------------------------
function uploadOnTime () {
	return (uploadDouble ("txtbxTrapezOn") * 1e-9);
}

//-----------------------------------------------------------------------------
function uploadHeight () {
	return (uploadDouble ("txtbxTrapezHeight"));
}

//-----------------------------------------------------------------------------
function uploadTrapezFactor() {
	return (uploadDouble ("txtbxTrapezFactor"));
}
//-----------------------------------------------------------------------------
function handleTrapez (reply) {
    var p = document.getElementById("cellStatus");
    try {
		var jReply = JSON.parse(reply);//.sampling;
    }
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function downloadTrapez(dictTrapez) {
	try {
		downloadRealValue ('txtbxTrapezRise', parseInt ((dictTrapez.rise * 1e9) + 0.5));
		downloadRealValue ('txtbxTrapezFall', parseInt ((dictTrapez.fall * 1e9) + 0.5));
		downloadRealValue ('txtbxTrapezOn', parseInt ((dictTrapez.on * 1e9) + 0.5));
		//downloadRealValue ('txtbxTrapezHeight', parseInt (dictTrapez.height + 0.5));
		downloadRealValue ('txtbxTrapezHeight', dictTrapez.height.toFixed(2));
		downloadRealValue ('txtbxTrapezFactor', dictTrapez.factor);
		downloadTrapezOnOff (dictTrapez.on_off);
	}
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function downloadVersion(txtVersion) {
	try {
		var pr = document.getElementById ("parVersion");
		pr.innerText = txtVersion;
		//pr.innerText = "(Version " + txtVersion + ")";
	}
    catch (exception) {
		var p = document.getElementById ("txtReply");
		if (p != null)
			p.value = reply;
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function downloadRealValue (idTxt, dValue, nDigits=-1) {
	var txtbx = document.getElementById(idTxt);
	if (txtbx != null) {
		if (nDigits < 0)
			txtbx.value = dValue;
		else {
			txtbx.value = dValue.toFixed(3);
		}
	}
}

//-----------------------------------------------------------------------------
function uploadKernelCheckbox() {
    return (uploadCheckBox ("cboxKernel"));
}

//-----------------------------------------------------------------------------
function uploadText (idTxtBox) {
	var txt="";
	var txtbx = document.getElementById(idTxtBox);
	if (txtbx != null)
		txt = txtbx.value;
	return (txt);
}

//-----------------------------------------------------------------------------
function uploadTextAsInt (idTxtBox) {
	var nValue=0;
	var txtbx = document.getElementById(idTxtBox);
	if (txtbx != null) {
		var txt = txtbx.value;
		nValue = parseInt (txtbx);
	}
	return (txt);
}

//-----------------------------------------------------------------------------
function uploadRemoteProcessing() {
    var msg=new Object;
	var host="", port="";

	var f = uploadCheckBox ("cboxRemoteHost");
	msg['remote_on_off'] = f;
	if (f) {
		host = uploadText ('txtRemoteHost');
		port = uploadTextAsInt ('txtRemotePort');
	}
	msg['remote_host'] = host;
	msg['remote_port'] = port;
	return (msg);
}

//-----------------------------------------------------------------------------
function downloadRemoteProceesing(dictRemoteProc) {
	try {
		var id = document.getElementById ('cboxRemoteHost');
		if (id != null)
			id.checked = dictRemoteProc.remote_on_off;//, "cboxStartSignal");
        donwloadText ('txtRemoteHost', dictRemoteProc.remote_host);
        donwloadText ('txtRemotePort', dictRemoteProc.remote_port);
	}
    catch (exception) {
        console.log(exception);
    }
}

//-----------------------------------------------------------------------------
function onSaveClick() {
	try {
		save();
	}
    catch (exception) {
        console.log(exception);
    }

}

//-----------------------------------------------------------------------------
function exportToCsv(filename, rows) {
    var blob = new Blob([rows], { type: 'text/csv;charset=utf-8;' });
    //var blob = new Blob([csvFile], { type: 'text/csv;charset=utf-8;' });
    if (navigator.msSaveBlob) { // IE 10+
        navigator.msSaveBlob(blob, filename);
    } else {
        var link = document.createElement("a");
        if (link.download !== undefined) { // feature detection
            // Browsers that support HTML5 download attribute
            var url = URL.createObjectURL(blob);
            link.setAttribute("href", url);
            link.setAttribute("download", filename);
            link.style.visibility = 'hidden';
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }
    }
}

//-----------------------------------------------------------------------------
function save() 
{ 
	var data = localStorage.getItem ('MCA_Data');
	if (data != null) {
		try {
			var lines = data.toString().replaceAll (',','\n');
			exportToCsv("mca.csv", lines);
		}
    	catch (exception) {
        	console.log(exception);
    	}
	}
    
} 

//-----------------------------------------------------------------------------
function uploadSamplingRate() {
    var rate = uploadRate();
	var dt = 1;
	if (rate.length > 0) {
		rate = parseFloat(rate);
		dt = 1.0 / rate;
/*
		if (rate == 125000000">125Msps</option>
                            	<option value="
		if (rate == 15625000">15.625Msps</option>
                            	<option value="
		if (rate == 1953000">1.953Msps</option>
                            	<option value="
		if (rate == 122070">122.070ksps</option>
                            	<option value="
		if (rate == 15258">15.258ksps</option>
                            	<option value="1907">1.907ksps</option>
*/
	}
	console.log(rate);
	return (dt);
}
