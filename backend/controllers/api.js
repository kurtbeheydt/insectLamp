var App = require("../App");

exports.index = (req, res) => {
  var data = App.getData();
  res.json(data);
};