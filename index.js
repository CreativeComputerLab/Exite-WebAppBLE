var express = require("express");
const { exec } = require("child_process");
var path = require("path");
var bodyParser = require("body-parser"); 

var app = express();
app.use(express.static(__dirname + "/"));
app.use(bodyParser.urlencoded({
  extended: true
}));
app.use(bodyParser.json());

// Initialize the app.
var server = app.listen(process.env.PORT || 9090, function () {
    var port = server.address().port;
    console.log("App now running on port", port);
});
