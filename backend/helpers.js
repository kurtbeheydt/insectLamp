var basicAuth = require("basic-auth");
var fs = require("fs");
var path = require("path");
let config = JSON.parse(fs.readFileSync(path.join(__dirname, "/config.json")));

exports.logMessage = (event, message, connId, data = false) => {
  var timestamp = new Date().toISOString();
  console.log(timestamp + " - " + event + " - " + connId + " - " + message);
  if (data) {
    console.log(data);
  }
};

exports.parseIntZero = (input, defaultValue) => {
  return parseInt(
    input
    ? input
    : defaultValue
      ? defaultValue
      : 0);
};

exports.stringToBoolean = input => {
  return input == "true";
};

exports.auth = (req, res, next) => {
  var user = basicAuth(req);
  if (!user || !user.name || !user.pass) {
    res.set("WWW-Authenticate", "Basic realm=Authorization Required");
    res.sendStatus(401);
    return;
  }
  if (user.name === config.user.name && user.pass === config.user.pass) {
    next();
  } else {
    res.set("WWW-Authenticate", "Basic realm=Authorization Required");
    res.sendStatus(401);
    return;
  }
};

exports.generateRandomString = length => {
  var result = "";
  var characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  var charactersLength = characters.length;
  for (var i = 0; i < length; i++) {
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
  }
  return result;
};

exports.isJSONStringObject = str => {
  try {
    const parsed = JSON.parse(str);
    return (typeof parsed === "object" && parsed !== null && !Array.isArray(parsed));
  } catch (e) {
    return false;
  }
};