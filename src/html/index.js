
var g_interval = 2000;
var g_timer = null;


function $(id) 
{
	var _obj = document.getElementById(id);
	//if(_obj==null)	alert("ID " + id + " is null");
	return _obj;
}


function RequestHttp(url, event)
{
	var _xmlhttp = new XMLHttpRequest();
	_xmlhttp.onreadystatechange=function()
	{
		if (_xmlhttp.readyState==4 && _xmlhttp.status==200)
		{
			if(event!=null)
			{
				event(_xmlhttp.responseText);
			}
		}
	}

	_xmlhttp.open("GET", url, true);
	_xmlhttp.send();	
}


window.onload=function()
{
	$('ID_MASK').value = "32";
	$('ID_SRC_PORT').value = "18001";
	$('ID_TIMEOUT').value = "5";

	ChangeTab("ID_QUEUE");
	RequestHttp("about", OnAbout);
	
	$("ID_BEGIN_PORT").disabled = true;
	$("ID_END_PORT").disabled = true;
	$("ID_SRC_PORT").disabled = true;
}


function ChangeTab(tab)
{
	clearTimeout(g_timer);

	$("ID_QUEUE").className = "SPAN_UNSELECT";
	$("ID_SENT").className = "SPAN_UNSELECT";
	$("ID_RESULT").className = "SPAN_UNSELECT";
	$(tab).className = "SPAN_SELECTED";

	if(tab=="ID_QUEUE")
	{
		RequestHttp("queue", OnQueue);
	}
	else if(tab=="ID_SENT")
	{
		RequestHttp("sent", OnSent);
	}
	else if(tab=="ID_RESULT")
	{
		RequestHttp("result", OnResult);
	}
	$("ID_CONTENT").innertHTML = "loading...";
}


function GetTableRows(id)
{
	return document.getElementById(id).getElementsByTagName("tbody")[0].getElementsByTagName("tr").length;
}

function OnSent(content)
{
	var _obj = JSON.parse(content);

	if( $("ID_TABLESENT")!=null && GetTableRows("ID_TABLESENT")==_obj.size)
		return;	
	
	var _table = document.createElement("table");
	_table.createCaption().innerHTML = "Has sent without response"
	_table.setAttribute("id", "ID_TABLESENT");
	
	var _th = _table.createTHead();
	var _tr = _th.insertRow(0);
	_tr.insertCell(0).innerHTML = "sent time";
	_tr.insertCell(1).innerHTML = "ip";
	_tr.insertCell(2).innerHTML = "port";
	_tr.insertCell(3).innerHTML = "type";
	
	var _tbody = _table.createTBody();
	for(var i=0; i<_obj.size; i++)
	{
		var j=0;
		var _row = _tbody.insertRow(i);
		_row.insertCell(j++).innerHTML = _obj.data[i].senttime;
		_row.insertCell(j++).innerHTML = _obj.data[i].ip;
		_row.insertCell(j++).innerHTML = _obj.data[i].port;
		_row.insertCell(j++).innerHTML = _obj.data[i].type;
	}

	$('ID_CONTENT').innerHTML = "";
	$('ID_CONTENT').appendChild(_table);
	
	g_timer = setTimeout( function(){ RequestHttp("sent", OnSent); }, g_interval);
}


function OnQueue(content)
{
	var _obj = JSON.parse(content);

	if( $("ID_TABLEQUEUE")!=null && GetTableRows("ID_TABLEQUEUE")==_obj.size)
		return;	
		
	var _table = document.createElement("table");
	_table.createCaption().innerHTML = "Prepare to send"
	_table.setAttribute("id", "ID_TABLEQUEUE");

	var _th = _table.createTHead();
	var _tr = _th.insertRow(0);

	_tr.insertCell(0).innerHTML = "ip";
	_tr.insertCell(1).innerHTML = "port";
	_tr.insertCell(2).innerHTML = "type";
	
	var _tbody = _table.createTBody();
	for(var i=0; i<_obj.size; i++)
	{
		var j=0;
		var _row = _tbody.insertRow(i);
		_row.insertCell(j++).innerHTML = _obj.data[i].ip;
		_row.insertCell(j++).innerHTML = _obj.data[i].port;
		_row.insertCell(j++).innerHTML = _obj.data[i].type;
	}

	$('ID_CONTENT').innerHTML = "";
	$('ID_CONTENT').appendChild(_table);
	
	g_timer = setTimeout( function(){ RequestHttp("queue", OnQueue); }, g_interval);
}


