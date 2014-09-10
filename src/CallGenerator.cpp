// Zhe Song, University of Virginia
// June 2014

// Revised by Elahe, University of Virginia, Sep 2014

#include "CallGenerator.h"
#include "randgen.h"
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <stdexcept>


using namespace std;

CallGenerator::CallGenerator(int as_num) {
    //the index of AS
    asnum = as_num;
    //An array which used for producing random value to choos USST or EST in the randGen file
    USST_EST[0] = 1.0;
    USST_EST[1] = 2.0;
    //the index of generated Dest AS
    dest_AS = 1;
    //the index of genereated Dest node
    dest_node = 1;
}

//read the number of nodes in all the ASs
void CallGenerator::readNodeVector(int nodeNumArray[]) {
    total_node = 0;
    //find the size of array (which is the number of ASs)
    int arraySize=sizeof(nodeNumArray)/sizeof(nodeNumArray[0]);
    
    //push the node numbers in the vector
    for(int i=0; i<arraySize; i++)
        nodevec.push_back(nodeNumArray[i]);
    
    //find the total size of node numbers
    for (int i = 0; i < nodevec.size(); i++)
        total_node += nodevec[i];
        
}

/*
Input (path): Full path of the file that has the common parameters
 For example, call the function below with this argument
"C:\Users\mmv\Dropbox\AAAMEDPaperProject\common\common_parameter"

Function Read every parameter in the file and assign values to corresponding variables of the CallGenerator object
*/
void CallGenerator::readCommonFile(string path)
{
    // define input stream
    ifstream inf(path.c_str());
    
    //if the input stream opened correctly
    if (inf.is_open()) {
        
        //define a line string
        string line;
        //while reading lines from the input file
        while (getline(inf, line)) {
            //check if it is a comment line, ignore it
            if (line[0] == '/' && line[1] == '/')
                continue;
            else {
                
                stringstream lineStream(line);
                string indicator;
                //read the first word of the line to find what is the type of call (USST or EST)
                lineStream >> indicator;
                
               // read the next parameters
                if (indicator == "USST") {
                    lineStream >> USSTcap >> USSTduration >> USSTn >> USST_EST_prob[0];
                } else if (indicator == "EST") {
                    lineStream >> ESTcap >> ESTduration_min >> ESTduration_max >> ESTn >> USST_EST_prob[1] >> zipf_alpha;
                } else {
                    cout << "Read Common file error!" << endl;
                }
            }
        }
    } else {
        cout << "Unable to open common file for AS" << asnum << "!";
    }
    
    //close the input file
    inf.close();
}


/*

Input: path of input file which is src_dst_prob_matrix.

Example input file:C:\Users\mmv\Dropbox\AAAMEDPaperProject\common\src_dst_prob_matrix

Function loads the matrix from the file into the CallGenerator variable "theMatrix"
*/
void CallGenerator::readprobMatrix(string path) {
    //define an input stream
    ifstream inf(path.c_str());
    //if it opened correctly
    if (inf.is_open()) {
        //read 2D matrix and save it in the array probMatrix
        for (int i = 0; i < total_node; i++) {
            for (int j = 0; j < total_node; j++) {
                inf >> probMatrix[i][j];
            }
        }
    } else {
        cout << "Unable to open common file for AS" << asnum << "!";
    }
    inf.close();
}

/*
 Input index is a number used to identify the AS number and node-number-within-AS. For example, AS1 has 6 nodes; AS2 has 9 nodes, etc. See SimulationTopology.jpg in \Dropbox\AAAMEDPaperProject\inputdata. Then if index = 5, the output of this function will be dest_AS=1, dest_node = 5. These two variables are private variables in CallGenerator object.
 
Job of the function: Take index as input and output corresponding AS number and node number 

Output: dest_AS and dest_node variables of CallGenerator object instance get populated.

Clearly, this function needs to assume a topology, or at least have information about how many nodes are in each AS.

The implication is that this function cannot be tested until nodevec variable of CallGenerator is loaded. WHICH FUNCTION DOES THAT? In other words, which function should be called before calling mapNode.

*/
void CallGenerator::mapNode(int index) {
    int tmp_as = 0;
    int tmp_nodes = index;
    while (tmp_nodes >= nodevec[tmp_as]) {
        tmp_nodes -= nodevec[tmp_as];
        tmp_as++;
    }
    dest_AS = tmp_as + 1;
    dest_node = tmp_nodes + 1;
}

/*
 Input: The AS number and the node number of a node. 
 Output: An index number which shows the row of that specific node in prob matrix
 job: find the row number in the prob matrix; So if the AS number is 1 and the node number is 5, then the output index is 5
 */

