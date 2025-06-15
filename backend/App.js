var helpers = require("./helpers");

function Powerled(id, name) {
  this.id = id;
  this.name = name;
  this.pwm = 0;

  this.setPwm = function (pwm) {
    if (pwm < 0) {
      pwm = 0;
    } else if (pwm > 255) {
      pwm = 255;
    }
    this.pwm = pwm;
  };
}

var App = {
  mqttClient: false,
  powerLeds: [],
  presets: [],
  

  init(mqttClient, callback) {
    this.mqttClient = mqttClient;

    this.mqttClient.on("connect", function () {
      console.log("mqtt connected");
    });

    this.mqttClient.on("error", function (error) {
      console.log("mqtt connection error: " + error);
    });

    this.mqttClient.on("message", this.onMessage.bind(this));

    this.mqttClient.subscribe("insectLamp/#");

    this.powerLeds.push(new Powerled(1, "powerled01"));
    this.powerLeds.push(new Powerled(2, "powerled02"));
    this.powerLeds.push(new Powerled(3, "powerled03"));
    this.powerLeds.push(new Powerled(4, "powerled04"));
    //this.powerLeds.push(new Powerled(5, "powerled05"));

    helpers.logMessage("resetGame", "reset", "admin");

    if (callback) {
      callback(this.getData());
    }
  },

  sendPowerUpdate() {
    var powerUpdate = {
      action: "setPower",
      ledValues: {}
    };
    this.powerLeds.forEach(led => {
      powerUpdate.ledValues[led.id] = led.pwm;
    });

    this.mqttClient.publish("insectLamp/unitUpdate", JSON.stringify(powerUpdate), {
      qos: 1
    }, function (error) {
      if (error) {
        console.error("Error publishing power update:", error);
      } else {
        console.log("Power update sent successfully:", powerUpdate);
      }
    });
  },

  onMessage(topic, message) {
    var parsedMessage;

    const parts = topic.split("/");

    if (helpers.isJSONStringObject(message)) {
      parsedMessage = JSON.parse(message);
    } else {
      parsedMessage = message.toString();
    }

    console.log("Received message on topic:", topic, "with data:", parsedMessage);

    if (parts[1] === "init") {
      console.log("Initialization message received:", parsedMessage);
      this.sendPowerUpdate();
    } else if (parts[1] === "webUpdate") {
      if (parsedMessage.action == "setPower") {
        for (const [idStr, pwm] of Object.entries(parsedMessage.ledValues)) {
          const id = parseInt(idStr, 10);

          const led = this.powerLeds.find(led => led.id === id);
          console.log(`Updating LED ${id} with PWM ${pwm}`);

          if (led) {
            led.setPwm(pwm);
            console.log(`Set LED ${id} to PWM ${pwm}`);
          }
        }

        this.sendPowerUpdate();
      }
    }
  },

  getData() {
    var data = {
      ledValues: this.powerLeds,
      presets: this.presets,
    };

    return data;
  }
};

module.exports = App;