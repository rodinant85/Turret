let MOUSE_EVENTS_LOG = false;
let MOUSE_COORDINATE_LOG = false;


// Approximate values.
// The exact values are set after calibration.
let default_steps_per_revolution = 36000;
let x_steps_per_revolution = default_steps_per_revolution;
let y_steps_per_revolution = default_steps_per_revolution;

let x0_clickCoord = -1;
let y0_clickCoord = -1;
let x1_clickCoord = -1;
let y1_clickCoord = -1;

let DX;
let DY;
let line;

let clearLine;

let defaultTable = "{\"comm\":[\"CLR_FULL_TBL\"]}";
//indexTable = ws_send("{\"comm\":[\"GET_FCI\"]}");
// var g;
// g = document.createElement('div')
// var gg;
// gg = document.getElementById('HS').createElement('input')
// gg.id = 'horizSlider';
// gg.type='range';


function cl() {
    console.log(ws_send(clearLine));
    ws_send("{\"comm\":[\"GET_FCA\"]}");
}

function setDefaultTableIndex() {
    console.log(ws_send("{\"comm\":[\"CLR_TBL_REC:" + indexTable + "\"]}"));
    ws_send("{\"comm\":[\"GET_FCA\"]}");
    console.log("Index is clear");
}

function setDefaultTable() {
    let e = document.getElementById('clear-table');
    // let e1 = document.getElementById('clear-table');
    e.style.display="block"
    e.innerHTML="Впевнені? <div class='clear-table-accept' onclick='setDefaultTableAccept()'>Так</div><div class='clear-table-decline' onclick='clearTableDecline()'>Ні</div>";
    // e1.style.display="inline";
    // e1.innerHTML="Дядя ты уверен? <div class='clear-table-accept' onclick='setDefaultTableAccept()'>Так</div><div class='clear-table-decline' onclick='clearTableDecline()'>Ні</div>";

    //e.style.display="inline";
    //e.classList.add('shootTextAlarm');
}

function reloadDecline() {
    let e = document.getElementById('ask-reload');
    // let e1 = document.getElementById('clear-table');
    e.style.display="none";
}

function askReload() {
    let e = document.getElementById('ask-reload');
    // let e1 = document.getElementById('clear-table');
    e.style.display="inline"
    e.innerHTML="Впевнені? <div class='clear-table-accept' onclick='reload()'>Так</div><div class='clear-table-decline' onclick='reloadDecline()'>Ні</div>";

    // e1.style.display="inline";
    // e1.innerHTML="Дядя ты уверен? <div class='clear-table-accept' onclick='setDefaultTableAccept()'>Так</div><div class='clear-table-decline' onclick='clearTableDecline()'>Ні</div>";

    //e.style.display="inline";
    //e.classList.add('shootTextAlarm');
}

function clearTableAcceptText() {
    document.getElementById("shootText").style.display="none";
}

function setDefaultTableAccept() {
    console.log(ws_send(defaultTable))
    ws_send("{\"comm\":[\"GET_FCA\"]}");
    let e = document.getElementById('clear-table');
    //let e1 = document.getElementById('clear-table1');
    document.getElementById("shootText").style.display="inline";
    document.getElementById("shootText").innerHTML="Таблиця очищена!";
    console.log("Table is clear");
    e.style.display="none";
    //e1.style.display="none";
    setTimeout(clearTableAcceptText, 3000);
}

function clearTableDecline() {
    let e = document.getElementById('clear-table');
    //let e1 = document.getElementById('clear-table1'); // For button set default table in setting
    e.style.display="none";
    //e1.style.display="none"; // For button set default table in setting
}


let isFireCorrectionProcessed = false;


