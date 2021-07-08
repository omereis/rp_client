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
/*
**
            var reply;
            if (typeof(response) != 'string')
                reply = JSON.parse(response);
            else
                reply = response;
            console.log(JSON.stringify(reply));
            handle_reply(reply);
        **
*/
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
