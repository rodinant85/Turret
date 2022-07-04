let stopInterval;

let traker_target;

let WS2_MESSAGES_LOG = false;


//For WS2_URI see file: config_ws2.js

// For WebSocket #2 
// followed by forwarding
//let WS2_URI = "ws://" + document.location.hostname + ":56779"

// For WebSocket #2 
// direct connection
//let WS2_URI = "ws://192.168.88.214:56779"


/*$(document).ready(function () {
    function reorient(e) {
        var portrait = (window.orientation % 180 == 0);
        $("body > div").css("-webkit-transform", portrait ? "rotate(90deg)" : "");
    }
    window["onorientationchange"] = reorient;
    window.setTimeout(reorient, 0);
});*/

/*
$(document).keydown(function(e){
    if(e.which === 17 || e.which === 18){
        $('.id_mode_button_fire').show();
        setTimeout(function() {
            $('.id_mode_button_fire').hide();
        }, 3000);
    }
    return e;
});
*/


/*Change color button fire*/

let indexTable;

$('input[type=checkbox]').each(function()
{
    this.checked = false;
});

function fire() {
    $('#id_mode_button_fire').css({'background-color': 'rgba(236, 38, 103, 0.5)'}, ws.send("{\"comm\":[\"FUSE_R\"]}"));
    setTimeout(function () {
        $('#id_mode_button_fire').css({'background-color': 'rgba(76, 175, 80, 0.5)'}, ws.send("{\"comm\":[\"FUSE_I\"]}"));
    }, 2000);
}

/*
if( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) {
    $('link[rel="stylesheet"]').attr('href','mob_app_view.css');
} else {
    $('link[rel="stylesheet"]').attr('href','app_view.css');
}
*/

var ColorDefault = "background-color': 'rgba(76, 175, 80, 0.5)";

let ws;
let ws2;
let html5VideoElement;
let webrtcPeerConnection;
let webrtcConfiguration;
let reportError;
let joyCtrlCamera;

let isPDotP = false;
let isPDotM = false;
let defaultFullTable;
let index;
let inx;

let let_buttonAltCtrlIsPressed;

let arr = [
    '{"type":"ice","data":{"candidate":"candidate:0 1 UDP 2122252543 f1e2609b-79e0-492c-a168-caf9974cfa79.local 54334 typ host","sdpMid":"video0","sdpMLineIndex":0,"usernameFragment":"c19644b3"}}',
    '{"type":"ice","data":{"candidate":"candidate:2 1 TCP 2105524479 f1e2609b-79e0-492c-a168-caf9974cfa79.local 9 typ host tcptype active","sdpMid":"video0","sdpMLineIndex":0,"usernameFragment":"c19644b3"}}',
    '{"type":"ice","data":{"candidate":"candidate:1 1 UDP 1686052863 192.168.1.111 54334 typ srflx raddr 0.0.0.0 rport 0","sdpMid":"video0","sdpMLineIndex":0,"usernameFragment":"c19644b3"}}',
    '{"type":"ice","data":{"candidate":"","sdpMid":"video0","sdpMLineIndex":0,"usernameFragment":"c19644b3"}}'
];

var showDist;

var table = '';
var selectedTableRowIndex = -1;
var requestedTableRowIndex = -1;

let arrInx = 0;
let isRegistrationMode = false;
let isStreamsSwapped = false;

function onClickButtonRegFire() {
    // isFireCorrectionProcessed is in 'coordinates.js'
    if (isFireCorrectionProcessed) {
        console.log("impossible to change reg fire mode when fire is processed");
        return;
    }
    if (isRegistrationMode) {
        isRegistrationMode = false;
        document.getElementById("id_mode_button_reg_fire").style.background = "rgba(76, 175, 80, 0.5)";
        ws.send("{\"comm\":[\"N_REG_FIRE\"]}");
    } else {
        isRegistrationMode = true;
        document.getElementById("id_mode_button_reg_fire").style.background = "rgba(236, 38, 103, 0.5)";
        ws.send("{\"comm\":[\"SET_FCI:" + 0 + "\"]}");
        ws.send("{\"comm\":[\"REG_FIRE\"]}");
    }
    ws.send("{\"comm\":[\"GET_FCI\"]}");
}

function onClickButtonZoomPlus() {
    ws.send("bt:00100000000");
}

function onClickButtonZoomMinus() {
    ws.send("bt:10000000000");
}

function onClickButtonSwapStreams() {
    // isFireCorrectionProcessed is in 'coordinates.js'
    if (isFireCorrectionProcessed) {
        console.log("impossible to swap streams when fire is processed");
        return;
    }
    if (isStreamsSwapped) {
        isStreamsSwapped = false;
        document.getElementById("id_mode_button_swap_streams").style.background = "rgba(76, 175, 80, 0.5)";
        // document.getElementById("canvas-square").style.display = "none";
        ws.send("{\"comm\":[\"SWP_STRM\"]}");
    } else {
        isStreamsSwapped = true;
        document.getElementById("id_mode_button_swap_streams").style.background = "rgba(236, 38, 103, 0.5)";
        // document.getElementById("canvas-square").style.display = "block";
        ws.send("{\"comm\":[\"SWP_STRM\"]}");
    }
}

function hideBlockSwapStreams(){
    document.getElementById("swap_streams_on_click").style.display="none";
}

function displayBlockSwapStreams(){
    document.getElementById("swap_streams_on_click").style.display="block";
}

// const cheat_str = 'WMS *\r\nm=video 9 UDP/TLS/RTP/SAVPF 96\r\nc=IN IP4 0.0.0.0\r\na=recvonly\r\na=fmtp:96 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\na=ice-pwd:79a44e7c9d548a4ef5470eb7ab8709d0\r\na=ice-ufrag:49401ce7\r\na=mid:video0\r\na=rtcp-fb:96 nack pli\r\na=rtcp-mux\r\na=rtcp-rsize\r\na=rtpmap:96 VP8/90000\r\na=setup:active\r\na=ssrc:2189315312 cname:{1ffb8d7e-786b-4e69-89ed-55563307c36d}\r\n"}'
// const cheat_str = 'WMS *\\r\\nm=video 9 UDP/TLS/RTP/SAVPF 96\\r\\nc=IN IP4 0.0.0.0\\r\\na=recvonly\\r\\na=fmtp:96 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\\r\\na=ice-pwd:79a44e7c9d548a4ef5470eb7ab8709d0\\r\\na=ice-ufrag:49401ce7\\r\\na=mid:video0\\r\\na=rtcp-fb:96 nack pli\\r\\na=rtcp-mux\\r\\na=rtcp-rsize\\r\\na=rtpmap:96 VP8/90000\\r\\na=setup:active\\r\\na=ssrc:2189315312 cname:{1ffb8d7e-786b-4e69-89ed-55563307c36d}\\r\\n"}'

