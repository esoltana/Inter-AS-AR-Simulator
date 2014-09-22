#include <string>
#include <vector>

using namespace std;

class CallGenerator {
public:
    CallGenerator(int as_num);
    void readNodeVector(int nodeNumArray[]);
    void mapNode(int index);
    int mapIndex(int AS_num, int vertex_num);
    void generateCall(double arr, int windowsize, int leadtime,      int slot_length , int flag);
    void readCommonFile(string path);
    void readprobMatrix(string path);

    /*call generation related parameters */
    double arrival_rate;
    double arrival_time;
    int source_node, dest_AS, dest_node;
    vector<int> ARvec;
    int Duration;
    double Capacity;
    
private:
    int asnum;
    vector<int> nodevec;  // vector of number of nodes in each AS
    int total_node;
    double USSTcap, ESTcap;
    int USSTduration, ESTduration_min, ESTduration_max;
    int USSTn, ESTn;
    double zipf_alpha;
    double probMatrix[75][75];
    double USST_EST_prob[2];
    double USST_EST[2];

};