int CallGenerator::mapIndex(int AS_num, int vertex_num) {
    int index = 0;
    //add the number of nodes of ASs before the AS number of this node
    for (int i = 0; i < (AS_num - 1); i++) {
        index += nodevec[i];
    }
    //add the number of node (to add the number of nodes which are before this node)
    index += vertex_num;
    //subtract one because the index starts form 0
    index -= 1;
    
    return index;
}
/*
 * This is the function that generate calls. Meaning that this function will generate random numbers of AR time options, duration, capacity(call rate) 
 * source node, Destination AS, destination node according to their distributions
 * Input: arr: arrival time; windowsize: the window size of the advance reservation window; leadtime: the leadtime(see paper for more explanation about lead time, slot_length: the time length of a single slot,
 * for example: one slot can be in the length of 1 min, 30 seconds or 1 hour;)
 * flag = 0:normal inter-domain
 * flag = 1:intra-domain
 * suggest modification:
 * here I use a flag to distinguish inter domain calls from intra domain calls. Th reason of this implementation is that: the source node is already chosen before
 * calling this function, and the source node is chosen by a rand() function of C++; However, a more native implementation should be: taken the portion of the matrix that corresponds
 * to the source AS(for example, the source AS is 1, and so we should take those rows from 1 to 6) and form a 1D probability vector. Each index of this vector corresponds to
 * a {source_node:(destination_AS,destination_node)} pair and call the rand_prob_vector() function in randgen.c to generate source and destination.
 * 
 * After calling this function, you can acquire all the parameters from public variables source_node, dest_AS, dest_node, ARvec,Duration and Capacity.  
*/
void CallGenerator::generateCall(double arr, int windowsize, int leadtime, int slot_length, int flag) {

    if (ARvec.size() != 0)
        ARvec.clear();

    //generate random number for source vertex
    if (asnum > nodevec.size())
        cout << "More AS than expected!" << endl;
    
    //get the number of nodes for this AS
    int vertices_num_of_this_AS = nodevec[asnum - 1];

    //Generate random source node
    source_node = (rand() % vertices_num_of_this_AS) + 1;

    //generate an index for this node 
    int thisindex = mapIndex(asnum, source_node);
    
    double probvec[total_node];
    //get the probability values of this specific source node (according to the row number)
    for (int i = 0; i < total_node; i++)
        probvec[i] = probMatrix[thisindex][i];
    
    
    //save the ASindex in the array to use it for producing random value for which AS to be Dest AS
    double candvec[total_node];
    for (int i = 0; i < total_node; i++)
        candvec[i] = (double) i;
    
    
    int tmpindex;
    
    //inter-domain
    if (flag == 0) {
        //TODO: How does exactly this function work?
        //produce a random dest AS and dest nodeNum in index format
        tmpindex = (int) rand_prob_vector(candvec, probvec, total_node);
        //convert the produced index to the corresponding dest_AS and Dest_Node
        mapNode(tmpindex);
    } else   //intra-domain
    {
        //for intra domain the Dest_AS is within this AS and equals asnum
        dest_AS = asnum;
        //produce a random dest_node
        dest_node = (rand() % vertices_num_of_this_AS) + 1;
        //continue producing a new dest node if it equals with source node
        while (dest_node == source_node)
            dest_node = (rand() % vertices_num_of_this_AS) + 1;
    }
    
    //TODO: What does EST and USST exactly mean?
    
    //whether it's a USST call or EST call
    double indicator = rand_prob_vector(USST_EST, USST_EST_prob, 2);
    
    //USST
    if (indicator == 1.0) {
        //this values were determined based on common file 
        Capacity = USSTcap;
        Duration = USSTduration * 60 / slot_length;
        
        //generate AR option times according to USSTn value
        for (int i = 0; i < USSTn; i++) {
            int ARoption = 0;
            //if the AR vector is not empty produce one ARoption randomely which is not equal to the last inserted element in ARvec
            if (ARvec.size() != 0) {
                do {
                    ARoption = rand() % (windowsize) + leadtime;
                } while (find(ARvec.begin(), ARvec.end(), ARoption) != ARvec.end());
            } else
                //if ARvec is empty, produce a random ARoption (doesn't need to check the last element of ARvec)
                ARoption = rand() % (24 * 60) + 1;
            //push the produced AR option into ARvec
            ARvec.push_back(ARoption);
        }
        //sort the AR options in increasing order
        sort(ARvec.begin(), ARvec.end());


    }//EST
    else if (indicator == 2.0) {
        Capacity = ESTcap;
        //produce the duration value according to zipf distribution
        Duration = (zipf(zipf_alpha, (ESTduration_max - ESTduration_min)) + ESTduration_min)*60 / slot_length;
        //push the sequential AR options based on the number of ESTn
        for (int i = 0; i < ESTn; i++) {
            ARvec.push_back(leadtime + i); //TODO: arrival time + leadtime + i
        }
    } else
        cout << "USST EST selection error!" << endl;
    
    
    //save the produced call in a file named Call_for_AS+number
    stringstream ss;
    ss << arr << " " << ARvec[0];
    for (int i = 1; i < ARvec.size(); i++) {
        ss << " " << ARvec[i];
    }
    ss << " " << Capacity << " " << Duration << " " << source_node << " " << dest_AS << " " << dest_node;
    
    stringstream sss;
    sss << "Call_for_AS" << asnum;
    ofstream fout;
    fout.open(sss.str().c_str(), ios::app);
    
    fout.close();
    
}


