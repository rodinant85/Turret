let DRAG_EVENTS_LOG = false;

let ERRORS_ARAEA_TEST_MODE = false;


/* Change color for stream */
let isNotActiveDragInterval = true;
let dragInterval;

function streamChangeColor() {
    console.log('changed');
    let ifx = document.getElementById("stream");
    //let br = document.getElementById("br");
    //let ct = document.getElementById("ct");
    let st = document.getElementById("st");

    /*ifx.style.webkitFilter = "brightness(" + br.value * 10 + "%) contrast(" + ct.value * 10 + "%) saturate(" + st.value * 10 + "%)";
    ifx.style.filter = "brightness(" + br.value * 10 + "%) contrast(" + ct.value * 10 + "%) saturate(" + st.value * 10 + "%)";*/
    /*brightness(" + br.value * 10 + "%) contrast(" + ct.value * 10 + "%)*/
    ifx.style.webkitFilter = "saturate(" + st.value * 10 + "%)";
    ifx.style.filter = " saturate(" + st.value * 10 + "%)";

    // its need to turn app
    /*
    ifx.style.webkitFilter = "brightness(" + br.value * 10 + "%)";
    ifx.style.filter = "brightness(" + br.value * 10 + "%)";
    ifx.style.webkitFilter = "contrast(" + ct.value * 10 + "%)";
    ifx.style.filter = "contrast(" + ct.value * 10 + "%)";
    ifx.style.webkitFilter  = "saturate(" + st.value * 10 + "%)";
    ifx.style.filter = " saturate(" + st.value * 10 + "%)";
    */
}



/* ////////// reg_button_fire */

let spaceIsPressed = false;

document.addEventListener('keydown', function(event) {
    if (event.ctrlKey && event.altKey) {
        let_buttonAltCtrlIsPressed();
        ws.send("{\"comm\":[\"FUSE_UP\"]}");
        document.getElementById("shootText").innerHTML="READY TO FIRE";
        document.getElementById("shootText").style.display="inline";
        document.addEventListener('keyup', function(event) {
            if (event.ctrlKey && event.altKey && event.code === 'Space') {
                // spaceIsPressed = true;
                ws.send("{\"comm\":[\"DO_FIRE:1\"]}");
                document.getElementById("shootText").innerHTML="FIRE!!!";
                document.getElementById("shootText").style.display="inline";

                if (typeof GAMEPAD_PRESENT == "undefined") {

                } else {
                    if (GAMEPAD_PRESENT) {
                        gamepadVibrate();
                    }
                }

                // document.getElementById("shootText").innerHTML="FIRE!!!";
                // document.getElementById("shootText").style.display="inline";
                // setTimeout(function () {
                //     document.getElementById("shootText").innerHTML="FIRE!!!";
                //     document.getElementById("shootText").style.display="inline";
                // }, 300);
                // setTimeout(function () {
                //     document.getElementById("shootText").innerHTML="";
                //     document.getElementById("shootText").style.display="none";
                // }, 5000);
            }

            // if (spaceIsPressed === true) {0
            //     let_buttonAltCtrlIsPressed();
            //     ws.send("{\"comm\":[\"DO_FIRE:3\"]}");
            //     spaceIsPressed = false;
            // }
        });
    }
});

document.addEventListener('keyup', function(event) {
    if (event.ctrlKey || event.altKey) {
        ws.send("{\"comm\":[\"FUSE_DOWN\"]}");
        document.getElementById("shootText").innerHTML="";
        document.getElementById("shootText").style.display="none";
    }
});



// $(document).keydown(function (e) {
//     if (e.which === 17 && 18 && 32) {
//         let_buttonAltCtrlIsPressed();
//         ws.send("{\"comm\":[\"DO_FIRE:3\"]}");
//     }
// })
//
// $(document).keydown(function (e) {
//     if (e.which === 17 && 18) {
//         let_buttonAltCtrlIsPressed();
//         ws.send("{\"comm\":[\"FUSE_UP\"]}");
//         document.getElementById("shootText").innerHTML="READY TO FIRE";
//         document.getElementById("shootText").style.display="inline";
//     }
// })
//
// $(document).keyup(function (e) {
//     if (e.which === 17 && 18) {
//         ws.send("{\"comm\":[\"FUSE_DOWN\"]}");
//         document.getElementById("shootText").innerHTML="";
//         document.getElementById("shootText").style.display="none";
//     }
// })





