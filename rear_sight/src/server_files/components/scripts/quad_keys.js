is_H_pressed = false;
is_C_pressed = false;
is_LeftSquareBracket_pressed = false;
is_RightSquareBracket_pressed = false;


function setKeydown(code) {
    if (code === "KeyH") {
        is_H_pressed = true;
        calibrationStartStop();
    }
    if (code === "KeyC") {
        is_C_pressed = true;
        steerToCenter();
    }
    if (code === "Space") {
        beepBeep(); 
    }
    if (code === "BracketLeft") {
        is_LeftSquareBracket_pressed = true;
    }
    if (code === "BracketRight") {
        is_RightSquareBracket_pressed = true;
    }
}


function setKeyup(code) {
    if (code === "KeyH") {
        is_H_pressed = false;
    }
    if (code === "KeyC") {
        is_C_pressed = false;
    }
    if (code === "Space") {
        beepBeepStop();
    }
    if (code === "BracketLeft") {
        is_LeftSquareBracket_pressed = false;
    }
    if (code === "BracketRight") {
        is_RightSquareBracket_pressed = false;
    }
}