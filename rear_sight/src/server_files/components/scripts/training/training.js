let IS_TREINING_AVAILABLE = true;

let is_training = false;


function trainingStart() {
    if (is_training) {
        is_training = false;
        trainingImageHide();
        return;
    }

    is_training = true;
    trainingImageShow();
}


function trainingImageShow() {
    document.getElementById("training_div").classList.remove("hidden-class");  
}


function trainingImageHide() {
    document.getElementById("training_div").classList.add("hidden-class");  
}