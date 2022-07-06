let MEASURING_ZOOM_MIN = 0.5;
let MEASURING_ZOOM_MAX = 20;
let MEASURING_ZOOM_DEFAULT = 5;
let MEASURING_ZOOM_STEP = 0.05;
let measuring_zoom = MEASURING_ZOOM_DEFAULT;
let is_measuring = false;
let measuring_interval = 0; 

let full_screen_width_in_mrad;
let full_screen_height_in_mrad;


function processMeasuring() {
    if (!(isRightSquareBracket_ButtonPushed || isLeftSquareBracket_ButtonPushed)) {
        return;
    }

    measuringRoutine();
}


function measuringRoutine() {
    is_measuring = true;
    resetMeasuringInterval();
    measuringImageShow();

    if (isRightSquareBracket_ButtonPushed) {
        measuring_zoom += MEASURING_ZOOM_STEP;
        if (measuring_zoom > MEASURING_ZOOM_MAX) {
            measuring_zoom = MEASURING_ZOOM_MAX;
        }
    } else if (isLeftSquareBracket_ButtonPushed) {
        measuring_zoom -= MEASURING_ZOOM_STEP;
        if (measuring_zoom < MEASURING_ZOOM_MIN) {
            measuring_zoom = MEASURING_ZOOM_MIN;
        }
    } 

    measuringZoomCSSSet(measuring_zoom);

    rvs = recalculateRealVideoSize();

    let square_image_zoomed_width_px = SQUARE_IMAGE_WIDTH_PX * measuring_zoom;

    let square_image_width_part = square_image_zoomed_width_px / rvs.w;

    if (!isStreamsSwapped) {
        square_image_width_part *= video_zoom;
    }

    let square_image_width_in_mrad = full_screen_width_in_mrad * square_image_width_part;
    let dist = 1000 / square_image_width_in_mrad;


    let st = "" + Math.floor(square_image_width_in_mrad * 10) / 10;
    let st2 = "" + Math.floor(dist);

    let text = "" + st + " mrad = 1m at " + st2 + "m";

    document.getElementById("measuring_label").innerText = text;
}


function measuringOnChanges() {
    if (is_measuring) {
        measuringRoutine();
    }
}


function resetMeasuringInterval() {
    if (measuring_interval != 0) {
        clearInterval(measuring_interval);
        measuring_interval = 0;
    }

    measuring_interval = setInterval(measuringTimeoutHandler, 3000);
}


function measuringTimeoutHandler() {
    clearInterval(measuring_interval);
    measuring_interval = 0;
    is_measuring = false;
    measuringImageHide();
}


function measuringImageShow() {
    document.getElementById("measuring_div").classList.remove("hidden-class");  
}


function measuringImageHide() {
    document.getElementById("measuring_div").classList.add("hidden-class");  
}


function measuringZoomCSSSet(zoom) {
    oul = document.getElementsByClassName("outline")[0];
    oul.style.setProperty("--square-zoom", zoom);
}


$(document).ready(measuringPrepear);

function measuringPrepear() {
    let w_rad = Math.atan( SCREEN_WIDTH_MM / SCREEN_DISTANCE_MM );
    let h_rad = Math.atan( SCREEN_HEIGHT_MM / SCREEN_DISTANCE_MM );

    let w_mrad = w_rad * 6 / (2 * Math.PI) * 1000;
    let h_mrad = h_rad * 6 / (2 * Math.PI) * 1000; 

    full_screen_width_in_mrad = w_mrad;
    full_screen_height_in_mrad = h_mrad;
}