document.getElementById("stream").onclick = function (event) {
    if (!isFireCorrectionProcessed)
        return;
    event = event || window.Event;

    if (x0_clickCoord === -1) {
        x0_clickCoord = event.offsetX;
        y0_clickCoord = event.offsetY;
        document.getElementById("shootText").innerHTML="Місце влучання";
        document.getElementById("shootText").style.display="inline";
    } else {
        x1_clickCoord = event.offsetX;
        y1_clickCoord = event.offsetY;

        // todo some events
        let WIDTH = document.getElementById("stream").offsetWidth;
        let HEIGHT = document.getElementById("stream").offsetHeight;
        DX = (x0_clickCoord - x1_clickCoord) / WIDTH;
        DY = (y0_clickCoord - y1_clickCoord) / HEIGHT;
        line = "{\"comm\":[\"FIX_FC:DX:" + (Math.round(DX * 10000) / 10000) + ":DY:" + (Math.round(DY * 10000) / 10000) + "\"]}";

        clearLine = "{\"comm\":[\"FIX_FC:DX:" + (-DX) + ":DY:" + (-DY) + "\"]}";

        if (DX !== 0 || DY !== 0) {
            ws_send(line);
            // then request table again
            ws_send("{\"comm\":[\"GET_FCA\"]}");
        }


        console.log(line);

        document.getElementById("stream").style.cursor = "default";
        document.getElementById("table_check").checked = false;
        $(".table-content").css('display', 'none');

        isFireCorrectionProcessed = false;
        document.getElementById("canvas-square").style.display = "block";
        document.getElementById("changeText").innerHTML="Змінити";
        document.getElementById("shootText").innerHTML="";
        document.getElementById("shootText").style.display="none";
        x0_clickCoord = -1;
    }
};

function onMouseOverButtonTable() {
    if (!isFireCorrectionProcessed) {
        document.getElementById("canvas-square").style.display = "none";
        $(".table-content").css('display', 'block');
        $(".distInfo").css('display', 'none');
    }
}

setInterval(function (){
    if (document.getElementById("table-content").style.display==="none") {
        $(".distInfo").css('display', 'block');
    }
},100);

function onMouseLeaveButtonTable() {
    if (!isFireCorrectionProcessed) {
        document.getElementById("canvas-square").style.display = "block";
        $(".distInfo").css('display', 'block');
        $(".table-content").css('display', 'none');
    }
}

function RegistrationModeIsOnTextAlarm() {
    let e = document.getElementById('shootText');
    e.innerHTML="Корекція увімкнена, Вимкніть, перед використанням Прицілки";
    e.style.display="inline";
    e.classList.add('shootTextAlarm');
}

function RegistrationModeIsOnTextAlarmClear() {
    let e = document.getElementById('shootText');
    e.innerHTML="";
    e.style.display="none";
    e.classList.remove('shootTextAlarm');
}

function SwappedStreamIsOnTextAlarm() {
    let e = document.getElementById('shootText');
    e.innerHTML="Камеру змінено, Змініть, перед використанням Прицілки";
    e.style.display="inline";
    e.classList.add('shootTextAlarm');
}

function SwappedStreamIsOnTextAlarmClear() {
    let e = document.getElementById('shootText');
    e.innerHTML="";
    e.style.display="none";
    e.classList.remove('shootTextAlarm');
}

function table_check() {
    if (!$("#table_check").prop("checked")) {
        document.getElementById("stream").style.cursor = "default";
        x0_clickCoord = -1;
        isFireCorrectionProcessed = false;
        document.getElementById("changeText").innerHTML = "Змінити";
        document.getElementById("shootText").innerHTML = "";
        document.getElementById("shootText").style.display = "none";
    } else {
        // check is fire correction mode, if enable alert should tell user about it
        if (isRegistrationMode) {
            RegistrationModeIsOnTextAlarm();
            console.log("Корекція увімкнена, Вимкніть, перед використанням Прицілки");
            clearTimeout(this.timeout1);
            clearTimeout(this.timeout2);
            this.timeout1 = setTimeout(RegistrationModeIsOnTextAlarmClear, 3000);
            document.getElementById("table_check").checked = false;
            return;
        }

        // check is swapped streams, if enabled alert should tell user about it
        if (isStreamsSwapped) {
            SwappedStreamIsOnTextAlarm();
            console.log("Камеру змінено, Поверніться до обраного режиму!");
            clearTimeout(this.timeout1);
            clearTimeout(this.timeout2);
            this.timeout2 = setTimeout(SwappedStreamIsOnTextAlarmClear, 3000);
            document.getElementById("table_check").checked = false;
            return;
        }

        // don't allow user to set fire correction mode
        // this done in other function in file 'app.js'

        // don't allow user to swap streams
        // this done in other function in file 'app.js'

        // don't allow user to do zooming
        // this done in other function in the file 'app.js'
        // and this done the other function in the file 'keyboardMouseKeys.js'

        // don't allow user to change pusuiting distance
        // thin done in other function in the file 'app.js'
        // this done in other function in the file 'WSADKeys.js'


        // fix table until done
        $(".table-content").css('display', 'block');

        //  update table and update index
        // this done on the end of double click in the 'onclick' function in the file 'coordinates.js

        document.getElementById("stream").style.cursor = "crosshair";
        isFireCorrectionProcessed = true;
        document.getElementById("shootText").classList.remove('shootTextAlarm');
        clearTimeout(this.timeout1);
        clearTimeout(this.timeout2);
        document.getElementById("changeText").innerHTML = "Корекція";
        document.getElementById("shootText").innerHTML = "Місце прицілювання";
        document.getElementById("canvas-square").style.display = "none";
        document.getElementById("shootText").style.display = "inline";
    }
}
let x, y, i = 0;