//
// function onLocalDescription(desc) {
//     console.info("Local description: " + JSON.stringify(desc));
//     if (JSON.stringify(desc).includes("VP8")) {
//         let descStrOk = JSON.stringify(desc);
//         const descStrArr = descStrOk.split("WMS");
//         const descStrNew = descStrArr[0] + cheat_str;
//         desc = JSON.parse(descStrNew);
//     }
//     // let isNotRunned = true;
//     webrtcPeerConnection.setLocalDescription(desc).then(function () {
//         console.info(JSON.stringify({type: "sdp", "data": webrtcPeerConnection.localDescription}));
//         // if (JSON.stringify(webrtcPeerConnection.localDescription).includes("VP8")) {
//         //     ws.send(cheat_str);
//         // } else {
//         ws.send(JSON.stringify({type: "sdp", "data": webrtcPeerConnection.localDescription}));
//         isNotRunned = false;
//         // }
//     }).catch(reportError);
//     // if (isNotRunned) {
//     //     ws.send("aaaaaaaaa: KKK?????????WTF????");
//     //     ws.send(JSON.stringify({type: "sdp", "data": desc}));
//     // }
// }

function onLocalDescription(desc) {
    console.log("Local description: " + JSON.stringify(desc));
    webrtcPeerConnection.setLocalDescription(desc).then(function () {
        ws.send(JSON.stringify({type: "sdp", "data": webrtcPeerConnection.localDescription}));
    }).catch(reportError);
}

function onIncomingSDP(sdp) {
    console.log("Incoming SDP: " + JSON.stringify(sdp));
    webrtcPeerConnection.setRemoteDescription(sdp).catch(reportError);
    webrtcPeerConnection.createAnswer().then(onLocalDescription).catch(reportError);
}

function onIncomingICE(ice) {
    let candidate = new RTCIceCandidate(ice);
    console.log("Incoming ICE: " + JSON.stringify(ice));
    webrtcPeerConnection.addIceCandidate(candidate).catch(reportError);
}

function onAddRemoteStream(event) {
    html5VideoElement.srcObject = event.streams[0];
    html5VideoElement.autoplay = true;
}

function onIceCandidate(event) {
    if (event.candidate == null)
        return;

    console.info("Sending ICE candidate out: " + JSON.stringify(event.candidate));
    //ws.send(JSON.stringify({"type": "ice", "data": event.candidate}));
    ws.send(arr[arrInx]);
    arrInx++;
}

function myRound(number) {
    return (Math.round(number * 10) / 10);
}

