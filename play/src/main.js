var dagreD3 = require('dagre-d3');
var d3 = require('d3');
var qajax = require('qajax');

document.addEventListener("DOMContentLoaded", function(event) {

    qajax("data.json").then(function(xhr) {

        var data = JSON.parse(xhr.responseText);

        var g = new dagreD3.graphlib.Graph().setGraph({});

        var svg = d3.select("svg"),
            inner = svg.append("g");

        g.setNode("A", { style: "fill: #afa" });

        g.setNode("B", { labelStyle: "font-weight: bold" });

        g.setNode("C", { labelStyle: "font-size: 2em" });

        g.setEdge("A", "B", {
            style: "stroke: #f66; stroke-width: 3px; stroke-dasharray: 5, 5;",
            arrowheadStyle: "fill: #f66"
        });

        g.setEdge("C", "B", {
            label: "A to C",
            labelStyle: "font-style: italic; text-decoration: underline;"
        });

        var render = new dagreD3.render();

        render(inner, g);

        var xCenterOffset = (svg.attr("width") - g.graph().width) / 2;
        inner.attr("transform", "translate(" + xCenterOffset + ", 20)");
        svg.attr("height", g.graph().height + 40);

        document.onkeydown = function(event) {
            if(event.keyCode == 39 || event.keyCode == 37) {
                if(event.keyCode == 39) { //right
                    g.setNode("A", { style: "fill: #000" });
                } else { //left
                    g.setNode("A", { style: "fill: #FFF" });
                }
                render(inner, g);
            }
        };
    });
});
