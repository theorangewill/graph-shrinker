## Graph shrinker
This program shrink graph, deleting unecessary vertices on graphs.
For example, consider the following graph:
![Alt text](img/graph-not-shrinked.jpg)
The red vertices can be replaced by a single vertix to reduce the graph size.


### data
There are small inputs (.in) in data folder which are not real. There are also big inputs (.g) which are real.

Example of input:
G 3 4 2
N 1 0.0 0.0 2 2 0.9 3 0.8
N 2 1.0 1.0 1 3 1.3
N 3 2.0 2.0 1 1 9.0

The first line has infos about the graph (G): number of vertices, number of edges and maximum degree. 
This graph has 3 vertices, 4 edges and 2 as maximum degree.

The next lines have infos about each vertix (N): vertix id, latitude, longitude, number of neighbours, list of neighbours with neighbour's id and the edge weight.
The vertix 1 has 2 neighbours, 2 and 3. The edge to vertix 2 has weight 0.9 and to 3, weight 0.8.


### usage

```
./shrink <input> <shrinked> <paths>
    <input> is the data
    <shrinked> is the name of the file with the input graph shrinked
    <shrinked> is the name of the file with shrinked paths


./shrink data/input1.in shrinked.out paths.out
```

For the above graph, which is the input1.in file, the outpurs would contain:
shrinked.out:

![Alt text](img/graph-shrinked.jpg)
paths.out:
![Alt text](img/paths.jpg)