function onServerMessage(event) {
    console.log("SERVER MSG: " + event.data);
    if (event.data === "YOU_ARE_EXCESS")
        //alert("You canNOT connect to the server, you are an excess client.\nPlease wait till you be able to connect!");
        alert("Вже встановлено інше підключення,\n або перезавантажте пристрій.");

    if (event.data.includes("N_REG_FIRE")) {
        isRegistrationMode = false;
        document.getElementById("id_mode_button_reg_fire").style.background = "rgba(76, 175, 80, 0.5)";
    } else if (event.data.includes("REG_FIRE")) {
        isRegistrationMode = true;
        document.getElementById("id_mode_button_reg_fire").style.background = "rgba(236, 38, 103, 0.5)";
    }
    if (event.data.includes("N_SWP_STRM")) {
        isStreamsSwapped = false;
        document.getElementById("id_mode_button_swap_streams").style.background = "rgba(76, 175, 80, 0.5)";
    } else if (event.data.includes("SWP_STRM")) {
        isStreamsSwapped = true;
        document.getElementById("id_mode_button_swap_streams").style.background = "rgba(236, 38, 103, 0.5)";
    }

    if (event.data.includes("FCA")) {
        if (event.data.includes("NO")) {
            // request Fire Corrections Array again
            ws.send("{\"comm\":[\"GET_FCA\"]}");
        } else {
            defaultFullTable = $(document).ready(function() {
                var myJsonData = (event.data);
                var obj = $.parseJSON(myJsonData);
                let v = Object.values = obj => Object.keys(obj).map(key => obj['v']);
                console.log(v);
                var tblInx = 0
                table = '<table><thead><th class="gt">Дистанція</th><th class="gt">V</th><th class="gt">H</th></thead><tbody>';
                $.each(obj.FCA, function() {
                    table += '<tr class="row_' + tblInx +'"><td>' + this['d'] + '</td><td>' + myRound(this['v']) + '</td><td>' + myRound(this['h']) + '</td></tr>';
                    tblInx++;
                });
                table += '</tbody></table>';
                document.getElementById("datalist").innerHTML = table;
            });

            // timeout for get table data which will be selected field in table
            setTimeout(
                function () {
                    $("#datalist tr").click(function () {
                        // isFireCorrectionProcessed is in 'coordinates.js'
                        if (isFireCorrectionProcessed) {
                            console.log("impossible to change PURSUIT DISTANCE when fire is processed");
                            return;
                        }
                        inx = this.className;
                        inx = inx.replace("row_", "");
                        inx = inx.replace("selected", "");
                        if (requestedTableRowIndex !== inx) {
                            ws.send("{\"comm\":[\"SET_FCI:" + inx + "\"]}");
                            ws.send("{\"comm\":[\"GET_FCI\"]}");
                            requestedTableRowIndex = inx;
                            console.log("req inx " + requestedTableRowIndex);
                            console.log("inx" + inx);
                        }
                        // $(this).addClass('selected'); ws.send(jsonData.FCI);
                        // $(this).siblings().removeClass('selected');
                        // var value = $(this).find('tr:first').html();
                    })
                }, 1000 );

            // request Fire Correction Index
            ws.send("{\"comm\":[\"GET_FCI\"]}");
        }
        // return;
    }


   let_buttonAltCtrlIsPressed = function buttonAltCtrlIsPressed() {
        if (event.data.includes('FUSE_UP')) {
            document.getElementById("shootText").innerHTML = "READY TO FIRE";
            document.getElementById("shootText").style.display = "inline";
            console.log('ready')
        }
    //console.log(event.data.includes("FIRE:3"
        if (event.data.includes('DONE_FIRE')) {
        //    if (event.data.includes('{\"comm\":[\"FUSE_DOWN\"]}') === true) {
            document.getElementById("shootText").innerHTML = "FIRE!!!";
            document.getElementById("shootText").style.display = "inline";
            setTimeout(function () {
                document.getElementById("shootText").innerHTML = "FIRE!!!";
                document.getElementById("shootText").style.display = "inline";
            }, 300);
            setTimeout(function () {
                document.getElementById("shootText").innerHTML = "";
                document.getElementById("shootText").style.display = "none";
            }, 5000);
            console.log("fire_bitch")
        }
    }

    if (event.data.includes("FCI")) {
        let jsonData = JSON.parse(event.data);
        index = jsonData.FCI;

        console.log('\n'+'\n'+'\n'+'\n'+'\n'+'\n'+'\n'+'\n' + "index: " + index + '\n'+'\n'+'\n'+'\n' +'\n'+'\n'+'\n'+'\n');
       // var tblRow = document.querySelector(".row_" + index);
       // tblRow.className = "row_" + index + " selected";

        // Write distance under buttons

        /*
        $(function() {
            $("#range").slider({
                orientation: "vertical",
                min: 0,
                max: 19,
                slide: function(event,range){
                    //document.getElementById('distanceWriteInfo').innerHTML=range.value;
                    $("#distanceWriteInfo").html(range.value+"00");
                    $("#distanceWritePrevInfo").html(range.value-1);
                    $("#distanceWriteNextInfo").html(range.value+1);
                    console.log(range.value);
                }});
            //$("#distanceWriteInfo").val($("#range").slider("value"));
        });
        */

        let obj = JSON.parse(event.data);
        indexTable = obj.FCI;
        document.getElementById('distanceWriteInfo').innerHTML=obj.FCI;
        document.getElementById('distanceWritePrevInfo').innerHTML=obj.FCI;
        document.getElementById('distanceWritePrev2Info').innerHTML=obj.FCI;
        document.getElementById('distanceWritePrev3Info').innerHTML=obj.FCI;
        document.getElementById('distanceWriteNextInfo').innerHTML=obj.FCI;
        document.getElementById('distanceWriteNext2Info').innerHTML=obj.FCI;
        document.getElementById('distanceWriteNext3Info').innerHTML=obj.FCI;


        //  &nbsp;

        let _100 = 100;
        let _200 = 200;
        let _300 = 300;


        function WriteDistance() {
            let distanceWriteInfo = document.getElementById('distanceWriteInfo');
            let t = distanceWriteInfo.textContent
            if (t === '0') {
                distanceWriteInfo.innerHTML="100";
            }
            if (t === '1') {
                distanceWriteInfo.innerHTML="200";
            }
            if (t === '2') {
                distanceWriteInfo.innerHTML="300";
            }
            if (t === '3') {
                distanceWriteInfo.innerHTML="400";
            }
            if (t === '4') {
                distanceWriteInfo.innerHTML="500";
            }
            if (t === '5') {
                distanceWriteInfo.innerHTML="600";
            }
            if (t === '6') {
                distanceWriteInfo.innerHTML="700";
            }
            if (t === '7') {
                distanceWriteInfo.innerHTML="800";
            }
            if (t === '8') {
                distanceWriteInfo.innerHTML="900";
            }
            if (t === '9') {
                distanceWriteInfo.innerHTML="1000";
            }
            if (t === '10') {
                distanceWriteInfo.innerHTML="1100";
            }
            if (t === '11') {
                distanceWriteInfo.innerHTML="1200";
            }
            if (t === '12') {
                distanceWriteInfo.innerHTML="1300";
            }
            if (t === '13') {
                distanceWriteInfo.innerHTML="1400";
            }
            if (t === '14') {
                distanceWriteInfo.innerHTML="1500";
            }
            if (t === '15') {
                distanceWriteInfo.innerHTML="1600";
            }
            if (t === '16') {
                distanceWriteInfo.innerHTML="1700";
            }
            if (t === '17') {
                distanceWriteInfo.innerHTML="1800";
            }
            if (t === '18') {
                distanceWriteInfo.innerHTML="1900";
            }
            if (t === '19') {
                distanceWriteInfo.innerHTML="2000";
            }
        }
        WriteDistance();

        function WritePrevDist() {
            let distanceWritePrevInfo = document.getElementById('distanceWritePrevInfo');
            let t = distanceWritePrevInfo.textContent;

            if (t === '0') {
                distanceWritePrevInfo.innerHTML="100" - _100;
            }
            if (t === '1') {
                distanceWritePrevInfo.innerHTML="200" - _100;
            }
            if (t === '2') {
                distanceWritePrevInfo.innerHTML="300" - _100;
            }
            if (t === '3') {
                distanceWritePrevInfo.innerHTML="400" - _100;
            }
            if (t === '4') {
                distanceWritePrevInfo.innerHTML="500" - _100;
            }
            if (t === '5') {
                distanceWritePrevInfo.innerHTML="600" - _100;
            }
            if (t === '6') {
                distanceWritePrevInfo.innerHTML="700" - _100;
            }
            if (t === '7') {
               distanceWritePrevInfo.innerHTML="800" - _100;
            }
            if (t === '8') {
                distanceWritePrevInfo.innerHTML="900" - _100;
            }
            if (t === '9') {
                distanceWritePrevInfo.innerHTML="1000" - _100;
            }
            if (t === '10') {
                distanceWritePrevInfo.innerHTML="1100" - _100;
            }
            if (t === '11') {
                distanceWritePrevInfo.innerHTML="1200" - _100;
            }
            if (t === '12') {
                distanceWritePrevInfo.innerHTML="1300" - _100;
            }
            if (t === '13') {
                distanceWritePrevInfo.innerHTML="1400" - _100;
            }
            if (t === '14') {
                distanceWritePrevInfo.innerHTML="1500" - _100;
            }
            if (t === '15') {
                distanceWritePrevInfo.innerHTML="1600" - _100;
            }
            if (t === '16') {
                distanceWritePrevInfo.innerHTML="1700" - _100;
            }
            if (t === '17') {
                distanceWritePrevInfo.innerHTML="1800" - _100;
            }
            if (t === '18') {
                distanceWritePrevInfo.innerHTML="1900" - _100;
            }
            if (t === '19') {
                distanceWritePrevInfo.innerHTML="2000" - _100;
            }
        }
        WritePrevDist();

        function WritePrev2Dist() {
            let distanceWritePrev2Info = document.getElementById('distanceWritePrev2Info');
            let t = distanceWritePrev2Info.textContent;

            if (t === '0') {
                distanceWritePrev2Info.innerHTML="100" - _200;
            }
            if (t === '1') {
                distanceWritePrev2Info.innerHTML="200" - _200;
            }
            if (t === '2') {
                distanceWritePrev2Info.innerHTML="300" - _200;
            }
            if (t === '3') {
                distanceWritePrev2Info.innerHTML="400" - _200;
            }
            if (t === '4') {
                distanceWritePrev2Info.innerHTML="500" - _200;
            }
            if (t === '5') {
                distanceWritePrev2Info.innerHTML="600" - _200;
            }
            if (t === '6') {
                distanceWritePrev2Info.innerHTML="700" - _200;
            }
            if (t === '7') {
                distanceWritePrev2Info.innerHTML="800" - _200;
            }
            if (t === '8') {
                distanceWritePrev2Info.innerHTML="900" - _200;
            }
            if (t === '9') {
                distanceWritePrev2Info.innerHTML="1000" - _200;
            }
            if (t === '10') {
                distanceWritePrev2Info.innerHTML="1100" - _200;
            }
            if (t === '11') {
                distanceWritePrev2Info.innerHTML="1200" - _200;
            }
            if (t === '12') {
                distanceWritePrev2Info.innerHTML="1300" - _200;
            }
            if (t === '13') {
                distanceWritePrev2Info.innerHTML="1400" - _200;
            }
            if (t === '14') {
                distanceWritePrev2Info.innerHTML="1500" - _200;
            }
            if (t === '15') {
                distanceWritePrev2Info.innerHTML="1600" - _200;
            }
            if (t === '16') {
                distanceWritePrev2Info.innerHTML="1700" - _200;
            }
            if (t === '17') {
                distanceWritePrev2Info.innerHTML="1800" - _200;
            }
            if (t === '18') {
                distanceWritePrev2Info.innerHTML="1900" - _200;
            }
            if (t === '19') {
                distanceWritePrev2Info.innerHTML="2000" - _200;
            }
        }
        WritePrev2Dist();

        function WritePrev3Dist() {
            let distanceWritePrev3Info = document.getElementById('distanceWritePrev3Info');
            let t = distanceWritePrev3Info.textContent;

            if (t === '0') {
                distanceWritePrev3Info.innerHTML="100" - _300;
            }
            if (t === '1') {
                distanceWritePrev3Info.innerHTML="200" - _300;
            }
            if (t === '2') {
                distanceWritePrev3Info.innerHTML="300" - _300;
            }
            if (t === '3') {
                distanceWritePrev3Info.innerHTML="400" - _300;
            }
            if (t === '4') {
                distanceWritePrev3Info.innerHTML="500" - _300;
            }
            if (t === '5') {
                distanceWritePrev3Info.innerHTML="600" - _300;
            }
            if (t === '6') {
                distanceWritePrev3Info.innerHTML="700" - _300;
            }
            if (t === '7') {
                distanceWritePrev3Info.innerHTML="800" - _200;
            }
            if (t === '8') {
                distanceWritePrev3Info.innerHTML="900" - _300;
            }
            if (t === '9') {
                distanceWritePrev3Info.innerHTML="1000" - _300;
            }
            if (t === '10') {
                distanceWritePrev3Info.innerHTML="1100" - _300;
            }
            if (t === '11') {
                distanceWritePrev3Info.innerHTML="1200" - _300;
            }
            if (t === '12') {
                distanceWritePrev3Info.innerHTML="1300" - _300;
            }
            if (t === '13') {
                distanceWritePrev3Info.innerHTML="1400" - _300;
            }
            if (t === '14') {
                distanceWritePrev3Info.innerHTML="1500" - _300;
            }
            if (t === '15') {
                distanceWritePrev3Info.innerHTML="1600" - _300;
            }
            if (t === '16') {
                distanceWritePrev3Info.innerHTML="1700" - _300;
            }
            if (t === '17') {
                distanceWritePrev3Info.innerHTML="1800" - _300;
            }
            if (t === '18') {
                distanceWritePrev3Info.innerHTML="1900" - _300;
            }
            if (t === '19') {
                distanceWritePrev3Info.innerHTML="2000" - _300;
            }
        }
        WritePrev3Dist();

        function WriteNextDistance() {
            let distanceWriteNextInfo = document.getElementById('distanceWriteNextInfo');
            let t = distanceWriteNextInfo.textContent;

            if (t === '0') {
                distanceWriteNextInfo.innerHTML="100" - - _100;
            }
            if (t === '1') {
                distanceWriteNextInfo.innerHTML="200" - - _100;
            }
            if (t === '2') {
                distanceWriteNextInfo.innerHTML="300" - - _100;
            }
            if (t === '3') {
                distanceWriteNextInfo.innerHTML="400" - - _100;
            }
            if (t === '4') {
                distanceWriteNextInfo.innerHTML="500" - - _100;
            }
            if (t === '5') {
                distanceWriteNextInfo.innerHTML="600" - - _100;
            }
            if (t === '6') {
                distanceWriteNextInfo.innerHTML="700" - - _100;
            }
            if (t === '7') {
                distanceWriteNextInfo.innerHTML="800" - - _100;
            }
            if (t === '8') {
                distanceWriteNextInfo.innerHTML="900" - - _100;
            }
            if (t === '9') {
                distanceWriteNextInfo.innerHTML="1000" - - _100;
            }
            if (t === '10') {
                distanceWriteNextInfo.innerHTML="1100" - - _100;
            }
            if (t === '11') {
                distanceWriteNextInfo.innerHTML="1200" - - _100;
            }
            if (t === '12') {
                distanceWriteNextInfo.innerHTML="1300" - - _100;
            }
            if (t === '13') {
                distanceWriteNextInfo.innerHTML="1400" - - _100;
            }
            if (t === '14') {
                distanceWriteNextInfo.innerHTML="1500" - - _100;
            }
            if (t === '15') {
                distanceWriteNextInfo.innerHTML="1600" - - _100;
            }
            if (t === '16') {
                distanceWriteNextInfo.innerHTML="1700" - - _100;
            }
            if (t === '17') {
                distanceWriteNextInfo.innerHTML="1800" - -  _100;
            }
            if (t === '18') {
                distanceWriteNextInfo.innerHTML="1900" - - _100;
            }
            if (t === '19') {
                distanceWriteNextInfo.innerHTML="2000" - - _100;
            }
        }
        WriteNextDistance();


        //
        function WriteNext2Distance() {
            let distanceWriteNext2Info = document.getElementById('distanceWriteNext2Info');
            let t = distanceWriteNext2Info.textContent;

            if (t === '0') {
                distanceWriteNext2Info.innerHTML="100" - - _200;
            }
            if (t === '1') {
                distanceWriteNext2Info.innerHTML="200" - - _200;
            }
            if (t === '2') {
                distanceWriteNext2Info.innerHTML="300" - - _200;
            }
            if (t === '3') {
                distanceWriteNext2Info.innerHTML="400" - - _200;
            }
            if (t === '4') {
                distanceWriteNext2Info.innerHTML="500" - - _200;
            }
            if (t === '5') {
                distanceWriteNext2Info.innerHTML="600" - - _200;
            }
            if (t === '6') {
                distanceWriteNext2Info.innerHTML="700" - - _200;
            }
            if (t === '7') {
                distanceWriteNext2Info.innerHTML="800" - - _200;
            }
            if (t === '8') {
                distanceWriteNext2Info.innerHTML="900" - - _200;
            }
            if (t === '9') {
                distanceWriteNext2Info.innerHTML="1000" - - _200;
            }
            if (t === '10') {
                distanceWriteNext2Info.innerHTML="1100" - - _200;
            }
            if (t === '11') {
                distanceWriteNext2Info.innerHTML="1200" - - _200;
            }
            if (t === '12') {
                distanceWriteNext2Info.innerHTML="1300" - - _200;
            }
            if (t === '13') {
                distanceWriteNext2Info.innerHTML="1400" - - _200;
            }
            if (t === '14') {
                distanceWriteNext2Info.innerHTML="1500" - - _200;
            }
            if (t === '15') {
                distanceWriteNext2Info.innerHTML="1600" - - _200;
            }
            if (t === '16') {
                distanceWriteNext2Info.innerHTML="1700" - - _200;
            }
            if (t === '17') {
                distanceWriteNext2Info.innerHTML="1800" - - _200;
            }
            if (t === '18') {
                distanceWriteNext2Info.innerHTML="1900" - - _200;
            }
            if (t === '19') {
                distanceWriteNext2Info.innerHTML="2000" - - _200;
            }
        }
        WriteNext2Distance();

        function WriteNext3Distance() {
            let distanceWriteNext3Info = document.getElementById('distanceWriteNext3Info');
            let t = distanceWriteNext3Info.textContent;

            if (t === '0') {
                distanceWriteNext3Info.innerHTML="100" - - _300;
            }
            if (t === '1') {
                distanceWriteNext3Info.innerHTML="200" - - _300;
            }
            if (t === '2') {
                distanceWriteNext3Info.innerHTML="300" - - _300;
            }
            if (t === '3') {
                distanceWriteNext3Info.innerHTML="400" - - _300;
            }
            if (t === '4') {
                distanceWriteNext3Info.innerHTML="500" - - _300;
            }
            if (t === '5') {
                distanceWriteNext3Info.innerHTML="600" - - _300;
            }
            if (t === '6') {
                distanceWriteNext3Info.innerHTML="700" - - _300;
            }
            if (t === '7') {
                distanceWriteNext3Info.innerHTML="800" - - _300;
            }
            if (t === '8') {
                distanceWriteNext3Info.innerHTML="900" - - _300;
            }
            if (t === '9') {
                distanceWriteNext3Info.innerHTML="1000" - - _300;
            }
            if (t === '10') {
                distanceWriteNext3Info.innerHTML="1100" - - _300;
            }
            if (t === '11') {
                distanceWriteNext3Info.innerHTML="1200" - - _300;
            }
            if (t === '12') {
                distanceWriteNext3Info.innerHTML="1300" - - _300;
            }
            if (t === '13') {
                distanceWriteNext3Info.innerHTML="1400" - - _300;
            }
            if (t === '14') {
                distanceWriteNext3Info.innerHTML="1500" - - _300;
            }
            if (t === '15') {
                distanceWriteNext3Info.innerHTML="1600" - - _300;
            }
            if (t === '16') {
                distanceWriteNext3Info.innerHTML="1700" - - _300;
            }
            if (t === '17') {
                distanceWriteNext3Info.innerHTML="1800" - - _300;
            }
            if (t === '18') {
                distanceWriteNext3Info.innerHTML="1900" - - _300;
            }
            if (t === '19') {
                distanceWriteNext3Info.innerHTML="2000" - - _300;
            }
        }
        WriteNext3Distance();


        //remove not needed distance text

        function removePrevInfo () {
            let d = document.getElementById('distanceWritePrevInfo').textContent;
            if (d === '-200' || d === '-100' || d === '0'){
                document.getElementById('distanceWritePrevInfo').innerHTML='&nbsp;'
            }
        }
        removePrevInfo()

        function removePrev2Info () {
            let d = document.getElementById('distanceWritePrev2Info').textContent;
            if (d === '-200' || d === '-100' || d === '0'){
                document.getElementById('distanceWritePrev2Info').innerHTML='&nbsp;'
            }
        }
        removePrev2Info()

        function removePrev3Info () {
            let d = document.getElementById('distanceWritePrev3Info').textContent;
            if (d === '-200' || d === '-100' || d === '0'){
                document.getElementById('distanceWritePrev3Info').innerHTML='&nbsp;'
            }
        }
        removePrev3Info()

        function removeNextInfo () {
            let d = document.getElementById('distanceWriteNextInfo').textContent;
            if (d === '2100' || d === '2200' || d === '2300'){
                document.getElementById('distanceWriteNextInfo').innerHTML='&nbsp;'
            }
        }
        removeNextInfo()

        function removeNext2Info () {
            let d = document.getElementById('distanceWriteNext2Info').textContent;
            if (d === '2100' || d === '2200' || d === '2300'){
                document.getElementById('distanceWriteNext2Info').innerHTML='&nbsp;'
            }
        }
        removeNext2Info()


        function removeNext3Info () {
            let d = document.getElementById('distanceWriteNext3Info').textContent;
            if (d === '2100' || d === '2200' || d === '2300'){
                document.getElementById('distanceWriteNext3Info').innerHTML='&nbsp;'
            }
        }
        removeNext3Info()


        if (selectedTableRowIndex !== -1) {
            $(".row_" + selectedTableRowIndex).removeClass('selected');
        }
        selectedTableRowIndex = index;
        $(".row_" + index).addClass('selected');
        return;
    }

    let msg;

    try {
        msg = JSON.parse(event.data);
    } catch (e) {
        return;
    }

    if (!webrtcPeerConnection) {
        webrtcPeerConnection = new RTCPeerConnection(webrtcConfiguration);
        webrtcPeerConnection.ontrack = onAddRemoteStream;
        //webrtcPeerConnection.onicecandidate = onIceCandidate;
    }

    switch (msg.type) {
        case "sdp":
            onIncomingSDP(msg.data);
            break;
        case "ice":
            onIncomingICE(msg.data);
            break;
        default:
            break;
    }
}
function tableChangeColor(){
    if(document.getElementById('table-content').style.display == 'block' === true){
        document.getElementById('mode_button_table').style.backgroundColor='rgba(144, 215, 145, 0.0)';
    }
    if(document.getElementById('table-content').style.display == 'block' === false){
        document.getElementById('mode_button_table').style.backgroundColor='rgba(76, 175, 80, 0.5)';
    }
}
setInterval(tableChangeColor,0);

