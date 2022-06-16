/******************************************************************************\
|                                  rp_ws.js                                    |
|                           Red Pitaya Web Socket                              |
\******************************************************************************/

/******************************************************************************/

//-----------------------------------------------------------------------------
function onReadRedPitayaSetupClick () {
    var msg = new Object;
    msg['setup'] = 'read';
    //msg["sampling"] = "true";
    sendMesssageThroughFlask(msg, setupHandler);
}

var webSocket   = null;
var ws_protocol = null;
var ws_hostname = null;
var ws_port     = null;
var ws_endpoint = null;
var g_data = null;
/**
 * Event handler for clicking on button "Connect"
 */

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
    var msg = new Object;
    var msgBuf = new Object;
    msg['sampling'] = 'True';
    msgBuf['buffer_length'] = txt.value;//"100";
    msgBuf['units'] = "microseconds";
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
        var yData=[], xData=[], t=0;
        var nPulses = samples.pulses.signal.length;//samples.sampling.pulse_count.toString();
        for (var n=0 ; n < nPulses ; n++, t += 8e-9) {
            yData[n] = parseFloat (samples.pulses.signal[n]);
            xData[n] = t;
        }
        var chart = document.getElementById("chartSignal");
		Plotly.newPlot(chart, [{
            x: xData/*[1, 2, 3, 4, 5]*/,
            y: yData/*[1, 2, 4, 8, 16]*/ }], {
            margin: { t: 0 } } );
/*
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
*/
            //var chart = document.getElementById("chartSignal");
			//document.cookie = aPulses[aKeys[0]];
        	//drawSignal(aPulses[aKeys[0]]);
        }
    catch (exception) {
		var txt = cell.innerText;
		txt = exception;
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
		//drawChart();
	}
	catch (exception) {
		console.log(exception);
	}
}

//-----------------------------------------------------------------------------
function drawChart() {
    var margin = {top: 10, right: 30, bottom: 30, left: 60},
        width = 460 - margin.left - margin.right,
        height = 400 - margin.top - margin.bottom;
    
	var data = document.cookie;
    // append the svg object to the body of the page
    var svg = d3.select("#divChart")
      .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
      .append("g")
        .attr("transform",
              "translate(" + margin.left + "," + margin.top + ")");
        var x = d3.scaleTime()
          .domain(d3.extent(data, function(d) { return d.date; }))
          .range([ 0, width ]);
        xAxis = svg.append("g")
          .attr("transform", "translate(0," + height + ")")
          .call(d3.axisBottom(x));
    
        // Add Y axis
        var y = d3.scaleLinear()
          .domain([0, d3.max(data, function(d) { return +d.value; })])
          .range([ height, 0 ]);
        yAxis = svg.append("g")
          .call(d3.axisLeft(y));
    
        // Add a clipPath: everything out of this area won't be drawn.
        var clip = svg.append("defs").append("svg:clipPath")
            .attr("id", "clip")
            .append("svg:rect")
            .attr("width", width )
            .attr("height", height )
            .attr("x", 0)
            .attr("y", 0);
    
        // Add brushing
        var brush = d3.brushX()                   // Add the brush feature using the d3.brush function
            .extent( [ [0,0], [width,height] ] )  // initialise the brush area: start at 0,0 and finishes at width,height: it means I select the whole graph area
            .on("end", updateChart)               // Each time the brush selection changes, trigger the 'updateChart' function
    
        // Create the line variable: where both the line and the brush take place
        var line = svg.append('g')
          .attr("clip-path", "url(#clip)")
    
        // Add the line
        line.append("path")
          .datum(data)
          .attr("class", "line")  // I add the class line to be able to modify this line later on.
          .attr("fill", "none")
          .attr("stroke", "steelblue")
          .attr("stroke-width", 1.5)
          .attr("d", d3.line()
            .x(function(d) { return x(d.date) })
            .y(function(d) { return y(d.value) })
            )
    
        // Add the brushing
        line
          .append("g")
            .attr("class", "brush")
            .call(brush);
    
        // A function that set idleTimeOut to null
        var idleTimeout
        function idled() { idleTimeout = null; }
        function updateChart() {
    
          // What are the selected boundaries?
          extent = d3.event.selection
    
          // If no selection, back to initial coordinate. Otherwise, update X axis domain
          if(!extent){
            if (!idleTimeout) return idleTimeout = setTimeout(idled, 350); // This allows to wait a little bit
            x.domain([ 4,8])
          }else{
            x.domain([ x.invert(extent[0]), x.invert(extent[1]) ])
            line.select(".brush").call(brush.move, null) // This remove the grey brush area as soon as the selection has been done
          }
    
          // Update axis and line position
          xAxis.transition().duration(1000).call(d3.axisBottom(x))
          line
              .select('.line')
              .transition()
              .duration(1000)
              .attr("d", d3.line()
                .x(function(d) { return x(d.date) })
                .y(function(d) { return y(d.value) })
              )
        }
        svg.on("dblclick",function(){
          x.domain(d3.extent(data, function(d) { return d.date; }))
          xAxis.transition().call(d3.axisBottom(x))
          line
            .select('.line')
            .transition()
            .attr("d", d3.line()
              .x(function(d) { return x(d.date) })
              .y(function(d) { return y(d.value) })
          )
        });

}

//-----------------------------------------------------------------------------
