/* 
 * File:   intradijkstra.h
 * Author: leo
 *
 * Created on March 24, 2013, 2:42 AM
 */

#ifndef INTRADIJKSTRA_H
#define	INTRADIJKSTRA_H
#define MAX 40
//#include "p.h"
#include<vector>
#include<map>
#include"DataStructures.h"
using namespace std;

class intradijkstra{
    private:
        //TODO: the size of matrices should be modified 
        //TODO: check if all of these variables are used and rename some of them
        int adjMatrix[MAX][MAX];
        int predecessor[MAX],distance[MAX];
        bool mark[MAX]; //keep track of visited node
        
        int numOfNodes,linkcount,u,v,w;
        bool deletable[MAX];
        int ecount, theallcount, i;
    public:
        vector <int> pathvector;
        int flag;
        int source,dest;
       // void recreate(int start,map<int,EdgeTable>& edgedata, int allcount,int end, int capacity);
        int checkAvailability( map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity);
        void readForShortest(int starting, int ending, int numNode, map<int, linkAvailableBandwithTable>& intraLinks, int startTimeSlot, int endTimeSlot, double capacity);
        void checkDirectionUSST();
        void checkDirectionEST();
    /*
    * Function read() reads No of vertices, Adjacency Matrix and source
    * Matrix from the user. The number of vertices must be greather than
    * zero, all members of Adjacency Matrix must be postive as distances
    * are always positive. The source vertex must also be positive from 0
    * to noOfVertices - 1
 
    */
        void read(int starting,int ending, int vnum, map<int,linkAvailableBandwithTable>& edgedata, int start, int end, double capacity);
        void readEST(int starting,int ending, int vnum, map<int,linkAvailableBandwithTable>& edgedata, int start, int end, double capacity, double cap_return);
        
 
    /*
    * Function initialize initializes all the data members at the begining of
    * the execution. The distance between source to source is zero and all other
    * distances between source and vertices are infinity. The mark is initialized
    * to false and predecessor is initialized to -1
    */
 
        void initialize();
 
    /*
    * Function getClosestUnmarkedNode returns the node which is nearest from the
    * Predecessor marked node. If the node is already marked as visited, then it search
    * for another node.
    */
 
        int getClosestUnmarkedNode();
    /*
    * Function calculateDistance calculates the minimum distances from the source node to
    * Other node.
    */
 
        void calculateDistance();
    /*
    * Function output prints the results
    */
 
        void output();
        void constructPath(int);
};

#endif	/* INTRADIJKSTRA_H */