function settingsChangeColor(){
    if(document.getElementById('dropdown-content-pref').style.display == 'block' === true){
        document.getElementById('mode_button_pref').style.backgroundColor='rgba(144, 215, 145, 0.0)';
    }
    if(document.getElementById('dropdown-content-pref').style.display == 'block' === false){
        document.getElementById('mode_button_pref').style.backgroundColor='rgba(76, 175, 80, 0.5)';
    }
}
setInterval(settingsChangeColor,0);

function onMouseOverButtonPref() {
        $(".dropdown-content-pref").css('display', 'block');
}

function onMouseLeaveButtonPref() {
        $(".dropdown-content-pref").css('display', 'none');
}


function playStream(videoElement, configuration, reportErrorCB) {
    html5VideoElement = videoElement;
    webrtcConfiguration = configuration;
    reportError = (reportErrorCB != undefined) ? reportErrorCB : function (text) {
    };

    ws.addEventListener("message", onServerMessage);
}


function scheduled_sender() {
    ws.send("{\"comm\":[\"OK\"]}");
}

let isRunned = true;

function run_once() {
    if (isRunned) {
        ws.send(arr[0]);
        ws.send(arr[1]);
        ws.send(arr[2]);
        ws.send(arr[3]);
        isRunned = false;
    }
}

