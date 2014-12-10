var dagreD3 = require('dagre-d3');
var d3 = require('d3');
var qajax = require('qajax');
var moment = require('moment');

var data = {};
var index = 0;

document.addEventListener("DOMContentLoaded", function(event) {
    qajax("data.json").then(function(xhr) {
        data = JSON.parse(xhr.responseText);
        renderGraph(data, index); //TODO
        listenArrowKeys();
    }).catch(function(error) {
        console.error(error);
    });
});

var nodes = {};

function renderGraph(data, index) {
    var g = new dagreD3.graphlib.Graph().setGraph({});
    var style = function() { return { style: "fill: #afa"}; };
    var svg = d3.select("svg"),
        inner = svg.append("g");

    var state = data[index];
    var head = state.refineArrivalTimes.head;
    var headId = formatNode(head);
    var pushed = state.refineArrivalTimes.pushed;

    g.setNode(headId, style());
    pushed.forEach(function(gi) {
        var giId = formatNode(gi);
        if(!nodes[giId]) {
            g.setNode(giId, style());
            nodes[giId] = true;
        }
        g.setEdge(headId, giId, {});
    });

    var render = new dagreD3.render();

    render(inner, g);

    var xCenterOffset = (svg.attr("width") - g.graph().width) / 2;
    inner.attr("transform", "translate(" + xCenterOffset + ", 20)");
    svg.attr("height", viewportHeight());
}

function listenArrowKeys() {
    document.onkeydown = function(event) {
        if(event.keyCode == 39 || event.keyCode == 37) {
            if(event.keyCode == 39) { //right
                renderGraph(data, ++index);
            } else { //left
                ++index;
                if(index > 0) renderGraph(data, index);
            }
        }
    };
}

function formatNode(node) {
    return node.stopId + "\n"+ formatDateTime(node.arrivalTime);
}

function formatDateTime(timestamp) {
  return moment(timestamp * 1000).format('DD/MM/YYYY - HH:mm');
}

function viewportHeight() {
    return Math.max(document.documentElement.clientHeight, window.innerHeight || 0);
}
