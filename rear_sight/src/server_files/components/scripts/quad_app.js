let WS2_MESSAGES_LOG = false;

// Second WebSocket - for motors ant and switches
let ws2;

// Absolute motors position. [step].
let x_position_steps = 0;
let y_position_steps = 0;

// Limit switches.
let x_limit_min_flag = false;
let x_limit_max_flag = false;
let y_limit_min_flag = false;
let y_limit_max_flag = false;

let is_calibration_done = false;


$(function begin() {
    document.addEventListener('keydown', (e) => {
        setKeydown(e.code);
        processMeasuring();
    });

    document.addEventListener('keyup', (e) => {
        setKeyup(e.code);
    });

    playVideo();

    setTimeout(connectSecondSocket, 1000);

    setTimeout(sendIntervalHandler, 50);

    loadCookies();
});


function loadCookies() {
    let is_cookies_load_ok = true;

    x_min_in_steps = +getCookie("x_min_in_steps");
    if (typeof x_min_in_steps == "undefined" || isNaN(x_min_in_steps)) {
        x_min_in_steps = default_x_min_in_steps;
        is_cookies_load_ok = false;
    }
    x_max_in_steps = +getCookie("x_max_in_steps");
    if (typeof x_max_in_steps == "undefined" || isNaN(x_max_in_steps)) {
        x_max_in_steps = default_x_max_in_steps;
        is_cookies_load_ok = false;
    }
    x_center_in_steps = +getCookie("x_center_in_steps");
    if (typeof x_center_in_steps == "undefined" || isNaN(x_center_in_steps)) {
        x_center_in_steps = default_x_center_in_steps;
        is_cookies_load_ok = false;
    }

    if (is_cookies_load_ok) {
        is_calibration_done = true;
    }
}


function playVideo() {
    let vidstream = document.getElementById("stream");
    let config = {'iceServers': []};

    let orig = window.location.origin;
    let sa = orig.split(':') ;
    let videoOrigin = sa[0] + ':' + sa[1] + ':8080/?action=stream';
    vidstream.setAttribute("src", videoOrigin);
    console.log("Video src: " + videoOrigin);

    // playStream(vidstream, config, function (errmsg) {
    //     console.error(errmsg);
    // });
}