function detectMob() {
    const toMatch = [
        /Android/i,
        /webOS/i,
        /iPhone/i,
        /iPad/i,
        /iPod/i,
        /BlackBerry/i,
        /Windows Phone/i
    ];

    return toMatch.some((toMatchItem) => {
        return navigator.userAgent.match(toMatchItem);
    });
}

let camCommCounter = 0;

function pDotJoystickScheduleSender() {
    if (isPDotP) {
        ws.send("{\"comm\":[\"PDOT_P\"]}");
    }
    if (isPDotM) {
        ws.send("{\"comm\":[\"PDOT_M\"]}");
    }
}

function joystickScheduleSender() {
    let movStr = "bt:";

    switch (joyCtrlCamera.GetDir()) {
        case "C":
            return;
        case "N":
            movStr += "01000000000";
            break;
        case "NE":
            movStr += "01000100000";
            break;
        case "E":
            movStr += "00000100000";
            break;
        case "SE":
            movStr += "00001100000";
            break;
        case "S":
            movStr += "00001000000";
            break;
        case "SW":
            movStr += "00011000000";
            break;
        case "W":
            movStr += "00010000000";
            break;
        case "NW":
            movStr += "01010000000";
            break;
    }
    camCommCounter++;
    if (camCommCounter == 1) {
        ws.send(movStr);
        camCommCounter = 0;
    }
}