let stream = document.getElementById('stream');
let HW = stream.clientWidth/2;
let HH = stream.clientHeight/2;

let streamWidthOffset = stream.getBoundingClientRect().x;

// [DX_2, DY_2] = [degree, degree]
// A full circle is 360 degrees.
let DX_2 = 0.0; // = (x - (HW / 2)) / (HW / 2);
let DY_2 = 0.0; // = (y - (HH / 2)) / (HH / 2);

// Real video size object
let rvs;

let VIDEO_ZOOM_DEFAULT = 0.8;
// Current video zoom
let video_zoom = VIDEO_ZOOM_DEFAULT;


let is_got_start_position = false;
// Start motors position. [step].
// (This value (in steps) corresponds to the origin.)
let x_start_position_steps = 0;
let y_start_position_steps = 0;

// Absolute motors position. [step].
let x_position_steps = 0;
let y_position_steps = 0;

// Limit switches.
let x_limit_min_flag = false;
let x_limit_max_flag = false;
let y_limit_min_flag = false;
let y_limit_max_flag = false;

// Current angle position. [degree].
let x_angle_position_degree = 0;
let y_angle_position_degree = 0;


let isMouseDownMove = false;
const canvas = document.getElementById('canvas-square');

let mouseMovingInterval;

// function getCursorPosition(canvas, event) {
//     isMouseDownMove = true;
//
//     if (isMouseDownMove) {
//         // const rect = canvas.getBoundingClientRect();
//         // x = event.clientX - rect.left;
//         // y = event.clientY - rect.top;
//         x = event.clientX;
//         y = event.clientY;
//         console.log("x: " + x + " y: " + y);
//         //ws_send("{\"comm\":{\"MV_HLD:DX:" + (Math.round(x * 1000) / 1000) + ":DY:" + (Math.round(y * 1000) / 1000) + "\"}}")
//
//     }
//
//     if (!isMouseDownMove) {
//         alert("rg")
//     }
// }


// Assume that the video size is maximized and without crop.
function recalculateRealVideoSize() {
    let ret_obj = {};

    let place_w = stream.clientWidth;
    let place_h = stream.clientHeight;
    let video_w = stream.videoWidth;
    let video_h = stream.videoHeight;

    if (isNaN(video_w) && isNaN(video_h)) {
        // mjpeg stream case
        video_w = stream.naturalWidth;
        video_h = stream.naturalHeight;
    }

    if (place_w / video_w > place_h / video_h) {
        // The place wider then video.
        ret_obj.w = video_w * place_h / video_h;
        ret_obj.h = place_h;
        ret_obj.offset_x = (place_w - ret_obj.w) / 2;
        ret_obj.offset_y = 0;
    } else {
        // The place higher then video.
        ret_obj.w = place_w;
        ret_obj.h = video_h * place_w / video_w;
        ret_obj.offset_x = 0;
        ret_obj.offset_y = (place_h - ret_obj.h) / 2;
    }

    ret_obj.w = Math.floor(ret_obj.w);
    ret_obj.h = Math.floor(ret_obj.h);
    ret_obj.offset_x = Math.floor(ret_obj.offset_x);
    ret_obj.offset_y = Math.floor(ret_obj.offset_y);

    return ret_obj;
}


