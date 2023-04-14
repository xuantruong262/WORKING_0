var acid_value = document.getElementById("acid_volume").innerHTML;
var base_value = document.getElementById("base_volume").innerHTML;
var nutriA_value = document.getElementById("nutriA_volume").innerHTML;
var nutriB_value = document.getElementById("nutriB_volume").innerHTML;

let circularProgress_acid       =   document.querySelector("#acid_progress"),
    circularProgress_base       =   document.querySelector("#base_progress"),
    circularProgress_nutriA     =   document.querySelector("#nutriA_progress"),
    circularProgress_nutriB     =   document.querySelector("#nutriB_progress");
    

let progressValue_acid          =   document.querySelector("#acid_volume"),
    progressValue_base          =   document.querySelector("#base_volume"),
    progressValue_nutriA        =   document.querySelector("#nutriA_volume"),
    progressValue_nutriB        =   document.querySelector("#nutriB_volume");



let progressStartValue           =   0,
    progressEndValue_acid        =   acid_value,
    progressEndValue_base        =   base_value,
    progressEndValue_nutriA      =   nutriA_value,
    progressEndValue_nutriB      =   nutriB_value,
    speed                        =   10;


let progress            = setInterval(() =>{
    progressStartValue++;

    if(progressStartValue <= progressEndValue_acid )
    {
        progressValue_acid.textContent = `${progressStartValue}%`
        circularProgress_acid.style.background = `conic-gradient(red ${progressStartValue * 3.6}deg, rgba(153, 151, 125, 0.671) 0deg)`
    }
    if(progressStartValue <= progressEndValue_base )
    {
        progressValue_base.textContent = `${progressStartValue}%`
        circularProgress_base.style.background = `conic-gradient(blue ${progressStartValue * 3.6}deg, rgba(153, 151, 125, 0.671) 0deg)`
    }
    if(progressStartValue <= progressEndValue_nutriA )
    {
        progressValue_nutriA.textContent = `${progressStartValue}%`
        circularProgress_nutriA.style.background = `conic-gradient(green ${progressStartValue * 3.6}deg, rgba(153, 151, 125, 0.671) 0deg)`  
    }
    if(progressStartValue <= progressEndValue_nutriB )
    {
        progressValue_nutriB.textContent = `${progressStartValue}%`
        circularProgress_nutriB.style.background = `conic-gradient(yellow ${progressStartValue * 3.6}deg, rgba(153, 151, 125, 0.671) 0deg)`
    }


    if(progressStartValue > 100)
    {
        clearInterval(progress);
    }

},speed);