$(function begin() {
    document.addEventListener('keydown', (e) => {
        if (!e.repeat) {
            set_keydown(e.key);
            send_control_message();
        } else {
            send_control_message();
        }

        processMeasuring();
    });

    document.addEventListener('keyup', (e) => {
        set_keyup(e.key);
    });


    ws = new WebSocket('ws://' + document.location.host + '/chart', ['string', 'foo']);
    let vidstream = document.getElementById("stream");
    // let config = {'iceServers': [{'urls': 'stun:stun.l.google.com:19302'}]};
    // let config = {'iceServers': [{'urls': 'stun:192.168.1.131:3478'}]};
    let config = {'iceServers': []};

    ws.onopen = function () {
        setTimeout(run_once, 2000);

        console.log('ws onopen');
    };

    ws.onclose = function () {
        console.log('ws onclose');
    };

    ws.onmessage = function(event) {
        console.log("Received from ws: " + event.data);
        getAllFromData(event.data);
      };

    ws.onerror = function (error) {
        console.log('ws error');
    };

    playStream(vidstream, config, function (errmsg) {
        console.error(errmsg);
    });

    setInterval(pDotJoystickScheduleSender, 30);

    setTimeout(connectSecondSocket, 1000);

    //stopInterval = setInterval(stopIntervalHandler, 50);
    setTimeout(stopIntervalHandler, 50);
});


function getAllFromData(data) {
    try {
        const r = JSON.parse(data);

        if (typeof r["error"] != "undefined") {
            errorTextOut(r["error"]);
        }

        if (typeof r["tracker_target"] != "undefined") {
            // examle message from tracker:
            // {"tracker_target":{"t_dx":0.054355,"t_dy":0.131211}}
            traker_target = r["tracker_target"];
        }

        if (typeof r["sizes"] != "undefined") {
            // examle message:
            // {"sizes":{"full_width":1280,"full_height":720,"zoom":0.345}}
            video_zoom = +r["sizes"]["zoom"];
            measuringOnChanges();
        }
    } catch(e) {
        console.log(e);
    }
}


function getTrackerDelta2(data) {
    try {
        // examle message from tracker:
        // {"tracker_target":{"t_dx":0.054355,"t_dy":0.131211}}
        const r = JSON.parse(data);
        //let dx = r["tracker_target"]["t_dx"];
        //let dy = r["tracker_target"]["t_dy"];

        if (typeof r["error"] != "undefined") {
            errorTextOut(r["error"]);
        }

        traker_target = r["tracker_target"];
    } catch(e) {
        console.log(e);
    }
}


let is_configs_sending_complete = false;

function stopIntervalHandler() {
    //clearInterval(stopInterval);
    //stopInterval = 0;
    try {

        // console.log("stopIntervalHandler: dragInterval=" + dragInterval + "   mouseMovingInterval=" + mouseMovingInterval);

        if (((typeof(dragInterval) == "undefined") || (dragInterval == 0)) && 
            ((typeof(mouseMovingInterval) == "undefined") || (mouseMovingInterval == 0)) &&
            ((typeof(colibration_interval) == "undefined") || (colibration_interval == 0))) {

                let st = "{\"comm\":[\"MV_HLD:DX:0:DY:0\"]}";

                if (typeof(traker_target) != "undefined") {
                    let dx = traker_target["t_dx"];
                    let dy = traker_target["t_dy"];

                    dx = Math.floor(dx * 1000) / 1000;
                    dy = Math.floor(dy * 1000) / 1000;

                    st = "{\"comm\":[\"MV_HLD:DX:" + dx + ":DY:" + dy + "\"]}"; 

                    traker_target = undefined;
                } else {
                    if (typeof GAMEPAD_PRESENT == "undefined") {

                    } else {
                        if (GAMEPAD_PRESENT) { 
                            st = "{\"comm\":[\"MV_HLD:DX:" + gamepad_axe_x + ":DY:" + gamepad_axe_y + "\"]}"; 
                        }
                    }
                }

                if (typeof IS_SEND_CONFIGS_NEEDED != "undefined") {
                    if (IS_SEND_CONFIGS_NEEDED && !is_configs_sending_complete) {
                        let xmf = OX_STEPPER_MAX_STEPS_FREQUENCEY;
                        let ymf = OY_STEPPER_MAX_STEPS_FREQUENCEY;

                        st = '{"config":{"OX_MAX_FREQ":' + xmf + ';"OY_MAX_FREQ":' + ymf + '}}'; 
                    }
                }

                ws.send(st);
                ws2.send(st);
        }
    
    } catch (error) {
        
    }
    //stopInterval = setInterval(stopIntervalHandler, 50);
    setTimeout(stopIntervalHandler, 50);
}


function connectSecondSocket() {
    //ws2 = new WebSocket(WS2_URI, "string");
    ws2 = new WebSocket(WS2_URI);

    ws2.addEventListener('error', function (event) {
        console.log('ws2 error: ', event);
      });

    ws2.onopen = function () {
        setTimeout(ws2_send_once, 2000);
        console.log("ws2 open");
    };

    ws2.onclose = function () {
        console.log("ws2 close");
        setTimeout(connectSecondSocket, 1000);
    };

    ws2.onmessage = function(event) {
        if (WS2_MESSAGES_LOG) {
            console.log("Received from ws2: " + event.data);
        }
        getMotorsData(event.data);
      };

    ws2.onerror = function () {
        console.log("ws2 error");
    };
};


function ws2_send_once() {
    if (ws2) {
        ws2.send("Set ws2 connection.");
    }
}