$(document).keydown(function (e) {
    if (e.which === 17 || e.which === 18) {
        ws.send("{\"comm\":[\"FUSE_R\"]}");
        $('#id_mode_button_fire').css({'background-color': 'rgba(236, 38, 103, 0.5)'});
        setTimeout(function () {
            $('#id_mode_button_fire').css({'background-color': 'rgba(76, 175, 80, 0.5)'});
        }, 1500);
    }

    if (e.code == "KeyH") {
        startStopColibration();
    }

    if (e.code == "KeyJ") {
        startBinding();
    }

    if (e.code == "KeyP") {
        //startTraining();
    }

    // For errors panel test. 
    // Will be deleted or removed.
    if (e.code =="KeyO") {
        if (ERRORS_ARAEA_TEST_MODE) {
            let t = Date.now();
            let data_with_error = '{"error": "' + t + ': Произошла страшная ошибка! Все пропало!!! <b>Совсем все!!!!</b>"}';
            getAllFromWS2Data(data_with_error);
            let data_with_info = '{"info": "' + t + ': Сейчас все в порядке."}';
            getAllFromWS2Data(data_with_info);
            let data_with_list = '{"system_info": {"CPU Temp": "30.18 C", "SD": "28G, 12.34%", "RAM": "3743M, 3.8%", "CPU": "1.5H, 0.0%", "Uptime": "0 hours : 0 min", "Last reboot": "04.07.22-13:07:55", "IP": "et 192.168.88.232"}}';
            getAllFromWS2Data(data_with_list);
        } else {
            errorsShow();
        }
    }

    return e;
});

$(document).keyup(function (e) {
    if (e.which === 17 || e.which === 18) {
        ws.send("{\"comm\":[\"FUSE_I\"]}");
    }
    return e;
})

/* ////////// Button Swap Streams */

let isRButtonNotDown = true;
$(document).keydown(function (e) {
    if (e.which == 82 && isRButtonNotDown) {
        isRButtonNotDown = false;
        onClickButtonSwapStreams();
    }
    return e;
});

$(document).keyup(function (e) {
    if (e.which == 82) {
        isRButtonNotDown = true;
        onClickButtonSwapStreams();
    }
    return e;
});


/*   Mouse Zoom   */
function onDownMouseWheel() {
    ws.send("bt:10000000000");
    //ws.send("{\"comm\":[\"GET_FCI_-1\"]}" );
}

function onUpMouseWheel() {
    ws.send("bt:00100000000");
    //ws.send("{\"comm\":[\"GET_FCI_+1\"]}" );
}


function streamZoom() {
    window.addEventListener('wheel', zoom)
    {
        function zoom(event) {
            // isFireCorrectionProcessed is in 'coordinates.js'
            if (isFireCorrectionProcessed) {
                console.log("impossible to do stream zooming when fire is processed");
                return;
            }
            event.deltaY < 0 ? onUpMouseWheel() : onDownMouseWheel();
        }
    }
    let scale = 1;
    const el = document.querySelector('div');
    window.onwheel = zoom;
}

function streamZoomOut() {
    window.addEventListener('wheel', zoom)
    {
        function zoom(event) {
        }
    }

    let scale = 1;
    const el = document.querySelector('div');
    window.onwheel = zoom;
}


/////// button shift + wheel






// });


//////  РИЧАГ .........
document.addEventListener('DOMContentLoaded', function() {

    let horizontalJoystick = document.getElementById("horizontalJoystick"),
        knob = document.getElementById("knob"),
        target_x = horizontalJoystick.clientWidth / 2 - knob.clientWidth / 2,
        target_y = horizontalJoystick.clientHeight / 2 - knob.clientHeight / 2;

    // var panSpan  = document.getElementById("panValue"),
    //     tiltSpan = document.getElementById("tiltValue");

    knob.style.webkitTransform = "translate(" + target_x + "px, " + target_y + "px)";

    // update the position attributes
    let target = document.getElementById("knob");
    updatePositionAttributes(target, target_x, target_y);

    // target elements with the "draggable" class
    interact('.draggable')
        .draggable({
            inertia: false,
            // keep the element within the area of its parent
            restrict: {
                restriction: "parent",
                endOnly: false,
                elementRect: {top: 0, left: 0, bottom: 1, right: 1}
            },
            onmove: dragMoveListener,
            onend: function (event) {
                if (DRAG_EVENTS_LOG) {
                    console.log("dragEnd");
                }

                var target = event.target;
                TweenLite.to(target, 0.2, {
                    ease: Back.easeOut.config(1.7),
                    "webkitTransform": "translate(" + target_x + "px, " + target_y + "px)"
                });
                updatePositionAttributes(target, target_x, target_y);
                clearInterval(dragInterval);
                dragInterval = 0;
                isNotActiveDragInterval = true;
                DX_2 = 0.0;
                // panSpan.innerHTML = 0;
                // tiltSpan.innerHTML = 0;
            }
        });

    function dragMoveListener(event) {
        if (DRAG_EVENTS_LOG) {
            console.log("dragMove: " + event.dx + ", " + event.dy);
        }

        let target = event.target,
            // keep the dragged position in the data-x/data-y attributes
            x = (parseFloat(target.getAttribute('data-x')) || 0) + event.dx,
            y = (parseFloat(target.getAttribute('data-y')) || 0) + event.dy;

        DY_2 = 0.0;

        if (282.5 - x < 0) {
            DX_2 = 0.05;
        } else {
            DX_2 = -0.05;
        }

        // translate the element
        target.style.webkitTransform = target.style.transform = 'translate(' + x + 'px, ' + y + 'px)';
        updatePositionAttributes(target, x, y);

        if (isNotActiveDragInterval) {
            dragInterval = setInterval(function () {
                let st = '{"comm":["MV_HLD:DX:' + DX_2 + ':DY:' + DY_2 + '"]}'; 
                ws.send(st);
                ws2.send(st); 
            }, 50);
            isNotActiveDragInterval = false;
        }

        // update text display
        // panSpan.innerHTML = (x-joystick.clientWidth/4);
        // tiltSpan.innerHTML = (y-joystick.clientHeight/4);
    }

    function updatePositionAttributes(element, x, y) {
        target.setAttribute('data-x', x);
        target.setAttribute('data-y', y);
    }

});

    //vertical

