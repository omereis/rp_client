/******************************************************************************\
|                                 rp_gui.js                                    |
\******************************************************************************/

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
        var dictMessage = {};
        //var sampling = uploadSampling ();
        //var trigger = uploadTrigger ();
        dictMessage["setup"] = "read";
        //dictMessage["sampling"] = sampling;
        //dictMessage["trigger"] = trigger;
        console.log(JSON.stringify(dictMessage));
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    console.log(response);
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
function uploadTrigger() {
    var msg = {};

     msg["level"] = uploadLevel ();
     msg["dir"] = uploadTriggerDir ();
     msg["src"] = uploadTriggerSrc ();
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
function onUpdateSamplingClick () {
    var crOld = document.body.style.cursor;
    try {
        var dictMessage = {};
        var sampling = uploadSampling ();
        var trigger = uploadTrigger ();
        dictMessage["sampling"] = sampling;
        dictMessage["trigger"] = trigger;
        console.log(JSON.stringify(dictMessage));
        document.body.style.cursor = "wait";
        $.ajax({
            url: "/onparams",
            type: "get",
            data: {message: JSON.stringify(dictMessage)},
            success: function(response) {
                try {
                    console.log(response);
                    document.getElementById('txtSampling').textContent = response;
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
        downloadSampling (jSetup["sampling"]);
        downloadTrigger (jSetup["trigger"]);
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
    //console.log(rate);
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