function getMotorsData(mess) {
    try {
        const r = JSON.parse(mess);

        // mess example
        // {"error":"Fatal error on line 48!"}
        if (typeof r["error"] != "undefined") {
            errorTextOut(r["error"]);
            return;
        }

        // mess example
        // {"resp":{"STP_X":-3509,"STP_Y":955,"SW_X_MAX":0,"SW_X_MIN":0,"SW_Y_MAX":0,"SW_Y_MIN":0}}
        if (typeof r["resp"] != "undefined") {
            x_position_steps = r.resp.STP_X;
            y_position_steps = r.resp.STP_Y;

            x_limit_max_flag = (r.resp.SW_X_MAX != 0);
            x_limit_min_flag = (r.resp.SW_X_MIN != 0);
            y_limit_max_flag = (r.resp.SW_Y_MAX != 0);
            y_limit_min_flag = (r.resp.SW_Y_MIN != 0);

            if (x_limit_max_flag) {
                document.getElementById("xMax").classList.remove("hidden-class");
            } else {
                document.getElementById("xMax").classList.add("hidden-class");
            }

            if (x_limit_min_flag) {
                document.getElementById("xMin").classList.remove("hidden-class");
            } else {
                document.getElementById("xMin").classList.add("hidden-class");
            }

            if (y_limit_max_flag) {
                document.getElementById("yMax").classList.remove("hidden-class");
            } else {
                document.getElementById("yMax").classList.add("hidden-class");
            }

            if (y_limit_min_flag) {
                document.getElementById("yMin").classList.remove("hidden-class");
            } else {
                document.getElementById("yMin").classList.add("hidden-class");
            }


            if (!is_got_start_position) {
                // x_start_position_steps = +localStorage.getItem("x_start_position_steps");
                // if (isNaN(x_start_position_steps)) {
                //     x_start_position_steps = x_position_steps;
                // }
                // y_start_position_steps = +localStorage.getItem("y_start_position_steps");
                // if (isNaN(y_start_position_steps)) {
                //     y_start_position_steps = y_position_steps;
                // }
                // x_steps_per_revolution = +localStorage.getItem("x_steps_per_revolution");
                // if (isNaN(x_steps_per_revolution)) {
                //     x_steps_per_revolution = 60000;
                // }
                // y_steps_per_revolution = +localStorage.getItem("y_steps_per_revolution");
                // if (isNaN(y_steps_per_revolution)) {
                //     y_steps_per_revolution = 60000;
                // }

                x_start_position_steps = +getCookie("x_start_position_steps");
                if (typeof x_start_position_steps == "undefined" || isNaN(x_start_position_steps)) {
                    x_start_position_steps = x_position_steps;
                }
                y_start_position_steps = +getCookie("y_start_position_steps");
                if (typeof y_start_position_steps == "undefined" || isNaN(y_start_position_steps)) {
                    y_start_position_steps = y_position_steps;
                }
                x_steps_per_revolution = +getCookie("x_steps_per_revolution");
                if (typeof x_steps_per_revolution == "undefined" || isNaN(x_steps_per_revolution)) {
                    x_steps_per_revolution = default_steps_per_revolution;
                }
                y_steps_per_revolution = +getCookie("y_steps_per_revolution");
                if (typeof y_steps_per_revolution == "undefined" || isNaN(y_steps_per_revolution)) {
                    y_steps_per_revolution = default_steps_per_revolution;
                }

                bind_x_correction = +getCookie("bind_x_correction");
                if (typeof bind_x_correction == "undefined" || isNaN(bind_x_correction)) {
                    bind_x_correction = 0.0;
                }
                bind_y_correction = +getCookie("bind_y_correction");
                if (typeof bind_y_correction == "undefined" || isNaN(bind_y_correction)) {
                    bind_y_correction = 0.0;
                }

                is_got_start_position = true;
            }

            var dx_steps = x_position_steps - x_start_position_steps;
            var dy_steps = y_position_steps - y_start_position_steps;

            x_angle_position_degree = 1.0 * dx_steps / x_steps_per_revolution * 360;
            y_angle_position_degree = 1.0 * dy_steps / y_steps_per_revolution * 360;

            x_angle_position_degree = angleConstraint(x_angle_position_degree);
            y_angle_position_degree = angleConstraint(y_angle_position_degree);
        }

        // mess example
        // {"confirm":{"OX_MAX_FREQ":6000;"OY_MAX_FREQ":5000}}
        if (typeof r["confirm"] != "undefined") {
            // check confirmation
            if (r["confirm"]["OX_MAX_FREQ"] == OX_STEPPER_MAX_STEPS_FREQUENCEY &&
            r["confirm"]["OY_MAX_FREQ"] == OY_STEPPER_MAX_STEPS_FREQUENCEY) {
                is_configs_sending_complete = true;
            }
            return;
        }

    } catch(e) {
        console.log(e);
    }
}


function angleConstraint(ang) {
    while (ang > 360.0) {
        ang = ang - 360.0;
    }
    while (ang < 0) {
        ang = ang + 360.0;
    }
    return ang;
}


let colibration_interval = 0;
let colibration_started = false;
let colibration_stage = 0;

const COLIBRATION_STAGE_GO_INSIDE = 0;
const COLIBRATION_STAGE_GO_MIN = 1;
const COLIBRATION_STAGE_GO_MAX = 2;
const COLIBRATION_STAGE_CALCS = 3;
const COLIBRATION_STAGE_GO_CENTER = 4;

let colibration_speed = 0.6;

let x_min_in_steps;
let y_min_in_steps;
let x_max_in_steps;
let y_man_in_steps;


function startStopColibration()
{
    if (!colibration_started) {
        colibration_started = true;
        colibration_stage = COLIBRATION_STAGE_GO_INSIDE;
        console.log("Colibration started.");
        colibration_interval = setInterval(doColebration, 50);
        return;
    }
    if (colibration_started) {
        colibration_started = false;
        clearInterval(colibration_interval);
        colibration_interval = 0;
        console.log("Colibration stopped.");
        return;
    }
}


let d_stp_x;
let d_stp_y;
let d_stp_x_sign = 1;
let d_stp_y_sign = 1;

function doColebration() {
    console.log("colibration_stage: " + colibration_stage);

    if (colibration_stage == COLIBRATION_STAGE_GO_INSIDE)
    {
        if (y_limit_max_flag) {
            // go up
            step(0, -colibration_speed);
            return;
        } 

        if (y_limit_min_flag) {
            // go down
            step(0, colibration_speed);
            return;
        } 

        if (x_limit_max_flag) {
            //go left
            step(-colibration_speed, 0);
            return;
        }

        if (x_limit_min_flag) {
            //go right
            step(colibration_speed, 0);
            return;
        }

        colibration_stage = COLIBRATION_STAGE_GO_MIN;
        return;
    }

    if (colibration_stage == COLIBRATION_STAGE_GO_MIN) {
        let sp_x = 0;
        let sp_y = 0;

        if (!x_limit_min_flag) {
            //go left
            sp_x = -colibration_speed;
        }

        if (!y_limit_min_flag) {
            // go up
            sp_y = -colibration_speed;
        }

        if (sp_x != 0 || sp_y != 0) {
            step(sp_x, sp_y);
            return;
        }

        // remember minimums
        x_min_in_steps = x_position_steps;
        y_min_in_steps = y_position_steps;

        colibration_stage = COLIBRATION_STAGE_GO_MAX;
        return;
    }

    if (colibration_stage == COLIBRATION_STAGE_GO_MAX) {
        let sp_x = 0;
        let sp_y = 0;

        if (!x_limit_max_flag) {
            //go right
            sp_x = colibration_speed;
        }

        if (!y_limit_max_flag) {
            // go down
            sp_y = colibration_speed;
        }

        if (sp_x != 0 || sp_y != 0) {
            step(sp_x, sp_y);
            return;
        }

        // remember maximums
        x_max_in_steps = x_position_steps;
        y_max_in_steps = y_position_steps;

        colibration_stage = COLIBRATION_STAGE_CALCS;
        return;
    }

    if (colibration_stage == COLIBRATION_STAGE_CALCS) {
        // do claibration calcs

        x_steps_per_revolution = (x_max_in_steps - x_min_in_steps) / FULL_ANGLE_OF_VIEW_OX_DEGREES * 360;
        y_steps_per_revolution = (y_max_in_steps - y_min_in_steps) / FULL_ANGLE_OF_VIEW_OY_DEGREES * 360;
        if (x_steps_per_revolution < 0) {
            x_steps_per_revolution = -x_steps_per_revolution;
        }
        if (y_steps_per_revolution < 0) {
            y_steps_per_revolution = -y_steps_per_revolution;
        }
        //localStorage.setItem("x_steps_per_revolution", x_steps_per_revolution);
        //localStorage.setItem("y_steps_per_revolution", y_steps_per_revolution);
        setCookie("x_steps_per_revolution", x_steps_per_revolution, {'max-age': COOKI_KEEP_7_DEYS});
        setCookie("y_steps_per_revolution", y_steps_per_revolution, {'max-age': COOKI_KEEP_7_DEYS});

        // Set the origin of coordinates to the center.
        x_start_position_steps = (x_max_in_steps + x_min_in_steps) / 2;
        y_start_position_steps = (y_max_in_steps + y_min_in_steps) / 2;
        is_got_start_position = true;
        //localStorage.setItem("x_start_position_steps", x_start_position_steps);
        //localStorage.setItem("y_start_position_steps", y_start_position_steps);
        setCookie("x_start_position_steps", x_start_position_steps, {'max-age': COOKI_KEEP_7_DEYS});
        setCookie("y_start_position_steps", y_start_position_steps, {'max-age': COOKI_KEEP_7_DEYS});

        // Calc motors motion directions for move to center position.
        d_stp_x = x_position_steps - x_start_position_steps;
        d_stp_y = y_position_steps - y_start_position_steps;
        d_stp_x_sign = ((d_stp_x > 0) ? 1 : -1);
        d_stp_y_sign = ((d_stp_y > 0) ? 1 : -1);
        colibration_stage = COLIBRATION_STAGE_GO_CENTER;
        return;
    }

    if (colibration_stage == COLIBRATION_STAGE_GO_CENTER) {
        // go left and up
        sp_x = 0;
        sp_y = 0;

        d_stp_x = x_position_steps - x_start_position_steps;
        d_stp_y = y_position_steps - y_start_position_steps;

        let slov_dist_steps = 90;
        let slov_colibration_speed = colibration_speed / 50;

        let x_diff_steps = (x_position_steps - x_start_position_steps) * d_stp_x_sign;
        if (x_diff_steps > 0) {
            sp_x = -colibration_speed;
            if (x_diff_steps < slov_dist_steps) {
                sp_x = -slov_colibration_speed;
            }
        }

        let y_diff_steps = (y_position_steps - y_start_position_steps) * d_stp_y_sign;
        if ( y_diff_steps > 0) {
            sp_y = -colibration_speed;
            if (y_diff_steps < slov_dist_steps) {
                sp_y = -slov_colibration_speed;
            }
        }
        
        if (sp_x != 0 || sp_y != 0) {
            step(sp_x, sp_y);
            return;
        }

        // stop colibration
        colibration_started = false;
        clearInterval(colibration_interval);
        colibration_interval = 0;
        return;
    }

    // default 
    step(0, 0);
}

