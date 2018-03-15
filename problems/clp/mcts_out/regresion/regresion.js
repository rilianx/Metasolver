//numero maximo
//var maxIteration=13;

var configFile = "config.json";
d3.json(configFile,function(data){
  maxIteration = data.Max;
})

//var lineCoordinates =[{x1:0,y1:1,x2:1,y2:0}];

//tamaño del grafico
var height = 700;
var width = 1000;
//crep el lienzo
var svg = d3.select('#svg-container').append('svg').attr('height','100%').attr('width','100%');

// x e y son funciones que transforman del dominio numerico a posicion en el lienzo
var y = d3.scaleLinear().domain([0,0.1]).range([height,0]);
var x = d3.scaleLinear().domain([0,1]).range([0,width]);

//Crean los ejes
var yAxis = d3.axisLeft(y).ticks(10).tickPadding(6).tickSize(10);
var xAxis = d3.axisBottom(x).ticks(20).tickPadding(6).tickSize(10);

//Cada grupo contiene un tipo de figura (linea, punto)
var grp1 = svg.append('g').attr('transform','translate('+(50+ (320))+',20)');
var grp = svg.append('g').attr('transform','translate('+(50+ (320))+',20)');

//agrego los ejes
grp.append('g').attr('class','axis y').call(yAxis);

grp.append('g').attr('class','axis x').attr('transform','translate(0,'+height+')').call(xAxis);

//variables que se  muestran por pantalla en cada iteracion
var k=0;
var rsquared;

/*
variables para cambiar de archivo de input para imprimir puntos y lineas y mas informacion
*/
var rsquaredFile;
var rsquaredFileString="rsquared.json";
var pointsFile;
var pointsFileString = "points.json";
var lineFile;
var lineFileString = "line.json";

//velocidad  de reproduccion en milisegundos
var speed = 10;

//booleano para saber si se esta reproduciendo
var playing = false;

//creo un timer para poder llamar a la funcion refresh a una velocidad de 'speed'
var timer = setInterval(function(){refresh()},speed);
//lo detengo de inmediato porque el usuario debe decidir si reproduce o no
clearInterval(timer);

/* Controladores*/

//retrocede en una iteracion
function back(){
  if(playing==false && k>0){
  k--;
  pointsFile = "points/"+k+"_"+pointsFileString;
  lineFile = "points/"+k+"_"+lineFileString;
  rsquaredFile = "points/"+k+"_"+rsquaredFileString;

  //alert(pointsFile);
  refresh();
  }
}

//da inicio a la reproduccion
function play(){

  timer = setInterval(function(){refresh()},speed);
  playing = true;
}

//deja de reproducir
function pause(){
  clearInterval(timer);
  playing = false;
}

//va a la siguiente iteracion
function next(){
  if(playing==false && k<maxIteration){
  k++;
  pointsFile = "points/"+k+"_"+pointsFileString;
  lineFile = "points/"+k+"_"+lineFileString;
  rsquaredFile = "points/"+k+"_"+rsquaredFileString;

  //alert(pointsFile);
  refresh();
  }
}

//
function refresh(){
  //k++;
  if(k <= maxIteration){
    document.getElementById("Iteration_number").innerHTML= k;
    getRSquared();
    printPoints();
    printLine2();
    if (playing) {
      pointsFile = "points/"+k+"_"+pointsFileString;
      lineFile = "points/"+k+"_"+lineFileString;
      rsquaredFile = "points/"+k+"_"+rsquaredFileString;
      k++;
    }

  }else {
    pause();
  }
  //printLine();
}


/* Graficadores*/

function printLine(){
  grp1.selectAll("line")
    .remove();



  grp1.selectAll("line")
    .data(lineCoordinates)
    .enter().append('line')
    .attr('stroke','black')
    .attr('strokeWidth','1')
    .attr('x1',function(d){return x(d.x1);})
    .attr('y1',function(d){return y(d.y1);})
    .attr('x2',function(d){return x(d.x2);})
    .attr('y2',function(d){return y(d.y2);});
}

function printLine2(){
  d3.json(lineFile,function(error,data){
    //console.log(data);
    grp1.selectAll("line")
      .remove();

    grp1.selectAll("line")
      .data(data)
      .enter().append('line')
      .attr('stroke','black')
      .attr('strokeWidth','1')
      .attr('x1',function(d){return x(0);})
      .attr('y1',function(d){return y(d.intercept);})
      .attr('x2',function(d){return x(1);})
      .attr('y2',function(d){return y(d.slope+d.intercept);});
  });
}


function printPoints(){
  //alert(pointsFile);
  d3.json(pointsFile, function (error, data) {
    grp.selectAll("circle")
    .remove();
    escribirPuntos(data);
    grp.selectAll("circle")
      .data(data)
      .enter().append('circle')
          .attr('cx',function(d){return x(d.x);})
          .attr('cy',function(d){return y(d.y);})
          .attr('r','3');
  });

}

function escribirPuntos(data){
  $("#listapuntos").empty();
  for (var i = 0; i < data.length; i++) {
      $("#listapuntos").append("  <tr><td>"+data[i].x+"</td><td>"+data[i].y+"</td></tr>");
  }

}

  function getRSquared(){
    //alert(rsquaredFile);
    d3.json(rsquaredFile,function(data){

      rsquared = data[0].rsquared;
      //alert(rsquared);

      document.getElementById("rsquared").innerHTML= rsquared;

    })
  }