function mouseCoordinateMessageSender() {
    try {
        if (isI_ButtonPushed) {
            // For test ws2 disconnect case uncomment next line.
            // return;
        }

        if (isNaN(DX_2)) {
            DX_2 = 0;
        }

        if (isNaN(DY_2)) {
            DY_2 = 0;
        }

        let st = '{"comm":["MV_HLD:DX:' + (Math.round(DX_2 * 1000) / 1000) + ':DY:' + (Math.round(DY_2 * 1000) / 1000) + '"]}'; 
        if (MOUSE_COORDINATE_LOG) {
            console.log(st);
        }
        ws_send(st);
        ws2.send(st);
    } catch(exeption) {

    }
    
    if (mouseMovingInterval != 0) {
        mouseMovingInterval = setTimeout(mouseCoordinateMessageSender, 50);
    }
}

canvas.addEventListener('mousedown', function(e) {
    if ("colibration_started" in window) {
        if (colibration_started) {
            return;
        }
    }

    isMouseDownMove = true;
    HW = stream.clientWidth / 2;
    HH = stream.clientHeight / 2;
   
    x = e.clientX;
    y = e.clientY;

    rvs = recalculateRealVideoSize();

    let dx = x - HW;
    let dy = y - HH;

    if (dx > rvs.w / 2) {dx = Math.floor(rvs.w / 2);}
    if (dx < -rvs.w / 2) {dx =  Math.floor(-rvs.w / 2);}
    if (dy > rvs.h / 2) {dy =  Math.floor(rvs.h / 2);}
    if (dy < -rvs.h / 2) {dy =  Math.floor(-rvs.h / 2);}

    DX_2 = dx / rvs.w;
    DY_2 = dy / rvs.h;
  
    if(isF_ButtonPushed === true) {
         let dw = stream.videoWidth;
         let dh = stream.videoHeight;

         let xp = dw / 2 * (1 + 2 * DX_2);
         let yp = dh / 2 * (1 + 2 * DY_2);
         if (xp < 0) {xp = 0;}
         if (xp > dw - 1) {xp = dw - 1;}
         if (yp < 0) {yp = 0;}
         if (yp > dh - 1) {yp = dh - 1;}
         xp = Math.floor(xp);
         yp = Math.floor(yp);
         console.log("Start tracking comand:");
         //         x    y   ID
         // coords: 1232 564 48

         //         ID
         // delete: 48

         // delatealltracker
         let st = "coords: " + xp + " " + yp;
         console.log(st);
         ws_send(st);   
         
         document.getElementById("tracker_div").classList.remove("hidden-class");
    } else {
        mouseMovingInterval = setTimeout(mouseCoordinateMessageSender, 50);
    }

    if (MOUSE_EVENTS_LOG) {
        console.log("canvas mousedown " + DX_2 + ", " + DY_2);
    }
});

canvas.addEventListener('mouseup', function (e) {
    //DX_2 = 0.0;
    //DY_2 = 0.0;

    isMouseDownMove = false;

    //clearInterval(mouseMovingInterval);
    mouseMovingInterval = 0;

    if (MOUSE_EVENTS_LOG) {
        console.log("canvas mouseup " + DX_2 + ", " + DY_2);
    }
});

canvas.addEventListener('mousemove', function (e) {
    if ("colibration_started" in window) {
        if (colibration_started) {
            return;
        }
    }

    if (isMouseDownMove) {
        x = e.clientX;
        y = e.clientY;
    
        let dx = x - HW;
        let dy = y - HH;
    
        if (dx > rvs.w / 2) {dx = Math.floor(rvs.w / 2);}
        if (dx < -rvs.w / 2) {dx =  Math.floor(-rvs.w / 2);}
        if (dy > rvs.h / 2) {dy =  Math.floor(rvs.h / 2);}
        if (dy < -rvs.h / 2) {dy =  Math.floor(-rvs.h / 2);}
    
        DX_2 = dx / rvs.w;
        DY_2 = dy / rvs.h;

        if (MOUSE_EVENTS_LOG) {
            console.log("canvas mousemove " + DX_2 + ", " + DY_2);
        }
    }
});

