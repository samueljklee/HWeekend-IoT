var http = require('http');
var bodyParser = require('body-parser');
var express = require('express');
var app = express();
var bodyParser = require('body-parser');
var path = require("path");


//support parsing of application/json type post data
app.use(bodyParser.json());

app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

app.get('/', function (req, res) {
  res.send('Hello World')
});

app.post('/postjson', function(req, res) {
	var time  = req.body.Time;
	var hum  = req.body.Humidity;
	var temp = req.body.Temperature;
	var heat = req.body.HeatIndex;
	console.log(time);
	console.log(hum);
	console.log(temp);
	console.log(heat);
});

app.get('/:data', function(req, res) {
  var data  = req.param("data");
  console.log(data);// 1234
  res.send("Data is" + req.param("data"));
});
 
app.listen(3000, function () {
  console.log("== Server listening on port 3000");
});
