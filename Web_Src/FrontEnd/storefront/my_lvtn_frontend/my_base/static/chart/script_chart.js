// PH CHART
const ph_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
var arr =[];
for(let i = 0;i<24;i++)
{
    if(i==0)
    {
        arr[0] = 4;
    }
    else
    {
         arr.push(Math.floor(Math.random() * 10));
    }
        
  
   
    console.log(arr[i]);
}
var ph_data = {
    labels:ph_labels,
    datasets: [
        {
            label: 'PH Times Series Chart',
            backgroundColor: '#17c3b2',
            data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22,4],

        },
    ],
}
const ph_config = {
    type: 'line',
    data: ph_data,

}
const ph_canvas = document.getElementById('ph_canvas');
const ph_chart = new Chart(ph_canvas,ph_config);

function update_chart(){
    for(let i = 0;i <24;i++)
    {
        ph_chart.data.datasets[0].data[i] = arr[i];
        ph_chart.update();
    }

}
// TDS CHART
const tds_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
const tds_data = {
    labels:tds_labels,
    datasets: [
        {
            label: 'TDS Times Series Chart',
            backgroundColor: '#fea837',
            data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22],

        },
    ],
}
const tds_config = {
    type: 'line',
    data: tds_data,

}
const tds_canvas = document.getElementById('tds_canvas')
const tds_chart = new Chart(tds_canvas,tds_config)

// TEMP CHART

const temp_labels = ['0h','1h','2h','3h','4','5','6','7h','8h','9h','10h','11h','12h','13h','14h','15h','16h','17h','18h','19h','20h','21h','22h','23h']
const temp_data = {
    labels:temp_labels,
    datasets: [
        {
            label: 'Temperature Times Series Chart',
            backgroundColor: '#e97254',
            data: [1,3,4,19,5,9,6,2,7,8,11,21,12,13,23,14,15,10,16,17,20,18,22],

        },
    ],
}
const temp_config = {
    type: 'line',
    data: temp_data,

}

const temp_canvas = document.getElementById('temp_canvas')
const temp_chart = new Chart(temp_canvas,temp_config)


