var dagreD3 = require('dagre-d3');
var d3 = require('d3');
var qajax = require('qajax');
var moment = require('moment');

(function() {

    var defaultStyle = function() { return { style: "fill: #afa"}; };
    var headStyle = function() { return { style: "fill: #B4B4D5" }; };

    var data;
    var index = 0;
    var lastState;

    var g;
    var svg;
    var inner;
    var render;

    document.addEventListener("DOMContentLoaded", init);

    function init() {
        qajax("data.json").then(function(xhr) {
            data = JSON.parse(xhr.responseText);
            g = new dagreD3.graphlib.Graph().setGraph({});
            svg = d3.select("svg");
            inner = svg.append("g");
            render = new dagreD3.render();

            nextState();
            listenArrowKeys();
        }).catch(function(error) {
            console.error(error);
        });
    }


    function previousState() {
        var head = lastState.refineArrivalTimes.head;
        var headId = nodeId(head);
        var pushed = lastState.refineArrivalTimes.pushed;

        g.setNode(headId, defaultStyle());
        pushed.forEach(function(gi) {
            var giId = nodeId(gi);
            g.removeNode(giId);
        });

        renderGraph();
    }

    function nextState() {
        var state = data[index];
        var head = state.refineArrivalTimes.head;
        var headId = nodeId(head);
        var pushed = state.refineArrivalTimes.pushed;

        g.setNode(headId, headStyle());

        pushed.forEach(function(gi) {
            var giId = nodeId(gi);
            if(!g.hasNode(giId)) {
                g.setNode(giId, defaultStyle());
            }
            g.setEdge(headId, giId, {});
        });

        renderGraph();

        lastState = state;
    }

    function renderGraph() {
        render(inner, g);
        var xCenterOffset = (svg.attr("width") - g.graph().width) / 2;
        inner.attr("transform", "translate(" + xCenterOffset + ", 20)");
        svg.attr("height", viewportHeight());
    }

    function listenArrowKeys() {
        document.onkeydown = function(event) {
            var pKey = 80;
            var nKey = 78;
            if(event.ctrlKey) {
                if(event.keyCode == pKey || event.keyCode == nKey) {
                    if(event.keyCode == pKey) { //right
                        if((index + 1) < data.length) {
                            index++;
                            nextState();
                        }
                    } else { //left
                        if((index - 1) >= 0) {
                            index--;
                            previousState();
                        }
                    }
                }
            }
        };
    }

    function nodeId(node) {
        return node.stopId + "\n"+ formatDateTime(node.arrivalTime);
    }

    function formatDateTime(timestamp) {
        return moment(timestamp * 1000).format('DD/MM/YYYY - HH:mm');
    }

    function viewportHeight() {
        return Math.max(document.documentElement.clientHeight, window.innerHeight || 0);
    }
})();