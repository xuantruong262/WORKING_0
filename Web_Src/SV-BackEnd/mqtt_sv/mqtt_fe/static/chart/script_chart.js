// PH CHART
// const ph_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
// var arr =[];
// for(let i = 0;i<24;i++)
// {
//     if(i==0)
//     {
//         arr[0] = 4;
//     }
//     else
//     {
//          arr.push(Math.floor(Math.random() * 10));
//     }
        
  
   
//     console.log(arr[i]);
// }
// var ph_data = {
//     labels:ph_labels,
//     datasets: [
//         {
//             label: 'PH Times Series Chart',
//             backgroundColor: '#17c3b2',
//             data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22,4],

//         },
//     ],
// }
// const ph_config = {
//     type: 'line',
//     data: ph_data,

// }
// const ph_canvas = document.getElementById('ph_canvas');
// const ph_chart = new Chart(ph_canvas,ph_config);

// function update_chart(){
//     for(let i = 0;i <24;i++)
//     {
//         ph_chart.data.datasets[0].data[i] = arr[i];
//         ph_chart.update();
//     }

// }
// // TDS CHART
// const tds_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
// const tds_data = {
//     labels:tds_labels,
//     datasets: [
//         {
//             label: 'TDS Times Series Chart',
//             backgroundColor: '#fea837',
//             data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22],

//         },
//     ],
// }
// const tds_config = {
//     type: 'line',
//     data: tds_data,

// }
// const tds_canvas = document.getElementById('tds_canvas')
// const tds_chart = new Chart(tds_canvas,tds_config)

// // TEMP CHART

// const temp_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
// const temp_data = {
//     labels:temp_labels,
//     datasets: [
//         {
//             label: 'Temperature Times Series Chart',
//             backgroundColor: '#e97254',
//             data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22],

//         },
//     ],
// }
// const temp_config = {
//     type: 'line',
//     data: temp_data,

// }

// const temp_canvas = document.getElementById('temp_canvas')
// const temp_chart = new Chart(temp_canvas,temp_config)
var ph_chart_value;
var tds_chart_value;
var temp_chart_value;
var ws = new WebSocket('ws://127.0.0.1:8000/ws/xuantruong/')
    ws.onopen = function () {
        console.log('Websocket is open...')
        ws.send('Hi, Message from Client to Server...')
    }
    ws.onmessage = function (event) {
        var chData = JSON.parse(event.data)
        console.log('Websocket is open...', chData)
        ph_chart_value = chData["PH"];
        tds_chart_value = chData["TDS"];
        temp_chart_value = chData["TEMP"];
    }

    ws.onerror = function (event) {
        console.log('Websocket is Error...', event)
    }
    ws.onclose = function (event) {
        console.log('Websocket is Close...', event)
    }


var ChartData = {
    labels: [],
    datasets: [{
        label: 'PH',
        data: [],
        borderColor: 'rbg(255, 99, 132)',
        tension: 0.1
    }
    ]
};

var ChartOption = {
    responsive: true,
    interaction: {
        mode: 'index',
        intersect: false,
    },
    scales: {
        x: {
            display: true,
            title: {
                display: true,
                text: 'Time'
            }
        },
        y: {
            display: true,
            title: {
                display: true,
                text: 'Value'
            }
        }
    }
};

var ph_canvas = document.getElementById('ph_canvas');
var ctx = ph_canvas.getContext('2d');
var myphChart = new Chart(ctx, {
    type: 'line',
    data: ChartData,
    options: ChartOption
});

setInterval(function(){
    var value_ph = ph_chart_value;
    console.log(ph_chart_value)
    var time = new Date().toLocaleTimeString();
    myphChart.data.labels.push(time);
    myphChart.data.datasets[0].data.push(value_ph);
    myphChart.update();
}, 5000);


var ChartData = {
    labels: [],
    datasets: [{
        label: 'TDS',
        data: [],
        borderColor: 'rbg(36, 39, 237)',
        tension: 0.1
    }
    ]
};

var ChartOption = {
    responsive: true,
    interaction: {
        mode: 'index',
        intersect: false,
    },
    scales: {
        x: {
            display: true,
            title: {
                display: true,
                text: 'Time'
            }
        },
        y: {
            display: true,
            title: {
                display: true,
                text: 'Value'
            }
        }
    }
};

var tds_canvas = document.getElementById('tds_canvas');
var ctx = tds_canvas.getContext('2d');
var mytdsChart = new Chart(ctx, {
    type: 'line',
    data: ChartData,
    options: ChartOption
});

setInterval(function(){
    var value_tds = tds_chart_value;
    var time = new Date().toLocaleTimeString();
    mytdsChart.data.labels.push(time);
    mytdsChart.data.datasets[0].data.push(value_tds);
    mytdsChart.update();
}, 5000);

var ChartData = {
    labels: [],
    datasets: [{
        label: 'Temperature',
        data: [],
        borderColor: 'rbg(237, 36, 36)',
        tension: 0.1
    }
    ]
};

var ChartOption = {
    responsive: true,
    interaction: {
        mode: 'index',
        intersect: false,
    },
    scales: {
        x: {
            display: true,
            title: {
                display: true,
                text: 'Time'
            }
        },
        y: {
            display: true,
            title: {
                display: true,
                text: 'Value'
            }
        }
    }
};

var temp_canvas = document.getElementById('temp_canvas');
var ctx = temp_canvas.getContext('2d');
var mytempChart = new Chart(ctx, {
    type: 'line',
    data: ChartData,
    options: ChartOption
});

setInterval(function(){
    var value_temp = temp_chart_value;
    var time = new Date().toLocaleTimeString();
    mytempChart.data.labels.push(time);
    mytempChart.data.datasets[0].data.push(value_temp);
    mytempChart.update();
}, 5000);

