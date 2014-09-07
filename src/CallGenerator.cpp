// Zhe Song, University of Virginia
// June 2014

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
    asnum = as_num;
    USST_EST[0] = 1.0;
    USST_EST[1] = 2.0;
    dest_AS = 1;
    dest_node = 1;
}

void CallGenerator::readNodeVector(int[] nodeNumArray) {
    total_node = 0;
    /*int node;
    ifstream inf(dir.c_str());
    if (inf.is_open()) {
        while (inf >> node) {
            nodevec.push_back(node);
        }
    } else {
        cout << "Unable to open node vector for AS" << asnum << "!";
    }
    */
    for(int i=0; i<nodeNumArray.size; i++)
        nodevec.push_back(nodeNumArray[i]);
        
    for (int i = 0; i < nodevec.size(); i++)
        total_node += nodevec[i];
    //initialize the matrix
    //to be modified later
    //theMatrix = new double[75][75];
    inf.close();
}

/*
Input (dir): Full path of the file that has the common parameters
 For example, call the function below with this argument
"C:\Users\mmv\Dropbox\AAAMEDPaperProject\common\common_parameter"

"dir" is poorly named because it sounds like directory, but it needs to be a file with a full path or path relative to the source file location

What does the function do: Read in every parameter in the file and assign values to corresponding variables of the CallGenerator object

To test if it worked correctly: in the test main.cpp program you write, print out CallGenerator object instance and see if the variables were assigned properly.

See the above folder for a common_parameter file to use as input.

*/
void CallGenerator::readCommonFile(string dir)
{

    ifstream inf(dir.c_str());
    if (inf.is_open()) {
        string line;
        while (getline(inf, line)) {
            if (line[0] == '/' && line[1] == '/')
                continue;
            else {
                stringstream lineStream(line);
                string indicator;
                lineStream >> indicator;
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
    inf.close();
}


/*

Input: Same as for readCommonFile function. But the file needs to be a src_dst_prob_matrix.

Example input file:C:\Users\mmv\Dropbox\AAAMEDPaperProject\common\src_dst_prob_matrix

Job of this function: Load the matrix from the file into the CallGenerator variable theMatrix (see CallGenerator.h - this is a private variable)

Output: test as recommended for readCommonFile

*/
void CallGenerator::readMatrix(string dir) {
    ifstream inf(dir.c_str());
    if (inf.is_open()) {
        for (int i = 0; i < total_node; i++) {
            for (int j = 0; j < total_node; j++) {
                inf >> theMatrix[i][j];
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
 Output: An index number(The input of the function mapIndex, see description of mapNode)
 job of the function: This function is the reserve version of the above function mapNode(); So if the AS number is 1 and the node number is 5, then the output index is 5
 */

int CallGenerator::mapIndex(int AS_num, int vertex_num) {
    int index = 0;
    for (int i = 0; i < (AS_num - 1); i++) {
        index += nodevec[i];
    }
    index += vertex_num;
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

    /* initialize random seed: */
    // srand (time(NULL));
    if (ARvec.size() != 0)
        ARvec.clear();
    //generate random number for source vertex
    if (asnum > nodevec.size())
        cout << "More AS than expected!" << endl;
    int vertices_num_of_this_AS = nodevec[asnum - 1];
    //int vertices_num_of_this_AS = 3;
    source_node = (rand() % vertices_num_of_this_AS) + 1;
    //	source_node = 1;
    //generate random destination
    int thisindex = mapIndex(asnum, source_node);
    //cout<<"The index is :"<<thisindex <<"      "<<"!"<<endl;
    double probvec[total_node];
    for (int i = 0; i < total_node; i++)
        probvec[i] = theMatrix[thisindex][i];
    double candvec[total_node];
    for (int i = 0; i < total_node; i++)
        candvec[i] = (double) i;
    int tmpindex;
    //inter-domain
    if (flag == 0) {
        tmpindex = (int) rand_prob_vector(candvec, probvec, total_node);
        mapNode(tmpindex);
    } else
        //intra-domain
    {
        dest_AS = asnum;
        dest_node = (rand() % vertices_num_of_this_AS) + 1;
        while (dest_node == source_node)
            dest_node = (rand() % vertices_num_of_this_AS) + 1;
    }
    //int tmpindex = 5;

    //whether it's a USST call or EST call
    double indicator = rand_prob_vector(USST_EST, USST_EST_prob, 2);
    //double indicator = 1.0;
    //USST
    if (indicator == 1.0) {
        Capacity = USSTcap;
        Duration = USSTduration * 60 / slot_length;
        //generate AR time vector

        for (int i = 0; i < USSTn; i++) {
            int ARoption = 0;
            if (ARvec.size() != 0) {
                do {
                    ARoption = rand() % (windowsize) + leadtime;
                } while (find(ARvec.begin(), ARvec.end(), ARoption) != ARvec.end());
            } else
                ARoption = rand() % (24 * 60) + 1;
            ARvec.push_back(ARoption);
        }

        sort(ARvec.begin(), ARvec.end());


    }//EST
    else if (indicator == 2.0) {
        Capacity = ESTcap;
        Duration = (zipf(zipf_alpha, (ESTduration_max - ESTduration_min)) + ESTduration_min)*60 / slot_length;
        for (int i = 0; i < ESTn; i++) {
            ARvec.push_back(leadtime + i);
        }
    } else
        cout << "USST EST selection error!" << endl;

    stringstream ss;
    ss << arr << " " << ARvec[0];
    for (int i = 1; i < ARvec.size(); i++) {
        ss << " " << ARvec[i];
    }
    ss << " " << Capacity << " " << Duration << " " << source_node << " " << dest_AS << " " << dest_node;
    //callstring = ss.str();
    stringstream sss;
    sss << "Call_for_AS" << asnum;
    ofstream fout;
    fout.open(sss.str().c_str(), ios::app);
    //for(int i = 0; i < number_amount; i++)
    //fout << callstring << endl;
    //fout<<endl;
    fout.close();
    //for(int i = 0; i < callstring.length(); i++)
    //output_call[i] = callstring[i];

}