function connectSecondSocket() {
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
        getAllFromWS2Data(event.data);
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


function getAllFromWS2Data(mess) {
    try {
        const r = JSON.parse(mess);

        // mess example
        // {"error":"Fatal error on line 48!"}
        // if (typeof r["error"] != "undefined") {
        //     errorTextOut(r["error"]);
        //     return;
        // }

        // if (typeof r["info"] != "undefined") {
        //     errorMessageAdd(r["info"]);
        // }

        // if (typeof r["system_info"] != "undefined") {
        //     errorMessageAddList(r["system_info"]);
        // }

        // if (typeof r["need_reload"] != "undefined") {
        //     reload();
        // }

        // mess example
        // {"resp":{"STP_X":-3509,"STP_Y":955,"SW_X_MAX":0,"SW_X_MIN":0,"SW_Y_MAX":0,"SW_Y_MIN":0}}
        if (typeof r["resp"] != "undefined") {
            x_position_steps = r.resp.STP_X;
            y_position_steps = r.resp.STP_Y;

            x_limit_max_flag = (r.resp.SW_X_MAX != 0);
            x_limit_min_flag = (r.resp.SW_X_MIN != 0);
            y_limit_max_flag = (r.resp.SW_Y_MAX != 0);
            y_limit_min_flag = (r.resp.SW_Y_MIN != 0);

            updateGUI();            
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


function updateGUI() {
    // limits
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

    // ox
    let min_val = 0;
    let max_val = 100;
    
    if (is_calibration_done) {
        let v = 100 * (x_position_steps - x_min_in_steps) / (x_max_in_steps - x_min_in_steps);
        if (v < min_val) {
            v = min_val;
        }
        if (v > max_val) {
            v = max_val;
        }
        document.getElementById("range_input").value = v;
    }
}


function sendIntervalHandler() {
    //clearInterval(stopInterval);
    //stopInterval = 0;
    try {
        let st = '{"comm":["MV_HLD:DX:0:DY:0"]}';

        if (calibration_ox_started) {
            // Calibrationn OX case
            let dx = calibrationOXRoutine();

            st = '{"comm":["MV_HLD:DX:' + dx + ':DY:' + 0 + '"]}';
        } else if (typeof(traker_target) != "undefined") {
            // Tracker case
            let dx = traker_target["t_dx"];
            let dy = traker_target["t_dy"];

            dx = Math.floor(dx * 1000) / 1000;
            dy = Math.floor(dy * 1000) / 1000;

            st = '{"comm":["MV_HLD:DX:' + dx + ':DY:' + dy + '"]}'; 

            traker_target = undefined;
        } else if (typeof(GAMEPAD_PRESENT) != "undefined") {
            // gamepad case
            if (GAMEPAD_PRESENT) { 
                let dx = gamepad_axe_x;
                let dy = gamepad_axe_y;
                if (is_steering_to_center) {
                    dx = calcStearingToCenterSpeed();
                    if (Math.abs(gamepad_axe_x) > GAMEPAD_THRESHOLD) {
                        // gamepad steering halts centering
                        is_steering_to_center = false;
                    }
                }
                if (is_beep) {
                    dx = calcBeepSpeed();
                }
                st = '{"comm":["MV_HLD:DX:' + dx + ':DY:' + dy + '"]}';
            }
        } else if (typeof(GAMEPAD_PRESENT) == "undefined") {
            // gamepad disconnected
            if (is_steering_to_center) {
                let dx = calcStearingToCenterSpeed();
                let dy = 0;
                st = '{"comm":["MV_HLD:DX:' + dx + ':DY:' + dy + '"]}';
            }
        } else if (typeof IS_SEND_CONFIGS_NEEDED != "undefined") {
            // Config sending case 
            if (IS_SEND_CONFIGS_NEEDED && !is_configs_sending_complete) {
                let xmf = OX_STEPPER_MAX_STEPS_FREQUENCEY;
                let ymf = OY_STEPPER_MAX_STEPS_FREQUENCEY;

                st = '{"config":{"OX_MAX_FREQ":' + xmf + ';"OY_MAX_FREQ":' + ymf + '}}'; 
            }
        }

        //ws_send(st);
        ws2.send(st);
        
    
    } catch (error) {
        
    }
    setTimeout(sendIntervalHandler, 50);
}


let centering_tolerance_steps = 50;
let centering_speed = 0.5; // 2.0;
let centering_precision_tolerance_steps = 10;
let centering_precision_speed = 0.125; // 0.5;


function calcStearingToCenterSpeed() {
    let ret_s = 0;
    let d_in_steps = x_center_in_steps - x_position_steps;

    // if ox encoder inverted then center is at the other side
    let ox_inverted = false;
    if (x_max_in_steps < x_min_in_steps) {
        ox_inverted = true;
    }

    if (Math.abs(d_in_steps) > centering_tolerance_steps) {
        ret_s = centering_speed;
        if (ox_inverted) {
            ret_s = -ret_s;
        }
        if (d_in_steps < 0) {
            ret_s = -ret_s;
        }
    } else if (Math.abs(d_in_steps) > centering_precision_tolerance_steps) {
        ret_s = centering_precision_speed;
        if (ox_inverted) {
            ret_s = -ret_s;
        }
        if (d_in_steps < 0) {
            ret_s = -ret_s;
        }
    } else {
        is_steering_to_center = false;
    }

    return ret_s;
}

// ----------------------------------------------------------------------------------------------------------------------------
// Calibration

let calibration_ox_started = false;
let calibration_ox_stage = 0;

const CALIBRATION_OX_STAGE_GO_INSIDE = 0;
const CALIBRATION_OX_STAGE_GO_MIN = 1;
const CALIBRATION_OX_STAGE_GO_MAX = 2;
const CALIBRATION_OX_STAGE_CALCS = 3;
const CALIBRATION_OX_STAGE_GO_CENTER = 4;

let calibration_ox_speed = 0.5;

let x_min_in_steps;
let x_max_in_steps;
let x_center_in_steps;
let default_x_min_in_steps = 0;
let default_x_max_in_steps = 0;
let default_x_center_in_steps = 0;


function calibrationStartStop() {
    if (!calibration_ox_started) {
        calibration_ox_stage = 0;
        calibration_ox_started = true;
    } else if (calibration_ox_started) {
        calibration_ox_stage = 0;
        calibration_ox_started = false;
    }
}


function calibrationOXRoutine() {
    let dx = 0;

    console.log("calibration_ox_stage: " + calibration_ox_stage);

    if (calibration_ox_stage == CALIBRATION_OX_STAGE_GO_INSIDE)
    {
        if (x_limit_max_flag) {
            //go left
            dx = -calibration_ox_speed;
            return dx;
        }
        if (x_limit_min_flag) {
            //go right
            dx = calibration_ox_speed;
            return dx;
        }
        calibration_ox_stage = CALIBRATION_OX_STAGE_GO_MIN;
        return dx;
    }

    if (calibration_ox_stage == CALIBRATION_OX_STAGE_GO_MIN) {
        if (!x_limit_min_flag) {
            //go left
            dx = -calibration_ox_speed;
            return dx;
        }

        // remember minimum
        x_min_in_steps = x_position_steps;

        calibration_ox_stage = CALIBRATION_OX_STAGE_GO_MAX;
        return dx;
    }

    if (calibration_ox_stage == CALIBRATION_OX_STAGE_GO_MAX) {
        if (!x_limit_max_flag) {
            //go right
            dx = calibration_ox_speed;
            return dx;
        }

        // remember maximum
        x_max_in_steps = x_position_steps;

        calibration_ox_stage = CALIBRATION_OX_STAGE_CALCS;
        return dx;
    }

    if (calibration_ox_stage == CALIBRATION_OX_STAGE_CALCS) {
        // do claibration ox calcs
        
        // calc ox min, max, center and save it in cookies
        x_center_in_steps = Math.floor((x_max_in_steps + x_min_in_steps) / 2);
        setCookie("x_min_in_steps", x_min_in_steps, {'max-age': COOKI_KEEP_7_DEYS});
        setCookie("x_max_in_steps", x_max_in_steps, {'max-age': COOKI_KEEP_7_DEYS});
        setCookie("x_center_in_steps", x_center_in_steps, {'max-age': COOKI_KEEP_7_DEYS});

        is_calibration_done = true;

        calibration_ox_stage = CALIBRATION_OX_STAGE_GO_CENTER;
        return dx;
    }

    if (calibration_ox_stage == CALIBRATION_OX_STAGE_GO_CENTER) {
        steerToCenter();

        // stop calibration
        calibration_ox_started = false;
        return dx;
    }

    return dx;
}


// ----------------------------------------------------------------------------------------------------------------------------


let is_steering_to_center = false;

function steerToCenter() {
    if (is_calibration_done) {
        is_steering_to_center = true;
    }
}


let is_beep = false;
let beep_steps_at_start = 0;

function beepBeep() {
    is_beep = true;
    beep_steps_at_start = x_position_steps;
}

function beepBeepStop() {
    is_beep = false;
    
    if (is_calibration_done) {
        // fix calibration data
        let beep_steps_delta = x_position_steps - beep_steps_at_start;
        x_min_in_steps += beep_steps_delta;
        setCookie("x_min_in_steps", x_min_in_steps, {'max-age': COOKI_KEEP_7_DEYS});
        x_max_in_steps += beep_steps_delta;
        setCookie("x_max_in_steps", x_max_in_steps, {'max-age': COOKI_KEEP_7_DEYS});
        x_center_in_steps += beep_steps_delta;
        setCookie("x_center_in_steps", x_center_in_steps, {'max-age': COOKI_KEEP_7_DEYS});
    }
}


let beep_speed = 8.0;
let beep_sign = 1;

function calcBeepSpeed() {
    beep_sign = -beep_sign;
    return beep_speed * beep_sign;
}


// Assume that the video size is maximized and without crop.
function recalculateRealVideoSize() {
    let ret_obj = {};
    let stream = document.getElementById("stream");

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