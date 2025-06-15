let router = require("express").Router();
let path = require("path");
var helpers = require("../helpers");

var apiController = require("../controllers/api");

router.route("/").get(apiController.index);

module.exports = router;