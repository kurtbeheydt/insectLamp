var express = require("express");
const bodyParser = require("body-parser");
let path = require("path");
var fs = require("fs");
var helpers = require("./helpers");
var mqtt = require("mqtt");
var App = require("./App");

let config = JSON.parse(fs.readFileSync(path.join(__dirname, "/config.json")));

// express app shizzle
let app = express();
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, "public")));

let apiRoutes = require("./routes/api");
app.use("/api", apiRoutes);

app.get("/", function (req, res) {
  res.sendFile(path.join(__dirname, "public/index.html"));
});

var http = require("http").createServer(app);

/*
var mqttOptions = {
  host: config.mqtt.host,
  port: config.mqtt.port,
  username: config.mqtt.username,
  password: config.mqtt.password,
  clientId: config.mqtt.clientId,
  protocol: "mqtts"
};
*/

var mqttOptions = {
  host: "allesaanelkaar.be",
  port: 8889,
  protocol: "mqtt"
};

mqttClient = mqtt.connect(mqttOptions);

App.init(mqttClient);

var port = config.port;

http.listen(port, function () {
  console.log("listening on *:" + port);
});