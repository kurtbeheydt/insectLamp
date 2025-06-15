function collides(obj1, obj2) {
  return (obj1.x < obj2.x + obj2.width && obj1.x + obj1.width > obj2.x && obj1.y < obj2.y + obj2.height && obj1.y + obj1.height > obj2.y);
}

function positionColides(obj1, obj2) {
  return (obj1.position.x < obj2.position.x + obj2.width && obj1.position.x + obj1.width > obj2.position.x && obj1.position.y < obj2.position.y + obj2.height && obj1.position.y + obj1.height > obj2.position.y);
}

var isJSONStringObject = function (str) {
  try {
    const parsed = JSON.parse(str);
    return (typeof parsed === "object" && parsed !== null && !Array.isArray(parsed));
  } catch (e) {
    return false;
  }
};

function drawDigits(ctx, number, x, y, w, h, alignment = "left", spacing = 10) {
  const digits = number.toString().split("");
  const totalWidth = digits.length * w + (digits.length - 1) * spacing;

  if (alignment == "right") {
    x -= (w + spacing) * (digits.length - 1);
  }

  let startX = x;

  for (let i = 0; i < digits.length; i++) {
    const digit = parseInt(digits[i]);
    drawDigit(ctx, digit, startX, y, w, h);
    startX += w + spacing;
  }
}

function drawDigit(ctx, n, x, y, w, h) {
  var dw = (dh = (12 * 4) / 5);
  var blocks = DIGITS[n];

  if (blocks[0]) {
    ctx.fillRect(x, y, w, dh);
  }
  if (blocks[1]) {
    ctx.fillRect(x, y, dw, h / 2);
  }
  if (blocks[2]) {
    ctx.fillRect(x + w - dw, y, dw, h / 2);
  }
  if (blocks[3]) {
    ctx.fillRect(x, y + h / 2 - dh / 2, w, dh);
  }
  if (blocks[4]) {
    ctx.fillRect(x, y + h / 2, dw, h / 2);
  }
  if (blocks[5]) {
    ctx.fillRect(x + w - dw, y + h / 2, dw, h / 2);
  }
  if (blocks[6]) {
    ctx.fillRect(x, y + h - dh, w, dh);
  }
}

var DIGITS = [
  [
    1,
    1,
    1,
    0,
    1,
    1,
    1
  ], // 0
  [
    0,
    0,
    1,
    0,
    0,
    1,
    0
  ], // 1
  [
    1,
    0,
    1,
    1,
    1,
    0,
    1
  ], // 2
  [
    1,
    0,
    1,
    1,
    0,
    1,
    1
  ], // 3
  [
    0,
    1,
    1,
    1,
    0,
    1,
    0
  ], // 4
  [
    1,
    1,
    0,
    1,
    0,
    1,
    1
  ], // 5
  [
    1,
    1,
    0,
    1,
    1,
    1,
    1
  ], // 6
  [
    1,
    0,
    1,
    0,
    0,
    1,
    0
  ], // 7
  [
    1,
    1,
    1,
    1,
    1,
    1,
    1
  ], // 8
  [
    1,
    1,
    1,
    1,
    0,
    1,
    0
  ] // 9
];