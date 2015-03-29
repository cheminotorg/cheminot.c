var qajax = require('qajax');
var qstart = require('qstart');

qstart.then(function() {
  qajax("data.json").then(function(xhr) {
    var data = JSON.parse(xhr.responseText);
    L.mapbox.accessToken = 'pk.eyJ1Ijoic3JlbmF1bHQiLCJhIjoiNGRHRzgxWSJ9.pawb4Qw10gD_8dbE-_Qrvw';
    var map = L.mapbox.map('map', 'srenault.ljcc52c6').setView([48.858859, 2.3470599], 9);
    var myLayer = L.mapbox.featureLayer().addTo(map);

    var features = data['vertices'].map(function(vertice, index) {
      return {
        type: 'Feature',
        properties: {
          title: '['+ index +'] ' + vertice.name,
          'marker-color': '#2F8EDD'
        },
        geometry: {
          type: 'Point',
          coordinates: [vertice.lng, vertice.lat]
        }
      };
    });

    myLayer.setGeoJSON({
      type: 'FeatureCollection',
      features: features
    });

    myLayer.on('mouseover', function(e) {
      e.layer.openPopup();
    });

    myLayer.on('mouseout', function(e) {
      e.layer.closePopup();
    });
  });
}).catch(function(e) {
  console.log(e);
});