function step(_dx, _dy) {
    let st = "{\"comm\":[\"MV_HLD:DX:" + _dx + ":DY:" + _dy + "\"]}"; 
    ws.send(st);
    ws2.send(st); 
}


function startTraining() {
    if (typeof IS_TREINING_AVAILABLE != 'undefined') {
        if (IS_TREINING_AVAILABLE) {
            trainingStart();
        }
    }
}


function startBinding() {
    document.getElementById("binding_div").classList.remove("hidden-class");
}

function binding() {
    document.getElementById("binding_div").classList.add("hidden-class");
    // alert("Binding...");
    let bind_x = +document.getElementById("x_binding_input").value;
    let bind_y = +document.getElementById("y_binding_input").value;
    // alert("Binding to " + bind_x + "," + bind_y);
    bindPointToXY(bind_x, bind_y);
}

function bindingCancel() {
    document.getElementById("binding_div").classList.add("hidden-class");
}

function bindingReset() {
    document.getElementById("binding_div").classList.add("hidden-class");
    bindPointReset();
}


function errorsClose() {
    document.getElementById("errors_div").classList.add("hidden-class");
}


function errorsShow() {
    document.getElementById("errors_div").classList.remove("hidden-class");
}


function errorTextOut(mes) {
    errorsShow();
    document.getElementById("errors_textarea").innerText = mes;
}


/*     Fullscreen btn     */
let fullscreen;
let fsEnter = document.getElementById('fullscr');
fsEnter.addEventListener('click', function (e) {
    e.preventDefault();
    if (!fullscreen) {
        fullscreen = true;
        document.documentElement.requestFullscreen();
        fsEnter.innerHTML = "<\i class=\"bi bi-fullscreen-exit\"></i>";
    }
    else {
        fullscreen = false;
        document.exitFullscreen();
        fsEnter.innerHTML = "<\i class=\"bi bi-arrows-fullscreen\"></i>";
    }
});



/*
let isMob = !detectMob();
console.info("isMob: " + isMob);

if (!isMob) {
    joyCtrlCamera = new JoyStick('joyCtrlCamera');

    setInterval(joystickScheduleSender, 30);

} else {
    document.getElementById("idButtonZoomMinus").style.visibility = "hidden";
    document.getElementById("idButtonZoomPlus").style.visibility = "hidden";
}
//let messageSender = setInterval(scheduled_sender, 100);
*/




//joyRCamera = new JoyStick('joyRCamera');

//setInterval(joystickScheduleSender, 30);
/*
$('#zoomslider').slider({
    slide: function(event,ui){
        setzoomspeed(ui.value)
    },

    stop: function(event,ui) {
        $('#zoomslider').slider("value",0);
        setzoomspeed(0);
    },
    min:-20,
    max:20,
    value:0
});

 */

function reload() {
    let e = document.getElementById('shootText');
    e.style.display="inline";
    e.innerHTML="Перезавантаження";
    setInterval(function (){
        location.reload()
    }, 8000);
    let zero = ws.send("{\"FIX_FC\":[{\"d\":100, \"v\":0.0, \"h\":0.0}, {\"d\":200, \"v\":0.3, \"h\":0.0}, {\"d\":300, \"v\":0.9, \"h\":0}, {\"d\":400, \"v\":1.7, \"h\":0}, {\"d\":500, \"v\":2.800000, \"h\":0.000000}, {\"d\":600, \"v\":3.900000, \"h\":0.000000}, {\"d\":700, \"v\":5.000000, \"h\":0.000000}, {\"d\":800, \"v\":6.100000, \"h\":0.000000}, {\"d\":900, \"v\":7.200000, \"h\":0.000000}, {\"d\":1000, \"v\":8.800000, \"h\":0.000000}, {\"d\":1100, \"v\":9.800000, \"h\":0.000000}, {\"d\":1200, \"v\":11.800000, \"h\":0.000000}, {\"d\":1300, \"v\":13.800000, \"h\":0.000000}, {\"d\":1400, \"v\":15.800000, \"h\":0.000000}, {\"d\":1500, \"v\":17.800000, \"h\":0.000000}, {\"d\":1600, \"v\":20.800000, \"h\":0.000000}, {\"d\":1700, \"v\":22.800000, \"h\":0.000000}, {\"d\":1800, \"v\":25.800000, \"h\":0.000000}, {\"d\":1900, \"v\":28.800000, \"h\":0.000000}, {\"d\":2000, \"v\":31.800000, \"h\":0.000000}]}");
    console.log(zero);
}


//function menu open-close
(function() {
    const body = document.body;
    const burgerMenu = document.getElementsByClassName('b-menu')[0];
    const burgerContain = document.getElementsByClassName('b-container')[0];
    const burgerNav = document.getElementsByClassName('b-nav')[0];

    burgerMenu.addEventListener('click', function toggleClasses() {
        [body, burgerContain, burgerNav].forEach(function (el) {
            el.classList.toggle('open');
        });
    }, false);
})();


// setInterval(function() {
//     document.getElementById('verticalSlider').style.width="70px";
// }, 0);