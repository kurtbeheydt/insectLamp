/*var client = mqtt.connect("wss://mqtt.allesaanelkaar.be/mqtt", {
  username: "InsectLampClient",
  password: "peterKurt"
});
*/

var client = mqtt.connect("ws://allesaanelkaar.be:9102");

var ledCount = 4;

var publishLedValues = function () {
  var ledValues = {};
  for (var i = 1; i <= ledCount; i++) {
    var slider = $("#slider" + i);
    if (slider.length) {
      ledValues[i] = parseInt(slider.val(), 10);
    } else {
      console.warn("Slider with id slider" + i + " not found.");
    }
  }

  var powerUpdate = {
    action: "setPower",
    ledValues: ledValues
  };

  client.publish("insectLamp/webUpdate", JSON.stringify(powerUpdate));
};

var drawSliders = function (data) {
  var sliderContainer = $("#sliderContainer");
  sliderContainer.empty();

  for (var i = 1; i <= ledCount; i++) {
    var sliderItem = $("<div>", {class: "slider-item col"});

    led = data.ledValues.find(led => led.id === i);

    var slider = $("<input>", {
      type: "range",
      min: "0",
      max: "255",
      value: led.pwm,
      class: "form-range",
      id: "slider" + i
    });

    var sliderText = $("<input>", {
      type: "number",
      min: "0",
      max: "255",
      value: led.pwm,
      class: "form-control",
      id: "sliderText" + i
    });

    slider.on("input", function () {
      var sliderText = $("#sliderText" + this.id.replace("slider", ""));
      sliderText.val($(this).val());
    });

    slider.on("change", function () {
      publishLedValues();
    });

    sliderText.on("input", function () {
      var value = $(this).val();

      if (value < 0 || value > 255) {
        alert("Value must be between 0 and 255.");
        return;
      }

      var slider = $("#slider" + this.id.replace("sliderText", ""));

      slider.val(value);
    });

    sliderText.on("change", function () {
      publishLedValues();
    });

    var sliderLabel = $("<h2>").text(led.name);

    sliderItem.append(sliderLabel);
    sliderItem.append(slider);
    sliderItem.append(sliderText);
    sliderContainer.append(sliderItem);
  }
};

$(document).ready(function () {
  $.getJSON("/api/", function (data) {
    ledCount = data.ledValues.length;
    drawSliders(data);
  });
});