document.addEventListener('DOMContentLoaded', function(){

    let verticalJoystick = document.getElementById("verticalJoystick"),
        vknob = document.getElementById("vknob"),
        vtarget_x = verticalJoystick.clientWidth/2-vknob.clientWidth/2,
        vtarget_y = verticalJoystick.clientHeight/2-vknob.clientHeight/2;

    // var panSpan  = document.getElementById("panValue"),
    //     tiltSpan = document.getElementById("tiltValue");

    vknob.style.webkitTransform = "translate("+vtarget_x+"px, "+vtarget_y+"px)";

    // update the position attributes
    let vtarget = document.getElementById("vknob");
    vupdatePositionAttributes(vtarget,vtarget_x,vtarget_y);

    // target elements with the "draggable" class
    interact('.vdraggable')
        .draggable({
            inertia: false,
            // keep the element within the area of its parent
            restrict: {
                restriction: "parent",
                endOnly: false,
                elementRect: { top: 0, left: 0, bottom: 1, right: 1 }
            },
            onmove: vdragMoveListener,
            onend: function (vevent) {
                if (DRAG_EVENTS_LOG) {
                    console.log("dragEnd");
                }
                
                let vtarget = vevent.target;
                TweenLite.to(vtarget, 0.2, {ease: Back.easeOut.config(1.7), "webkitTransform":"translate("+vtarget_x+"px, "+vtarget_y+"px)"});
                vupdatePositionAttributes(vtarget,vtarget_x,vtarget_y);
                // panSpan.innerHTML = 0;
                // tiltSpan.innerHTML = 0;
                clearInterval(dragInterval);
                dragInterval = 0;
                isNotActiveDragInterval = true;
                DY_2 = 0.0;
            }
        });

    function vdragMoveListener (vevent) {
        if (DRAG_EVENTS_LOG) {
            console.log("vdragMove");
        }

        let vtarget = vevent.target,
            // keep the dragged position in the data-x/data-y attributes
            vx = (parseFloat(vtarget.getAttribute('data-x')) || 0) + vevent.dx,
            vy = (parseFloat(vtarget.getAttribute('data-y')) || 0) + vevent.dy;

        DX_2 = 0.0;

        if (282.5 - vy < 0) {
            DY_2 = 0.05;
        } else {
            DY_2 = -0.05;
        }

        // translate the element
        vtarget.style.webkitTransform = vtarget.style.transform = 'translate(' + vx + 'px, ' + vy + 'px)';
        vupdatePositionAttributes(vtarget,vx,vy);

        if (isNotActiveDragInterval) {
            dragInterval = setInterval(function () {
                let st = '{"comm":["MV_HLD:DX:' + DX_2 + ':DY:' + DY_2 + '"]}'; 
                ws.send(st);
                ws2.send(st);
            }, 50);
            isNotActiveDragInterval = false;
        }


        // update text display
        // panSpan.innerHTML = (x-joystick.clientWidth/4);
        // tiltSpan.innerHTML = (y-joystick.clientHeight/4);
    }

    function vupdatePositionAttributes(element,vx,vy){
        vtarget.setAttribute('data-x', vx);
        vtarget.setAttribute('data-y', vy);
    }

});


let scrollableElement = document.getElementById("scrollableElement");
scrollableElement.addEventListener("onmouseover", streamZoom, false);
scrollableElement.addEventListener("onmouseover", mouseOnDistInfo, false);
scrollableElement.addEventListener("onmouseleave", streamZoomOut, false);
scrollableElement.addEventListener("onmouseleave", clearInterval(mouseMovingInterval), false);
scrollableElement.addEventListener("onmouseleave", function() {
    DX_2=0.0; 
    DY_2=0.0;
}, false);

