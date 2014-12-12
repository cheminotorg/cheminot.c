var dagreD3 = require('dagre-d3');
var d3 = require('d3');
var qajax = require('qajax');
var moment = require('moment');

(function() {

    var defaultStyle = function() { return { style: "fill: #afa"}; };
    var headStyle = function() { return { style: "fill: #B4B4D5" }; };

    var data;
    var index = -1;

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
            listenMenu();
        }).catch(function(error) {
            console.error(error);
        });
    }


    function previousState() {
        var state = data[index];
        var head = state.refineArrivalTimes.head;
        var headId = nodeId(head);
        var pushed = state.refineArrivalTimes.pushed;

        g.setNode(headId, defaultStyle());
        pushed.forEach(function(gi) {
            var giId = nodeId(gi);
            g.removeNode(giId);
        });

        renderGraph();
        var previousState = data[--index];
        renderDepartures(previousState.refineArrivalTimes.departures);
        renderEdges(previousState.refineArrivalTimes.edges);
        renderMatched(previousState.refineArrivalTimes.matched);
    }

    function nextState() {
        var state = data[++index];
        var head = state.refineArrivalTimes.head;
        var headId = nodeId(head);
        var pushed = state.refineArrivalTimes.pushed;

        g.setNode(headId, headStyle());

        if(pushed) {
            pushed.forEach(function(gi) {
                var giId = nodeId(gi);
                if(!g.hasNode(giId)) {
                    g.setNode(giId, defaultStyle());
                }
                g.setEdge(headId, giId, {});
            });
        }

        renderGraph();
        renderDepartures(state.refineArrivalTimes.departures);
        renderEdges(state.refineArrivalTimes.edges);
        renderMatched(state.refineArrivalTimes.matched);
    }

    function renderDepartures(departures) {
        var $departures = departures.reduce(function(acc, next) {
            var arrival = '<span class="arrival">'+ formatDateTime(next.arrival) +'</span>';
            var departure = '<span class="departure">'+ formatDateTime(next.departure) +'</span>';
            var pos = '<span class="pos">'+ next.pos +'</span>';
            var tripId = '<span class="tripId">'+ next.tripId +'</span>';
            return acc + '<li>' + [arrival, departure, tripId, pos].join('') + '</li>';
        }, '');
        document.querySelector('.departures ul').innerHTML = $departures;
    }

    function renderEdges(edges) {
        var $edges = edges.reduce(function(acc, edge) {
            return acc + '<li>' + edge + '</li>';
        }, '');
        document.querySelector('.edges ul').innerHTML = $edges;
    }

    function renderMatched(matched) {
        var $matched = matched.reduce(function(acc, next) {
            var arrival = '<span class="arrival">'+ formatDateTime(next.arrival) +'</span>';
            var departure = '<span class="departure">'+ formatDateTime(next.departure) +'</span>';
            var pos = '<span class="pos">'+ next.pos +'</span>';
            var tripId = '<span class="tripId">'+ next.tripId +'</span>';
            return acc + '<li>' + [arrival, departure, tripId, pos].join('') + '</li>';
        }, '');
        document.querySelector('.matched ul').innerHTML = $matched;
    }

    function renderGraph() {
        render(inner, g);
        var xCenterOffset = (svg.attr("width") - g.graph().width) / 2;
        inner.attr("transform", "translate(" + xCenterOffset + ", 20)");
        svg.attr("height", viewportHeight());
    }

    function toggleDisplay(el) {
        if(el.classList.contains('on')) {
            el.classList.remove('on');
        } else {
            el.classList.add('on');
        }
    }

    function listenMenu() {
        var departures = document.querySelector(".departures");
        departures.addEventListener("click", toggleDisplay.bind(null, departures));

        var edges = document.querySelector(".edges");
        edges.addEventListener("click", toggleDisplay.bind(null, edges));

        var matched = document.querySelector(".matched");
        matched.addEventListener("click", toggleDisplay.bind(null, matched));
    }

    function listenArrowKeys() {
        document.onkeydown = function(event) {
            var pKey = 80;
            var nKey = 78;
            if(event.ctrlKey) {
                if(event.keyCode == pKey || event.keyCode == nKey) {
                    if(event.keyCode == nKey) {
                        if((index + 1) < data.length) {
                            nextState();
                        }
                    } else {
                        if((index - 1) >= 0) {
                            previousState();
                        }
                    }
                }
            }
        };
    }

    function nodeId(node) {
        return node.stopId + "\n"+ formatDateTime(node.arrival);
    }

    function formatDateTime(timestamp) {
        return moment(timestamp * 1000).format('DD/MM/YYYY - HH:mm');
    }

    function viewportHeight() {
        return Math.max(document.documentElement.clientHeight, window.innerHeight || 0);
    }
})();