canvas.addEventListener('mouseout', function (e) {
    if (isMouseDownMove) {
        var clickEvent = document.createEvent ('MouseEvents');
        clickEvent.initEvent ('mouseup', true, true);
        canvas.dispatchEvent (clickEvent);

        if (MOUSE_EVENTS_LOG) {
            console.log("canvas mouseout");
        }
    }
});


function trackerStop() {
    document.getElementById("tracker_div").classList.add("hidden-class");
    // Need to send "Stop" command to tracker here.
    let st = "delatealltracker";
    console.log(st);
    ws_send(st);   
}

// function UIslider(){
//     {
//         let labels = [];
//         for (let i = -30; i <= 30; i += 1) {
//             labels.push(i);
//         }
//
//         $(function() {
//             // var $slide =
//             $("#horizSlider").rsSliderLens({
//                 id: 'rrr',
//                 step: 0.1,
//                 paddingStart: 0.1,
//                 paddingEnd: 0.1,
//                 width: 800,
//                 height: 65,
//                 value: tr[index + 1].cells[1].innerText,
//                 min: -30,
//                 max: 31.8,
//                 fixedHandle: true,
//                 ruler: {
//                     size: 6,
//                     labels: {
//                         values: [0, 0.3, 0.9, 1.7, 2.8, 3.9, 5, 6.1, 7.2, 8.8, 9.8, 11.8, 13.8, 15.8, 17.8, 20.8, 22.8, 25.8, 28.8, 31.8]
//                     }
//                 },
//                 range: {
//                     type: [0, 30],
//                 }
//             });
//            //setInterval($slide.slider($slide.slider), 500);
//         })
//
//
//     }
//
//
//     {
//         let labels = [];
//         for (let i = -30; i <= 30; i += 5) {
//             labels.push(i);
//         }
//
//         $("#verticalSlider").rsSliderLens({
//             step: 0.1,
//             paddingStart: 0.1,
//             paddingEnd: 0.1,
//             min: -30,
//             max: 31.8,
//             width: 65,
//             height: 400,
//             value: tr[index+1].cells[2].innerText,
//             fixedHandle: true,
//             left: -5,
//             ruler: {
//                 size: 5,
//                 labels: {
//                     values: [-30, 0, 0.3, 0.9, 1.7, 2.8, 3.9, 5, 6.1, 7.2, 8.8, 9.8, 11.8, 13.8, 15.8, 17.8, 20.8, 22.8, 25.8, 28.8, 31.8]
//                 }
//             },
//             range: {
//                 type: [0, 30],
//             }
//         });
//     }
// }

// setTimeout(UIslider, 1000);
// setInterval(function (){g.remove();
//
//     g = document.createElement('div');
//     g.id='HS';
//     g.className='ellipseHorizontal';}, 2000)
//
//


// import SlideRuler from 'slide-ruler';

function handleValue(value){
    console.log(value); //SlideRuler return value
    document.querySelector('#valuesH').innerHTML = value;
}

// new SlideRuler(
//     {
//         el: document.querySelector('#horizSlider'),
//         maxValue: 230,
//         minValue: 100,
//         currentValue: 180,
//         handleValue: handleValue,
//         precision: 1
//     }
// );


// function slideR(){
//     $("#horizSlider").rsSliderLens( {value: tr[index+1].cells[1].innerText})
// }
//
// setTimeout(slideR, 1000)
//
// function UIsliderRefresh(){
//     {
//         $(function() {
//             var $slide = $("#horizSlider").rsSliderLens({
//                 value: tr[index + 1].cells[1].innerText
//             });
//             //setInterval($slide.slider($slide.slider), 500);
//
//         })
//     }
//
//
//     {
//         $("#verticalSlider").rsSliderLens({
//             value: tr[index+1].cells[1].innerText
//         });
//     }
// }
//
// setTimeout(UIsliderRefresh, 1000);





/*

    if (x < i) {
        for (i = 0; i < x ; i++) {
            console.log(ws_send('bt:00010000000'));
        }
        i = x;
    }
    if (x > 0) {
        for (i; i < x; i++) {
            console.log(ws_send('bt:00000100000'));
        }
        i = x;
    }



 */


    /*
    if (x < 0){
        for (let i = 0; i > x; i++) {
            console.log(ws_send('bt:00010000000'));
        }
    }*/





//setTimeout(document.getElementById("stream").style.cursor="default", 5000);