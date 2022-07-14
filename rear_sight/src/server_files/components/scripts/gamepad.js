const GAMEPAD_PRESENT = true;
const GAMEPAD_THRESHOLD = 0.1;
const SPEEDUP_RATE_MIN = 0.3;
const SPEEDUP_RATE_MAX = 2.5;
const SPEEDUP_RATE_STEP = 0.25;
const SPEEDUP_IGNORE_ON_X = true;
const SPEEDUP_IGNORE_ON_Y = true;

var gamepad_axe_x = 0.0;
var gamepad_axe_y = 0.0;
var speed_up_rate = SPEEDUP_RATE_MIN;
var speed_down_rate = 0.1;

var is_gp_ctrl_pressed = false;
var is_gp_alt_pressed = false;
var is_gp_space_pressed = false;
var is_gp_zoom_in_pressed = false;
var is_gp_zoom_out_pressed = false;
var is_gp_tracker_stop_pressed = false;
var is_gp_measuring_plus_pressed = false;
var is_gp_measuring_minus_pressed = false;
var is_gp_set_distance_pressed = false;

var gamepad_simulate_key_repeat_interval = 0;

var gamepads = [];


function toLogarithm(x, p, t) {
    // p: start shift part
    // within the limits of [0.0, 1.0]

    // t: slope of the logarithm
    //let t = 100;

    // fixed point No 0
    let x0 = 0;
    let y0 = 0;

    // fixed point No 1
    let x1 = 3;
    let y1 = 3;

    if (x > 0) {
        let tr_x =  Math.log((x - x0)/(x1 - x0)*(Math.E - 1) + 1) / Math.log(t) * (y1 - y0) + y0;
        let r = p * y1 + tr_x * (1.0 - p);
        return r;
    } else if (x == 0) {
        return 0;
    } else if (x < 0) {
        x = -x;
        let tr_x =  Math.log((x - x0)/(x1 - x0)*(Math.E - 1) + 1) / Math.log(t) * (y1 - y0) + y0;
        let r = p * y1 + tr_x * (1.0 - p);
        return -r;
    }
}


function gamepadVibrate() {
    const gamepads = navigator.getGamepads();
    if (gamepads.length > 0 && gamepads[0] != null) {
        let gp = gamepads[0];
        gp.vibrationActuator.playEffect("dual-rumble", {
            startDelay: 0,
            duration: 1000,
            weakMagnitude: 1.0,
            strongMagnitude: 1.0
        });
    }
}


$(function begin() {
    window.addEventListener('gamepadconnected', gamepadConnectedHandler);
    window.addEventListener("gamepaddisconnected", handleDisconnect);
});

function gamepadConnectedHandler(e) {
    //console.log("x,y: " + e.gamepad.axes[0] + ", " + e.gamepad.axes[1]); 
    console.log("Gamepad connected.");
    gamepads[0] = e.gamepad;
}

function handleDisconnect(e) {
    console.log("Gamepad disconnected.");
}


