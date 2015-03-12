# cheminot.c

`cheminot.c` is a C++ library that implement an answer to the TDSP problem (Time-Dependent Shortest Paths over Large Graphs).

## Two-Step-LTT algorythm

Two-Step-LTT is a two step algorythm namely "timeRefinement" algorythm and "pathSelection" algorythm.
In the first step, "timeRefinement" refines the earliest arrival time function gi(t), for all nodes vi, to reach ve from vs where t belongs to [ts, te].
The optimal starting time which minimizes travel time from vs to ve is also determined.
In the second step, based on the optimal starting time and arrival times, "pathSelection" recovers the optimal path from vs to ve.

### timeRefinement algorythm

For each node in the graph, we push a pair (ti, gi(ti)).
// ti is the last starting time computed.
// gi(ti) is the last arrival time for vs to vi computed.
Except for vs, we push ti = ts, and gi(ti) = INFINITE.
For vs, we push the pair (ts, ts), because gs(ts) = ts for t = [ts, ts].

The priority queue is sorted by gi(ti).
We first dequeue (ti, gi(ti)), the earliest arrival time from vs to ve, followed by (tk, gk(tk)).
The next earliest possible arrival time from vs to vi via any edge (vf, vi) is gk(tk) + min(wfi(gk(tk))).
We try to determine the latest starting time ti' that satisfies gi(t) <= gk(t) + min(wfi(gk(tk))).
We compute gi(t) for t = [ti, ti'], we can now compute gj(t) for t = [t, ti'].
We update the queue (tj, gj(t)).
If ti' >= te and vi = ve, the algorythm has done.
Else, we enqueue (ti`, gi(t)).
