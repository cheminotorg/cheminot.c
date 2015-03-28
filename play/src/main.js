var qajax = require('qajax');

MQA.EventUtil.observe(window, 'load', function() {
  var options = {
    elt: document.getElementById('map'),
    zoom: 10,
    latLng: { lat: 48.858859, lng: 2.3470599 }
  };

  window.map = new MQA.TileMap(options);

  MQA.withModule('smallzoom', 'mousewheel', function() {
    map.addControl(new MQA.SmallZoom());
    map.enableMouseWheelZoom();
  });
});

qajax("data.json").then(function(xhr) {
  render(JSON.parse(xhr.responseText));
}).catch(function(error) {
  console.error(error);
});

function render(data) {
  data['vertices'].forEach(function(vertice) {
    var point = new MQA.Poi({ lat: vertice.lat, lng: vertice.lng });
    point.setRolloverContent(vertice.name);
    point.setInfoContentHTML(vertice.name);
    map.addShape(point);
  });
}