function addNewPads() {
    if (typeof window.navigator.getGamepads === "function") {
        gamepads = navigator.getGamepads();
        if (gamepads.length > 0 && gamepads[0] != null) {
            //console.log("x,y: " + gamepads[0].axes[0] + ", " + gamepads[0].axes[1]); 
            //console.log("x2,y2: " + gamepads[2].axes[0] + ", " + gamepads[3].axes[1]);
            let gp = gamepads[0];

            let x0 = gp.axes[0];
            let y0 = gp.axes[1];
            let x = gp.axes[2];
            let y = gp.axes[3];
            let is_speed_up = gp.buttons[4].pressed;
            let gp_ctrl = gp.buttons[14].pressed;
            let gp_alt = gp.buttons[13].pressed;
            let gp_space = gp.buttons[6].pressed;
            let gp_zoom_in = gp.buttons[3].pressed;
            let gp_zoom_out = gp.buttons[0].pressed;
            let gp_tracker_stop = gp.buttons[1].pressed;
            var gp_measuring_plus = gp.buttons[5].pressed;
            var gp_measuring_minus = gp.buttons[7].pressed;
            var gp_set_distance = gp.buttons[2].pressed;

            let wy = y;
            if (typeof IS_QUAD_BIKE != 'undefined') {
                if (IS_QUAD_BIKE) {
                    // redefine OY axis (trottle)
                    wy = y0;
                }
            }

            if (Math.abs(x) > GAMEPAD_THRESHOLD || Math.abs(wy) >GAMEPAD_THRESHOLD)
            {
                if (is_speed_up) {
                    if (speed_up_rate < SPEEDUP_RATE_MAX)
                    {
                        speed_up_rate += SPEEDUP_RATE_STEP;
                    }

                    if (SPEEDUP_IGNORE_ON_X) {
                        gamepad_axe_x = x * SPEEDUP_RATE_MAX;
                    } else {
                        gamepad_axe_x = x * speed_up_rate;
                    }                    

                    if (SPEEDUP_IGNORE_ON_Y) {
                        gamepad_axe_y = wy * SPEEDUP_RATE_MAX;
                    } else {
                        gamepad_axe_y = wy * speed_up_rate;
                    }
                } else {
                    gamepad_axe_x = x * speed_down_rate;
                    gamepad_axe_y = wy * speed_down_rate;
                    speed_up_rate = SPEEDUP_RATE_MIN;
                }
            } else {
                gamepad_axe_x = 0.0;
                gamepad_axe_y = 0.0;
                speed_up_rate = SPEEDUP_RATE_MIN;
            }

            gamepad_axe_x = toLogarithm(gamepad_axe_x, 0.0, 100);
            gamepad_axe_y = toLogarithm(gamepad_axe_y, 0.65, 3);

            //if (gp_zoom_in != is_gp_zoom_in_pressed) 
            {
                is_gp_zoom_in_pressed = gp_zoom_in;
                if (is_gp_zoom_in_pressed) {
                    simulateEDown();
                } else {
                    simulateEUp();
                }            
            }
            
            //if (gp_zoom_out != is_gp_zoom_out_pressed) 
            {
                is_gp_zoom_out_pressed = gp_zoom_out;
                if (is_gp_zoom_out_pressed) {
                    simulateQDown();
                } else {
                    simulateQUp();
                }            
            }

            if (gp_ctrl != is_gp_ctrl_pressed) {
                is_gp_ctrl_pressed = gp_ctrl;
                if (is_gp_ctrl_pressed) {
                    simulateCtrlDown();                
                } else {
                    simulateCtrlUp();
                }
            }

            if (gp_alt != is_gp_alt_pressed) {
                is_gp_alt_pressed = gp_alt;
                if (is_gp_alt_pressed) {
                    simulateAltlDown();
                } else {
                    simulateAltlUp();
                }
            }

            if (gp_space != is_gp_space_pressed) {
                is_gp_space_pressed = gp_space;
                if (is_gp_space_pressed) {
                    simulateSpacelDown();
                } else {
                    simulateSpacelUp();
                }
            }

            if (gp_measuring_minus != is_gp_measuring_minus_pressed) {
                is_gp_measuring_minus_pressed = gp_measuring_minus;
                if (is_gp_measuring_minus_pressed) {
                    simulateBracketRightDown();
                    gamepad_simulate_key_repeat_interval = setInterval(simulateKeyRepeat, 30);
                } else {
                    simulateBracketRightUp();
                    clearInterval(gamepad_simulate_key_repeat_interval);
                    gamepad_simulate_key_repeat_interval = 0;
                }
            }

            if (gp_measuring_plus != is_gp_measuring_plus_pressed) {
                is_gp_measuring_plus_pressed = gp_measuring_plus;
                if (is_gp_measuring_plus_pressed) {
                    simulateBracketLeftDown();
                    gamepad_simulate_key_repeat_interval = setInterval(simulateKeyRepeat, 30);
                } else {
                    simulateBracketLeftUp();
                    clearInterval(gamepad_simulate_key_repeat_interval);
                    gamepad_simulate_key_repeat_interval = 0;
                }
            }

            if (gp_set_distance != is_gp_set_distance_pressed) {
                is_gp_set_distance_pressed = gp_set_distance;
                if (is_gp_set_distance_pressed) {
                    simulateEqualDown();
                } else {
                    simulateEqualUp();
                }
            }

            is_gp_tracker_stop_pressed = gp_tracker_stop;
            if (is_gp_tracker_stop_pressed) {
                trackerStop();
            }
        }
    } else {
        // console.log("window.navigator.getGamepads still not a function!");
    }
}

