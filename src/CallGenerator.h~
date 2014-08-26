#include <string>
#include <vector>

using namespace std;

class CallGenerator {
public:
    CallGenerator(int as_num);
    void readNodeVector(string dir);
    void mapNode(int index);
    int mapIndex(int AS_num, int vertex_num);
    void generateCall(double arr, int windowsize, int leadtime,      int slot_length , int flag);
    void readCommonFile(string dir);
    void readMatrix(string dir);

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
    double theMatrix[75][75];
    double USST_EST_prob[2];
    double USST_EST[2];

};

