//
//  main.cpp
//  Dijksta's for Arduino
//
//  Created by Mike Nourian on 3/4/18.
//  Copyright © 2018 Mike Nourian. All rights reserved.
//

#include <iostream>

//Globals
int NUM_NODES = 16;


//end of globals

#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

struct Point {
    bool inSPT;
    int pointID;
    int distance; //record the distance of all the points
    Point * prev;
    int northNeighbor;
    int rightNeightbor;
    int leftNeighbor;
    int southNeighbor;
};

//for knowing the directions such as turnleft, right and ..., we need this to know the relative directions
void initialize_points_for_16_nodes(Point * points){
    for (int i = 0; i <  NUM_NODES; i ++){
        points[i].northNeighbor = i + 4;
        points[i].southNeighbor = i -4;
        points[i].leftNeighbor = i -1;
        points[i].rightNeightbor = i+1;
        if ( i>= 12 && i <=15){
            points[i].northNeighbor = -1;
        }
        if (i >=0 && i <= 3){
            points[i].southNeighbor = -1;
        }
        if (i %4 == 0){
            points[i].leftNeighbor= -1;
        }
        if (i %4 == 3){
            points[i].rightNeightbor = -1;
        }
    }
}


int findShortestDistanceInGraph(Point * points){
    int min= INT_MAX;
    int shortestPathPt = 0;
    for (int i = 0; i < NUM_NODES; i++){
//        int distance = points[i].distance;
//        bool inSpt = points[i].inSPT;
        if (points[i].inSPT == false && points[i].distance < min){
            shortestPathPt = i;
            min = points[i].distance;
        }
    }
    return shortestPathPt;
}
int numberOfNodesToBeTraced = 0;



void getNumNodesToBeTraced(Point * points,int size, int * nodes, int source, int dest){
    int j = dest;
    int i = size - 1;
    while (j != source && j >=0 && i >= 0){
        nodes[i] = points[j].pointID;
        j = points[j].prev->pointID;
        i --;
    }
    nodes[0] = source;
}

int getDirection (Point * points, int firstPoint, int secondPoint){
    //use norht
    if (points[firstPoint].leftNeighbor == points[secondPoint].pointID){
        return WEST;
    }
    if (points[firstPoint].rightNeightbor == points[secondPoint].pointID){
        return EAST;
    }
    if (points[firstPoint].northNeighbor == points[secondPoint].pointID){
        return NORTH;
    }
    if (points[firstPoint].southNeighbor == points[secondPoint].pointID){
        return SOUTH;
    }
    return -1;
}


//directionarray will be returned
void backtrace(Point * points, int * nodes, int size, int * directionArray, int directionArraySize){
    for (int i = 0; i < directionArraySize; i ++){
        int direction = getDirection(points, nodes[i], nodes[i+1]);
        if (direction == -1){fprintf(stderr, "error in backtrace");}
        directionArray[i] = direction;
    }
}

//points start at 0 -->
void shortestPath(int graph[16][16], int source, int dest, int initialDirection){
    //create an array of points so that we can keep track of previous points for backtrace
    Point points [NUM_NODES];
    for (int i = 0; i < NUM_NODES; i++){
        points[i].pointID = i;
        points[i].distance = INT_MAX; //set all the distances initially to INFINITY
        points[i].prev = NULL;
        points[i].inSPT = false;
    }
    initialize_points_for_16_nodes(points);
    points[source].distance = 0; //set the distance equal to 0 for the source
    
    for (int i = 0; i < NUM_NODES && points[dest].inSPT == false; i++){
        //take the shortest distance in the graph and update its neighbors, using the matrix
        int minPt = findShortestDistanceInGraph(points);
        //set as in inSPT
        points[minPt].inSPT = true;
        //update the neighbors
        //u is the min pt
        for (int j = 0; j < NUM_NODES; j++){
            if (points[j].inSPT == false && graph[minPt][j] && points[minPt].distance != INT_MAX && points[minPt].distance+ graph[minPt][j]< points[j].distance){
                //set the previous pointer
                points[j].prev = &points[minPt];
                points[j].distance = points[minPt].distance + graph[j][minPt];
            }
        }
    }
    //now we have the the shortest path and we can backtrace
    int j = dest;
    
    //I dont want to use STL like queue or vector, since I am running on Arduino
    int count = 0;
    while (j != source){
        count++;
        j = points[j].prev->pointID;
    }
    count ++; //to include the source
    //create an array of size count
    int nodes [count];
    //this function modifies the nodes so now nodes has src -> p1 -> ... -> dest
    getNumNodesToBeTraced(points, count, nodes, source, dest);
    for (int i = 0; i < count; i++){
        fprintf(stdout, "%d,", nodes[i]);
    }
    fprintf(stdout, "\n");
    int directionsArray [count-1];
    backtrace(points, nodes, count, directionsArray, count-1);
    
    //when we update the u point (v --> u), then we update the previous points
}




int main(int argc, const char * argv[]) {
    
    //0 represents that there is no edge between the nodes
    int graph[16][16] = {
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,1,0,0,40,0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,40,0,0,0,0,0,0,0,50,0,0,0,0,0},
        {0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,50,0,0,0,0,0,0,0,20,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,20,0,0,0,0,5},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,5,0},
    };
    
    
    shortestPath(graph, 0, 14, 2);

    
}
