var table;
var tr;
let currentHolderPositionX;
let currentHolderPositionY;
const scaleShift = -15;

let bind_x_correction = 0.0;
let bind_y_correction = 0.0;

setTimeout(function (){
    setInterval(function (){
        table=[];
        tr=$('tr');
        for (var i=0; i<tr.length; i++) {
            table[i]=tr.eq(i).children().eq(0).text();
        }
    }, 300)


    // document.getElementById('value1').textContent=text[index-2];
    // document.getElementById('value2').textContent=text[index-1];
    // document.getElementById('value3').textContent=text[index];
    // document.getElementById('value4').textContent=text[index+1];
    // document.getElementById('value5').textContent=text[index+2];
    // document.getElementById('value6').textContent=text[index+3];
    // document.getElementById('value7').textContent=text[index+4];
    // tr[index+1].cells[1].innerText


    // document.getElementById('value1').textContent=tr[index-2].cells[1].innerText;
    // document.getElementById('value2').textContent=tr[index-1].cells[1].innerText;
    // document.getElementById('value3').textContent=tr[index].cells[1].innerText;
    // document.getElementById('value4').textContent=tr[index+1].cells[1].innerText;
    // document.getElementById('value5').textContent=tr[index+2].cells[1].innerText;
    // document.getElementById('value6').textContent=tr[index+3].cells[1].innerText;
    // document.getElementById('value7').textContent=tr[index+4].cells[1].innerText;

    currentHolderPositionX = 0;
    currentHolderPositionY = 0;

    var rulezH = document.getElementById("rulerH");
    var rulezH = new Rulez({
        element: rulerH,
        // layout: 'horizontal',
        divisionDefaults: {
            strokeWidth: .2,
        },
        textDefaults: {
            rotation: 0,
            offset: 3,
            className: 'rulez-text',
            centerText: true,

        },
        divisions: [
            {
                className: 'redz',
                pixelGap: 1,
                lineLength: 1
            },
            {
                className: 'greenz',
                pixelGap: 5,
                lineLength: 1.5
            }
        ],
        texts: [
            {
                className: 'whitez',
                pixelGap: 5
            },
        ],
        // textDefaults:{
        //     rotation: 90
        // }
    });
    rulezH.render();
    // document.getElementById('jumpToButton').addEventListener('click', function() {
        setInterval(function () {
            // [currentHolderPositionX] = [1 / 10 * rad] = [100 * millirad]
            // In our case, the full circle is 6 rad.
            //currentHolderPositionX -= (DX_2 / 6.0); 
            currentHolderPositionX = x_angle_position_degree / 6.0;
            //currentHolderPositionX = trim2Digits(currentHolderPositionX);

            currentHolderPositionX += bind_x_correction;

            while (currentHolderPositionX < 0.0) {
                currentHolderPositionX += 60.0;
            }
            while (currentHolderPositionX > 60.0) {
                currentHolderPositionX -= 60.0;
            }

            var horCoordM = Math.floor(currentHolderPositionX);
            var horCoordMRAD = Math.abs(Math.floor((currentHolderPositionX - horCoordM) * 100));

            var horMLine = horCoordM < 10 ? "0" : "";
            var horMRADLine = horCoordMRAD < 10 ? "0" : "";
            document.getElementById('horizontalCoordinates').textContent = horMLine + horCoordM + "–" + horMRADLine + horCoordMRAD;

            rulezH.scrollTo(horCoordMRAD + scaleShift);
            },100)
    // });

    var rulezV = document.getElementById("rulerV");
    var rulezV = new Rulez({
        element: rulerV,
        layout: 'vertical',
        divisionDefaults: {
            strokeWidth: .2,
        },
        textDefaults: {
            rotation: 0,
            offset: 3,
            className: 'rulez-text',
            centerText: true,

        },
        divisions: [
            {
                className: 'redz',
                pixelGap: 1,
                lineLength: 1
            },
            {
                className: 'greenz',
                pixelGap: 10,
                lineLength: 1.5
            }
        ],
        texts: [
            {
                className: 'whitez',
                pixelGap: 5
            },
        ],
        // textDefaults:{
        //     rotation: 90
        // }
    });
    rulezV.render();
    // document.getElementById('jumpToButton').addEventListener('click', function() {
    setInterval(function (){
        //currentHolderPositionY -= (0.75 * DY_2 / 6.0);
        currentHolderPositionY = y_angle_position_degree / 6.0;
        //currentHolderPositionY = trim2Digits(currentHolderPositionY);

        currentHolderPositionY += bind_y_correction;

        while (currentHolderPositionY < 0.0) {
            currentHolderPositionY += 60.0;
        }
        while (currentHolderPositionY > 60.0) {
            currentHolderPositionY -= 60.0;
        }

        var verCoordM = Math.floor(currentHolderPositionY);
        var verCoordMRAD = Math.abs(Math.floor((currentHolderPositionY - verCoordM) * 100));

        var verMLine = verCoordM < 10 ? "0" : "";
        var verMRADLine = verCoordMRAD < 10 ? "0" : "";
        document.getElementById('verticalCoordinates').textContent = verMLine + verCoordM + "–" + verMRADLine + verCoordMRAD;

        rulezV.scrollTo(verCoordMRAD + scaleShift);
        },100)



    // if(document.getElementById('value3').textContent === 'V') {
    //     document.getElementById('value3').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&emsp;&emsp;&emsp;&emsp;&emsp;';
    // }
    // if(document.getElementById('value2').textContent === 'V') {
    //     document.getElementById('value2').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&emsp;&emsp;&emsp;&emsp;&emsp;';
    // }
    // if(document.getElementById('value1').textContent === 'V') {
    //     document.getElementById('value1').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;';
    // }
    // if(document.getElementById('value5').textContent === '') {
    //     document.getElementById('value5').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;';
    // }
    // if(document.getElementById('value6').textContent === '') {
    //     document.getElementById('value6').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;';
    // }
    // if(document.getElementById('value7').textContent === '') {
    //     document.getElementById('value7').innerHTML='&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&emsp;&emsp;&emsp;';
    // }
},500)


function trim2Digits(num) {
    return Math.trunc(num * 100.0) / 100.0;
}


function bindPointToXY(bind_x, bind_y) {
    bind_x_correction = bind_x / 100 - (currentHolderPositionX - bind_x_correction);
    bind_y_correction = bind_y / 100 - (currentHolderPositionY - bind_y_correction);
    
    bind_x_correction = trim2Digits(bind_x_correction);
    bind_y_correction = trim2Digits(bind_y_correction);

    setCookie("bind_x_correction", bind_x_correction, {'max-age': COOKI_KEEP_7_DEYS});
    setCookie("bind_y_correction", bind_y_correction, {'max-age': COOKI_KEEP_7_DEYS});
}

function bindPointReset() {
    bind_x_correction = 0.0;
    bind_y_correction = 0.0;

    setCookie("bind_x_correction", bind_x_correction, {'max-age': COOKI_KEEP_7_DEYS});
    setCookie("bind_y_correction", bind_y_correction, {'max-age': COOKI_KEEP_7_DEYS});
}