function simulateEDown() {
    simulateKeyEvent("keydown", 0, "KeyE", "e", 69, 69);
}

function simulateEUp() {
    simulateKeyEvent("keyup", 0, "KeyE", "e", 69, 69);
}

function simulateQDown() {
    simulateKeyEvent("keydown", 0, "KeyQ", "q", 81, 81);
}

function simulateQUp() {
    simulateKeyEvent("keyup", 0, "KeyQ", "q", 81, 81);
}

function simulateCtrlDown() {
    simulateKeyEvent("keydown", 0, "ControlLeft", "Control", 17, 17);
}

function simulateCtrlUp() {
    simulateKeyEvent("keyup", 0, "ControlLeft", "Control", 17, 17);
}

function simulateAltlDown() {
    simulateKeyEvent("keydown", 0, "AltLeft", "Alt", 18, 18);
}

function simulateAltlUp() {
    simulateKeyEvent("keyup", 0, "AltLeft", "Alt", 18, 18);

}

function simulateSpacelDown() {
    simulateKeyEvent("keydown", 0, "Space", " ", 32, 32);
}

function simulateSpacelUp() {
    simulateKeyEvent("keyup", 0, "Space", " ", 32, 32);

}

function simulateBracketLeftDown() {
    simulateKeyEvent("keydown", 0, "BracketLeft", "[", 219, 219);
}

function simulateBracketLeftUp() {
    simulateKeyEvent("keyup", 0, "BracketLeft", "[", 219, 219);
}

function simulateBracketRightDown() {
    simulateKeyEvent("keydown", 0, "BracketRight", "]", 221, 221);
}

function simulateBracketRightUp() {
    simulateKeyEvent("keyup", 0, "BracketRight", "]", 221, 221);
}

function simulateEqualDown() {
    simulateKeyEvent("keydown", 0, "Equal", "=", 187, 187);
}

function simulateEqualUp() {
    simulateKeyEvent("keyup", 0, "Equal", "=", 187, 187);
}


function simulateKeyEvent(eventType, charCode, code, key, keyCode, which) {
    window.document.body.dispatchEvent(new KeyboardEvent(eventType, 
        {
            'isTrusted': true,
            'altKey': is_gp_alt_pressed, 
            'bubbles': true,
            'cancelBubble': false,
            'cancelable': true,
            'charCode': charCode,
            'code': code,
            'composed': true,
            'ctrlKey': is_gp_ctrl_pressed,
            'currentTarget': document,
            'defaultPrevented': false,
            'detail': 0,
            'eventPhase': 3,
            'isComposing': false,
            'key': key,
            'keyCode': keyCode,
            'location': 1,
            'metaKey': false,
            'repeat': false,
            'returnValue': true,
            'shiftKey': false,
            'type': eventType,  
            'which': which
        }
    ));
}

/*
'srcElement': body,
'target': body,

'view': Window,

charCode: 0
code: "ControlLeft"
composed: true
ctrlKey: true
currentTarget: document
defaultPrevented: false
detail: 0
eventPhase: 3
isComposing: false
key: "Control"
keyCode: 17
location: 1
metaKey: false
repeat: false
returnValue: true
shiftKey: false
srcElement: body
target: body
type: "keydown"
view: Window
which: 17
*/


function process() {
    addNewPads();
    requestAnimationFrame(process);
}
requestAnimationFrame(process);


function simulateKeyRepeat() {
    if (is_gp_measuring_minus_pressed) {
        simulateBracketRightDown();
    }

    if (is_gp_measuring_plus_pressed) {
        simulateBracketLeftDown();
    }
}