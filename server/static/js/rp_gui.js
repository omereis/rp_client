/******************************************************************************\
|                                 rp_gui.js                                    |
\******************************************************************************/

//-----------------------------------------------------------------------------
const SamplingTitle = "Sampling";
const McaTitle = "MCA";
const StatusTitle = "Status";
const ReadMcaTitle = "ReadMca";
const MCAStatusID = "MCA_Status";
//-----------------------------------------------------------------------------
function include(file) {
// source:
// https://www.geeksforgeeks.org/how-to-include-a-javascript-file-in-another-javascript-file/  
    var script  = document.createElement('script');
    script.src  = file;
    script.type = 'text/javascript';
    script.defer = true;

    document.getElementsByTagName('head').item(0).appendChild(script);
}
//include ('https://canvasjs.com/assets/script/canvasjs.min.js');
//var g_chart = null;//new CanvasJS.Chart("chartContainer");
//-----------------------------------------------------------------------------
function onConnectClick()
{
    console.log('Clicked')
    var host, port;
    var dictMessage = {};
    host= uploadHost ();
    port = uploadPort ();
    dictMessage["host"] = host;
    dictMessage["port"] = port.toString();
    console.log(JSON.stringify(dictMessage));
    $.ajax({
    url: "/onconnect",
    type: "get",
    //data: {message: JSON.stringify({"host":"rsa","port":"5500"})},
    data: {message: JSON.stringify(dictMessage)},
    success: function(response) {
        try {
            console.log(response);
            document.getElementById('connect_status').value = response;
}
        catch (err) {
            console.log(err);
        }
    },
    error: function(xhr) {
        alert('error');
        console.log(xhr.toString());
    }
});
}
//-----------------------------------------------------------------------------
function uploadHost ()
{
    var host = uploadTextBoxValue('server_name', 'rsa');
    return (host);
}
//-----------------------------------------------------------------------------
function uploadTextBoxValue(txt_id, strDefault='')
{
    var val;

    try {
        var element = document.getElementById(txt_id).value;
        if (element != null)
            val = element;
        else
            val = strDefault;
    }
    catch (err) {
        console.log(err);
        val = strDefault;
    }
    return (val);
}
//-----------------------------------------------------------------------------
function uploadPort ()
{
    var strPport = uploadTextBoxValue('server_port', '5500');
    var nPort;

    try {
        nPort = parseInt(strPort);
    }
    catch (err){
        console.log(err);
        nPort = 5500;
    }
    return (nPort);

}
//-----------------------------------------------------------------------------
function onReadSamplingClick () {
    var crOld = document.body.style.cursor;
    try {
        document.body.style.cursor = "wait";
        document.getElementById("txtMessage").value = "Reading...";
        var dictMessage = {};
        dictMessage["setup"] = {"read" : SamplingTitle};//"sampling"};
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    console.log(response);
                    document.getElementById("txtMessage").value = "";
                    downloadSetup (response);
                    //document.getElementById('txtSampling').textContent = response;
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                alert('error');
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        console.log(e);
    }
    finally {
        document.body.style.cursor = crOld;
    }
}
//-----------------------------------------------------------------------------
function uploadSampling () {
    var msg = {};
    msg["rate"] = uploadRate ();
    msg["decimation"] = uploadDecimation ();
    msg["buffer_length"] = uploadBufferLength();
    msg["signal_points"] = uploadSignalPoints();
    return (msg);
}
//-----------------------------------------------------------------------------
function uploadRate () {
    var rate;
    try {
        var id = document.getElementById("comboRate");
        var idSelected = id.selectedOptions[0].id;

        if (idSelected == "rate125")
            rate = 125e6;
        else if (idSelected == 'rate15_625')
            rate = 15.625e6;
        else if (idSelected == 'rate1_953')
            rate = 1.953e6;
        else if (idSelected == 'rate122_070')
            rate = 122.070e3;
            
    }
    catch {
        rate = 125e6;
    }
    return (rate);
}
//-----------------------------------------------------------------------------
function uploadDecimation() {
    var decimation;
    try {
        var id = document.getElementById("comboDecimation");
        var idSelected = id.selectedOptions[0].id;

        if (idSelected == "dcm1")
            decimation = 1;
        else if (idSelected == 'dcm8')
            decimation = 8;
        else if (idSelected == 'dcm64')
            decimation = 64;
        else if (idSelected == 'dcm1024')
            decimation = 1024;
            
    }
    catch {
        decimation = 1;
    }
    return (decimation);
}
//-----------------------------------------------------------------------------
function uploadBufferLength() {
    return (uploadTextAsInteger ("txtQueueSize", 1000));
}
//-----------------------------------------------------------------------------
function uploadSignalPoints() {
    return (uploadTextAsInteger ("txtSignalPoints", 200));
}
//-----------------------------------------------------------------------------
function uploadTextAsInteger (txtId, nDefault) {
    var size;

    try {
        var txt = document.getElementById(txtId)
        if (txt != null)
            size = txt.value;
    }
    catch (err) {
        alert (err);
        size = nDefault;
    }
    return (size);
}
//-----------------------------------------------------------------------------
function uploadTrigger() {
    var msg = {};

     msg["level"] = uploadLevel ();
     msg["dir"] = uploadTriggerDir ();
     msg["src"] = uploadTriggerSrc ();
     msg["type"] = uploadTriggerType ();
     return (msg);
}
//-----------------------------------------------------------------------------
function uploadLevel () {
    var level;

    var idLevel = document.getElementById("txtTriggerLevel");
    if (idLevel == null)
        level = 5e-3;
    else {
        try {
            level = 1e-3 * parseFloat(idLevel.value);
        }
        catch {
            level = 5e-3;
        }     
    }
    return (level);
}
//-----------------------------------------------------------------------------
function uploadTriggerDir () {
    var dir;
    var id = document.getElementById("comboTriggerDir");
    if (id != null) {
        if (id.selectedOptions[0].id == "dirDown")
            dir = "down";
        else
            dir = "up";
    }
    else
        dir = "up";
    return (dir);
}
//-----------------------------------------------------------------------------
function uploadTriggerSrc () {
    var src;
    var id = document.getElementById("comboTriggerSrc");
    if (id != null) {
        if (id.selectedOptions[0].id == "srcIn1")
            src = "in1";
        else if (id.selectedOptions[0].id == "srcIn2")
           src = "in2";
        else if (id.selectedOptions[0].id == "srcExt")
            src = "external";
        else
        src = "none";
    }
    else
        src = "in1";
    return (src);
}