function OnResult(content)
{
	var _obj = JSON.parse(content);

	if( $("ID_TABLERESULT")!=null && GetTableRows("ID_TABLERESULT")==_obj.size)
		return;	
		
	var _table = document.createElement("table");
	_table.createCaption().innerHTML = "Received list"
	_table.setAttribute("id", "ID_TABLERESULT");

	var _th = _table.createTHead();
	var _tr0 = _th.insertRow(0);
	_tr0.insertCell(0).innerHTML = "sent time";
	_tr0.insertCell(1).innerHTML = "received time";
	_tr0.insertCell(2).innerHTML = "ip";
	_tr0.insertCell(3).innerHTML = "port";
	_tr0.insertCell(4).innerHTML = "type";
	_tr0.insertCell(5).innerHTML = "status";
	_tr0.insertCell(6).innerHTML = "ttl";
	_tr0.insertCell(7).innerHTML = "os";
	
	$('ID_CONTENT').innerHTML = "";
	$('ID_CONTENT').appendChild(_table);
	
	var _tbody = _table.createTBody();
	for(var i=0; i<_obj.size; i++)
	{
		var j=0;
		var _row = _tbody.insertRow(i);
		_row.insertCell(j++).innerHTML = _obj.data[i].senttime;
		_row.insertCell(j++).innerHTML = _obj.data[i].recvtime;
		_row.insertCell(j++).innerHTML = _obj.data[i].ip;
		_row.insertCell(j++).innerHTML = _obj.data[i].port;
		_row.insertCell(j++).innerHTML = _obj.data[i].type;
		_row.insertCell(j++).innerHTML = _obj.data[i].status;
		_row.insertCell(j++).innerHTML = _obj.data[i].ttl;
		_row.insertCell(j++).innerHTML = _obj.data[i].os;
	}

	g_timer = setTimeout( function(){ RequestHttp("result", OnResult); }, g_interval);
}


function VerifyNumber(id, min, max)
{
	if( isNaN( $(id).value ))	return false;
	if( $(id).value > max )		return false;
	if( $(id).value < min )		return false;
	return true
}


function VerifyIP(ip)
{
	var re = /^((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))$/;

 		if(re.test(ip))
		return true;
	else  
		return false;
}


function GetRadioValue(name)
{
	var _arrScanType = document.getElementsByName(name);
	var _scantype = "";
	for(var i=0; i<_arrScanType.length; i++)
	{
		if(_arrScanType[i].checked)
		{
			_scantype = _arrScanType[i].value;
			break;
		}
	}
	return _scantype;
}

function OnClickScan()
{
	var _scantype = GetRadioValue("SCANTYPE");
	if(!VerifyNumber("ID_MASK",0,32 ))
	{
		alert("Mask is incorrect!!");
		return;
	}
	
	if(!VerifyNumber("ID_TIMEOUT",1,1000 ))
	{
		alert("Timeout is incorrect!!");
		return;
	}
	
	if(_scantype!="ICMP")
	{
		if( !VerifyNumber("ID_BEGIN_PORT",1,65535 ) || !VerifyNumber("ID_END_PORT",1,65535 ) || !VerifyNumber("ID_SRC_PORT",1024,65535 ))
		{
			alert("Port is incorrect!!");
			return;
		}
	}

	if(!VerifyIP($('ID_IP').value))
	{
		alert("IP is incorrect!!");
		return;
	}
	
	
	var _url = "";
	if(_scantype=="ICMP")
	{
		_url += "scan?ip=" + $('ID_IP').value + "&mask=" + $('ID_MASK').value + "&local_port=0";
		_url += "&begin_port=0&end_port=0";
		_url += "&timeout=" + $('ID_TIMEOUT').value + "&type="+ _scantype;
	}
	else
	{
		_url += "scan?ip=" + $('ID_IP').value + "&mask=" + $('ID_MASK').value + "&local_port=" + $('ID_SRC_PORT').value;
		_url += "&begin_port=" + $('ID_BEGIN_PORT').value + "&end_port=" + $('ID_END_PORT').value;
		_url += "&timeout=" + $('ID_TIMEOUT').value + "&type="+ _scantype;
	}
	
	RequestHttp(_url, OnScan);
}


function OnScan(content)
{
	ChangeTab('ID_QUEUE');
}


function OnClickClear()
{
	RequestHttp("clear", OnClear);
}


function OnClear()
{
	ChangeTab('ID_RESULT');
}


function OnRadioChange()
{
	var _scanType = GetRadioValue("SCANTYPE");
	if(_scanType!="ICMP")
	{
		$("ID_BEGIN_PORT").disabled = false;
		$("ID_END_PORT").disabled = false;
		$("ID_SRC_PORT").disabled = false;
	}
	else
	{
		$("ID_BEGIN_PORT").disabled = true;
		$("ID_END_PORT").disabled = true;
		$("ID_SRC_PORT").disabled = true;
	}
}

function OnAbout(content)
{
	var _obj = JSON.parse(content);
	
	var _p1 = document.createElement("p");
	_p1.innerHTML = "Copyright is owned by " + _obj.author + " ( <a href=\"mailto:" + _obj.mail + "\">" + _obj.mail + "</a> )";
	$('ID_TAIL').appendChild(_p1);
	
	var _p2 = document.createElement("p");
	_p2.innerHTML = "Version " + _obj.version + " was built at " + _obj.built;
	$('ID_TAIL').appendChild(_p2);
}
