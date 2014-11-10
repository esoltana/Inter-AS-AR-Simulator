#include <string>
#include <vector>

using namespace std;

class CallGenerator{
public:
    CallGenerator(int AS_ID,int window_size_sec, int lead_time, int slotlength);
    void readNodeVector(int nodeNumArray[],  int arraySize);
    void mapNode(int index);
    int mapIndex(int AS_num, int vertex_num);
    void generateCall(double arr);
    void readCommonFile(string path);
    void readprobMatrix(string path);

    /*call generation related parameters */
    double arrival_rate;
    double arrival_time;
    int arrival_timeslot;
    
      //used for call generating funciton
    int source_node, dest_AS, dest_node;
    
    //vector of start time options.
    vector<int> ARvec;
    //used in generating call for EST type
    int Duration;
    //keep the rate of USST and EST 
    double Capacity;
    
    int isUSST;
    
private:
    //the ID of the AS that this call generator belongs to
    int asID;
    //type of prob matrix
    int prob_matrix_type;
    vector<int> nodevec;  // vector of number of nodes in each AS
    //total number of nodes in the network
    int total_node;
    //matrix to keep src-dest probabilities
    double probMatrix[75][75];
    
    int windowsizeTimeslot;
    int leadtime;
    int slot_length;
   
    
    //read from the file (rate value)
    double USSTcap, ESTcap;
    
    int USSTduration, ESTduration_min, ESTduration_max;
    //read number of AR_options from the file
    int USSTn, ESTn;
    double zipf_alpha;
    //read call fraction from the file
    double USST_EST_prob[2];

    double USST_EST[2];

};