//-----------------------------------------------------------------------------
function uploadTriggerType () {
}

//-----------------------------------------------------------------------------
function onUpdateSamplingClick () {
    var crOld = document.body.style.cursor;
    try {
        var dictMessage = {};
        var dictSetup = {};
        var sampling = uploadSampling ();
        var trigger = uploadTrigger ();
        //dictSetup["sampling"] = sampling;
        dictSetup[SamplingTitle] = sampling;
        dictSetup["trigger"] = trigger;
        dictMessage["setup"] = dictSetup;
        document.getElementById("txtMessage").value = "Sent...";
        console.log(JSON.stringify(dictMessage));
        document.body.style.cursor = "wait";
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    downloadSetup (response);
                    document.getElementById("txtMessage").value = "";
                    //console.log(response);
                    //document.getElementById('txtSampling').textContent = response;
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                alert('error');
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        console.log(e);
    }
    finally {
        document.body.style.cursor = crOld;
    }
}
//-----------------------------------------------------------------------------
function onReadAppSetupClick () {
    var crOld = document.body.style.cursor;
    try {
        document.body.style.cursor = "wait";
        document.getElementById(MCAStatusID).textContent = "Reading...";
        var dictMessage = {};
        dictMessage["setup"] = {"read" : "applications"};
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    console.log(response);
                    document.getElementById(MCAStatusID).textContent = "done reading";
                    downloadAppsSetup (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                alert('error');
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        console.log(e);
    }
    finally {
        document.body.style.cursor = crOld;
    }
}
//-----------------------------------------------------------------------------
function onWriteAppSetupClick () {
    var crOld = document.body.style.cursor;
    try {
        document.body.style.cursor = "wait";
        document.getElementById(MCAStatusID).textContent = "Setting...";
        var dictSetup = {};
        var dictMessage = {};
        dictSetup["mca"] = uploadMcaSetup();
        dictSetup["psd"] = uploadPsdSetup();
        dictMessage["setup"] = dictSetup;
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    console.log(response);
                    document.getElementById(MCAStatusID).textContent = "done setting";
                    downloadAppsSetup (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                alert('error');
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        console.log(e);
    }
    finally {
        document.body.style.cursor = crOld;
    }
}
//-----------------------------------------------------------------------------
function downloadSetup (strResponse) {
    try {
        var jSetup = JSON.parse(strResponse);
        //downloadSampling (jSetup["sampling"]);
        downloadSampling (jSetup[SamplingTitle]);
        downloadTrigger (jSetup["trigger"]);
    }
    catch (error) {
        console.log(error);
        alert (error);
    }
}
//-----------------------------------------------------------------------------
function downloadAppsSetup (response) {
    try {
        var jSetup = JSON.parse(response);
        downloadMcaSetup (jSetup["mca"]);
        downloadPsdSetup (jSetup["psd"]);
    }
    catch (error) {
        console.log(error);
        alert (error);
    }
}
//-----------------------------------------------------------------------------
function downloadSampling (jSampling) {
    //var rate = jSampling["rate"];
    selectComboItem ("comboRate", jSampling["rate"]);
    selectComboItem ("comboDecimation", jSampling["decimation"]);
    var txt = document.getElementById("txtQueueSize");
    if (txt != null)
        txt.value =  jSampling["buffer_length"];
    txt = document.getElementById("txtSignalPoints");
    if (txt != null)
        txt.value =  jSampling["signal_points"];
    //console.log(rate);
}
//-----------------------------------------------------------------------------
function downloadMcaSetup (jMca) {
    document.getElementById("txtMcaChannels").value = jMca.channels;
    document.getElementById("txtMcaMinV").value     = jMca.min_voltage;
    document.getElementById("txtMcaMaxV").value     = jMca.max_voltage;
    console.log(JSON.stringify(jMca));
}
//-----------------------------------------------------------------------------
function downloadPsdSetup (jPsd) {
    console.log(JSON.stringify(jPsd));
}
//-----------------------------------------------------------------------------
function selectComboItem (strCombo, strValue) {
    try {
        var n, nFound=null;
        var combo = document.getElementById(strCombo);
        for (n=0 ; (n < combo.options.length) && (nFound == null) ; n++) {
            var strOption = combo.options[n].value;
            if (strOption == strValue)
                nFound = n;
        }
        if (nFound != null)
            combo.selectedIndex = nFound;
    }
    catch (error) {
        alert ("Runtime error in 'selectComboItem':\nerror");
    }
}
//-----------------------------------------------------------------------------
function downloadTrigger (jTrigger) {
    selectComboItem ("comboTriggerDir", jTrigger["in"]);
    selectComboItem ("comboTriggerSrc", jTrigger["src"]);
    var txtLevel = document.getElementById("txtTriggerLevel");
    var level = parseFloat(jTrigger["level"]) * 1e3;
    var strLevel = level.toString();
    txtLevel.value = strLevel;//jTrigger["level"];
}
//-----------------------------------------------------------------------------
function onGetPulses() {
    try {
        var dictMessage = {};
        var dictPulse = uploadPulses();
        dictMessage["ReadPulse"] = dictPulse;
        document.getElementById("cellPulseResult").textContent = "Reading...";
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/onreadpulse",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    downloadPulse (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                alert('error');
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        console.log(e);
    }
}
//-----------------------------------------------------------------------------
function uploadPulses() {
    var msg = {};
    var nPulses;
    try {
        nPulses = parseInt (document.getElementById("txtNumPulses").value);
    }
    catch (e) {
        console.log(e);
        nPulses = 0;
    }
    //msg["pulse"] = nPulses;
    return (nPulses);
}
//-----------------------------------------------------------------------------
function downloadPulse (response) {
    //document.getElementById("cellPulseResult").textContent = response;
    var dataPoints = [];
    var jPulse = JSON.parse(response);
    if (jPulse.length > 0) {
        for (var n=0 ; n < jPulse[0].length ; n++) {
            dataPoints.push({
                    x: n,
                    y: parseFloat(jPulse[0][n])
                });
        }
        showPulseOnChart (jPulse);
    }
    else
        document.getElementById("cellPulseResult").value = "no data";
}
//-----------------------------------------------------------------------------
function onStartClick() {
    SendStartStop (true);
    //SendStartStop ("start");
}
//-----------------------------------------------------------------------------
function SendStartStop (cmd) {
    try {
        var dictMessage = {};
        //dictMessage["sampling"] = cmd;//"start";
        dictMessage[SamplingTitle] = cmd;
        document.getElementById("spanStartStop").textContent = "Message Sent...";
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/on_gui_message",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    var j = JSON.parse(response);
                    document.getElementById("spanStartStop").textContent = j.Sampling;
                    console.log (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                document.getElementById("spanStartStop").textContent = xhr;
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        document.getElementById("spanStartStop").textContent = e;
        console.log(e);
    }
}
//-----------------------------------------------------------------------------
function onStopClick(){
    SendStartStop (false);
    //SendStartStop ("stop");
}
//-----------------------------------------------------------------------------
function onStatusClick() {
    SendStartStop ("status");
}
//-----------------------------------------------------------------------------
var chartPulse=null;
var chartMca=null;
//-----------------------------------------------------------------------------
function showPulseOnChart (jPulse) {
    var n, m, num, rate, factor;

    if (chartPulse == null)
        initPulseChart();
    chartPulse.data.datasets = [];
    for (n=0 ; n < jPulse.length ; n++) {
        var ds = {data: [], label: ' '};
        // X axis
        if (chartPulse.data.labels.length != jPulse[0].length) {
            chartPulse.data.labels = [];
            try {
                rate = 1.0 / parseFloat(uploadRate ());
                factor = 1e9;
            }
            catch (e) {
                rate = 1;
                factor = 1;
                console.log(e);
            }
            for (m=0 ; m < jPulse[0].length ; m++) {
                chartPulse.data.labels.push(m * rate * factor);
            }
        }
        for (m=0 ; m < jPulse[n].length ; m++) {
            try {
                num = parseFloat(jPulse[n][m])
            }
            catch (e) {
                console.log(e);
                num = 0;
            }
            ds.data.push(num);
        }
        chartPulse.data.datasets.push(ds);
    }
    chartPulse.update();
}
//-----------------------------------------------------------------------------
function initPulseChart() {
	//var myChart=document.getElementById("pulseChart").getContext("2d");
    var myChart=document.getElementById("idPulseCanvas").getContext("2d");

	if (chartPulse == null) {
		chartPulse = new Chart (myChart,{
			type: 'line', // bar, horizontal bar. pie, line. doughnut, radar. polarArea
			data: {
				labels: ["	Baltimore", "Columbia", "Germantown", "Silver Spring", "Waldorf", "Fredrick"],
				datasets: [{
					label: 'Pulse',
					data: [620915, 99615, 86395, 71452, 67752, 65455],
                    //backgroundcolor: "blue",
					borderColor: 'rgb(75, 192, 192)'
				}]
			},
			options: {
                scales : {
                    x: {
                        ticks: {
                            callback: function (val, index) {
                                return val.toFixed(1);
                            }
                        }
                    }
                },
                  plugins: {
                    zoom: {
                      pan: {
                        enabled: true,
                        mode: 'x'
                      },
                      zoom: {
                        pinch: {
                          enabled: true
                        },
                        wheel: {
                          enabled: true
                        },
                        mode: 'x'
                      },
                      limits: {
                        x: {
                          minDelay: 0,
                          maxDelay: 4000,
                          minDuration: 1000,
                          maxDuration: 20000
                        }
                      }
                    }
                }
            }
	    });
	}
}
//-----------------------------------------------------------------------------
function uploadMcaSetup() {
    var jMca = {};
    jMca["channels"] = document.getElementById("txtMcaChannels").value;
    jMca["min_voltage"] = document.getElementById("txtMcaMinV").value;
    jMca["max_voltage"] = document.getElementById("txtMcaMaxV").value;
    return (jMca);
}
//-----------------------------------------------------------------------------
function uploadPsdSetup() {
    var jPsd = {};
    jPsd["psd"] ="not yet supported";
    return (jPsd);
}
//-----------------------------------------------------------------------------
function onMcaStatus () {
    try {
        var dictMessage = {};
        //dictMessage["sampling"] = cmd;//"start";
        dictMessage[McaTitle] = StatusTitle;
        document.getElementById(MCAStatusID).textContent = "Message Sent...";
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/on_gui_message",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    var j = JSON.parse(response);
                    document.getElementById(MCAStatusID).textContent = j[StatusTitle];
                    console.log (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                document.getElementById(MCAStatusID).textContent = xhr;
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        document.getElementById(MCAStatusID).textContent = e;
        console.log(e);
    }
}
//-----------------------------------------------------------------------------
function onMcaStart () {
    onMcaStartStop (true);
}
//-----------------------------------------------------------------------------
function onMcaStop () {
    onMcaStartStop (false);
}
//-----------------------------------------------------------------------------
function onMcaStartStop (mca_cmd) {
    try {
        var dictMessage = {};
        //var mca_cmd = uploadMcaCommand ();
        dictMessage[McaTitle] = mca_cmd;
        document.getElementById(MCAStatusID).textContent = "Message Sent...";
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/on_gui_message",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    var j = JSON.parse(response);
                    document.getElementById(MCAStatusID).textContent = j[StatusTitle];
                    console.log (response);
                }
                catch (err) {
                    console.log(err);
                }
            },
            error: function(xhr) {
                document.getElementById(MCAStatusID).textContent = xhr;
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        document.getElementById(MCAStatusID).textContent = e;
        console.log(e);
    }
}
//-----------------------------------------------------------------------------
function uploadMcaCommand () {
    var cmd;

    try {
        var current = document.getElementById(MCAStatusID).textContent;
        if (current == "true")
            cmd = false;
        else
        cmd = true;
    }
    catch (err) {
        console.log(e);
        cmd = false;
    }
    return (cmd);
}
//-----------------------------------------------------------------------------
function onReadMcaClick() {
    try {
        var dictMessage = {};
        dictMessage[McaTitle] = ReadMcaTitle;
        document.getElementById(MCAStatusID).textContent = "Message Sent...";
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/on_gui_message",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    var jMCA = JSON.parse(response);
                    if (McaTitle in jMCA)
                        downloadMca(jMCA[McaTitle]);
                    //document.getElementById(MCAStatusID).textContent = j[StatusTitle];
                    else
                        document.getElementById(MCAStatusID).textContent = response;
                    console.log (response);
                }
                catch (err) {
                    console.log(err);
                    document.getElementById(MCAStatusID).textContent = err;
                }
            },
            error: function(xhr) {
                document.getElementById(MCAStatusID).textContent = xhr;
                console.log(xhr.toString());
            }
        });
    }
    catch (e) {
        document.getElementById(MCAStatusID).textContent = e;
        console.log(e);
    }
}
//-----------------------------------------------------------------------------
function downloadMca (jMca) {
    var ds = {data: [], label: []}, num;
	var aLabels=[];

    if (chartMca == null)
        initMcaChart();
/*
	for (var n=0 ; n < 1024 ; n++) {
		ds.data.push(1024 - (n+1));
		aLabels.push(n+1);
	}

*/

/*
*/
    for (var n=0 ; n < jMca.length ; n++) {
		aLabels.push(n+1);
        try {
            num = parseFloat(jMca[n])
        }
        catch (e) {
            console.log(e);
            num = 0;
        }
		if (num > 0)
			num += 0;
        ds.data.push(num);
    }

    while (chartMca.data.datasets.length > 0)
		chartMca.data.datasets.pop();
	chartMca.data.labels = aLabels;
    chartMca.data.datasets.push(ds);
    chartMca.update();
}
//-----------------------------------------------------------------------------
function initMcaChart() {
	//var myChart=document.getElementById("pulseChart").getContext("2d");
    var canvasChart=document.getElementById("idMcaCanvas").getContext("2d");
    var channels = document.getElementById("txtMcaChannels").value;

    try {
        chartMca = new Chart(canvasChart, {
            type: 'bar',
            data: {
                labels: [1, 2, 3, 4, 5],//["Africa", "Asia", "Europe", "Latin America", "North America"],
                datasets: [ {
                    label: "",//"Population (millions)",
                    //backgroundColor: ["#3e95cd", "#8e5ea2","#3cba9f","#e8c3b9","#c45850"],
                    data: [2478,5267,734,784,433]
                } ]
            },
            options: {
                legend: { display: false },
                title: {
                    display: false,//true,
                    text: 'Predicted world population (millions) in 2050'
              }
            }
        });
    }
    catch (err) {
        console.log(err);
    }
}
//-----------------------------------------------------------------------------
function onSaveMca() {
    onMcaStartStop ("SaveMca");
}
//-----------------------------------------------------------------------------
function testBarChart() {
/* Source
https://tobiasahlin.com/blog/chartjs-charts-to-get-you-started/#horizontal-bar-chart
*/
	var ctx = document.getElementById("idMcaCanvas")
/*new Chart(document.getElementById("idMcaChart"), {*/
	new Chart(ctx, {
    type: 'bar',
    data: {
      labels: ["Africa", "Asia", "Europe", "Latin America", "North America"],
      datasets: [
        {
          label: "Population (millions)",
          backgroundColor: ["#3e95cd", "#8e5ea2","#3cba9f","#e8c3b9","#c45850"],
          data: [2478,5267,734,784,433]
        }
      ]
    },
    options: {
      legend: { display: false },
      title: {
        display: true,
        text: 'Predicted world population (millions) in 2050'
      }
    }
});

}
//-----------------------------------------------------------------------------
