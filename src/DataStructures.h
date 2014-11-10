/* 
 * File:   GraphElements.h
 * Author: leo
 *
 * Created on June 26, 2014, 9:10 PM
 */



#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <float.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// A class for the object deletion
//////////////////////////////////////////////////////////////////////////

template<class T>
class DeleteFunc {
public:

    void operator()(const T* it) const {
        delete it;
    }
};
//TODO: I don't know why need this class, check if it's used
class BaseVertex {
    int m_nID;
    double m_dWeight;

public:

    int getID() const {
        return m_nID;
    }

    void setID(int ID_) {
        m_nID = ID_;
    }

    double Weight() const {
        return m_dWeight;
    }

    void Weight(double val) {
        m_dWeight = val;
    }

    void PrintOut(std::ostream& out_stream) {
        out_stream << m_nID;
    }
};

class linkAvailableBandwithTable {
public:
    std::vector<double> availableBandwidthTable;
    double bandwidth;
    int weight;
    void setBandandWeight(double band, int w) { 
        bandwidth = band;
        weight=w;
    }
    void constructResTable(int tableSize, int leadTime) {
        //TODO: make sure that we need to add leadTime or not
        int totalSize = tableSize;
        for (int i = 1; i <= totalSize; i++)
            availableBandwidthTable.push_back(bandwidth);
        
    }
    //return the first bandwidth value in the reservation table, and push back a new bandwidth value.
    double signaling() {
        
        double theReturnBand = availableBandwidthTable.front();
        availableBandwidthTable.erase(availableBandwidthTable.begin());
        availableBandwidthTable.push_back(bandwidth);
        return theReturnBand;
    }
    void printResTable()  {
        for(int i = 0; i < availableBandwidthTable.size(); i++)
        {
            cout<<availableBandwidthTable[i]<<" ";
        }
        cout<<endl;
    }
};

class Inter_AS_Links {
public:
    int start_AS;
    int end_AS;
    int start_node;
    int end_node;
    double bandwidth;
    double weight;
};

typedef struct {
    int start_node, end_node;
    double bandwidth;
    int weight;
} Intra_Link;

class TOPOLOGY {
public:
    vector<Intra_Link> Intra_Links_table;
    vector<Inter_AS_Links> InterASLinks_table;
};

class MCN {
public:
    int AS_ID;
    vector<pair<int, int> > egress_links;
    vector<pair<int, int> > ingress_links;
};

class Node {
public:
    int as_num;
    int vertex_num;

    Node(int as, int vertex) {
        as_num = as;
        vertex_num = vertex;
    }
};

struct NodeCompare {

    bool operator() (const Node& lhs, const Node& rhs) {
        return (lhs.as_num * (lhs.vertex_num + rhs.vertex_num) + lhs.vertex_num)< (rhs.as_num * (rhs.vertex_num + lhs.vertex_num) + rhs.vertex_num);
    }
};

class Route {
public:
    Node ORIGIN;
    vector<int> AS_PATH;
    Node NEXT_HOP;

};

class AdjRibsOut {
public:
    int BR_num;
    map <int, Route> RouteList;
};

class NLRI {
public:
    int AS_num;
    int node_num;
    int BR_of_this_AS;
    int med;
    //int BR_of_MCN;
    vector <int> timeSlotsVector;
    vector<int> as_path;
    NLRI()
    {}
    NLRI(const NLRI& copied)
    {
        AS_num = copied.AS_num;
        node_num = copied.node_num;
        BR_of_this_AS = copied.BR_of_this_AS;
        med = copied.med;
        for(int i = 0; i < copied.timeSlotsVector.size(); i++)
            timeSlotsVector.push_back(copied.timeSlotsVector[i]);
        for(int i = 0; i < copied.as_path.size(); i++)
            as_path.push_back(copied.as_path[i]);
    }

    string toString() const {
        stringstream ss;
        ss << AS_num << " " << node_num << " " << timeSlotsVector[0] << " " << timeSlotsVector[1] << " ";
        for (int i = 0; i < as_path.size() - 1; i++)
            ss << as_path[i] << ",";
        ss << as_path[as_path.size() - 1] << " ";
        ss << BR_of_this_AS << " ";
        ss << med;
        return ss.str();
    }
};

class BasePath {
protected:

    int m_nLength;
    double m_dWeight;
    std::vector<BaseVertex*> m_vtVertexList;

public:

    BasePath(const std::vector<BaseVertex*>& vertex_list, double weight)
    : m_dWeight(weight) {
        m_vtVertexList.assign(vertex_list.begin(), vertex_list.end());
        m_nLength = m_vtVertexList.size();
    }

    ~BasePath(void) {
    }

    double Weight() const {
        return m_dWeight;
    }

    void Weight(double val) {
        m_dWeight = val;
    }

    int length() {
        return m_nLength;
    }

    BaseVertex* GetVertex(int i) {
        return m_vtVertexList.at(i);
    }

    bool SubPath(std::vector<BaseVertex*>& sub_path, BaseVertex* ending_vertex_pt) {

        for (std::vector<BaseVertex*>::const_iterator pos = m_vtVertexList.begin();
                pos != m_vtVertexList.end(); ++pos) {
            if (*pos != ending_vertex_pt) {
                sub_path.push_back(*pos);
            } else {
                //break;
                return true;
            }
        }

        return false;
    }

    // display the content

    void PrintOut(std::ostream& out_stream) const {
        out_stream << "Cost: " << m_dWeight << " Length: " << m_vtVertexList.size() << std::endl;
        for (std::vector<BaseVertex*>::const_iterator pos = m_vtVertexList.begin(); pos != m_vtVertexList.end(); ++pos) {
            (*pos)->PrintOut(out_stream);
            out_stream << "->";
        }
        out_stream << std::endl << "*********************************************" << std::endl;
    }

    std::vector<BaseVertex*> ReturnPath() {
        return m_vtVertexList;
    }
};

////////////implement the ARBGP queue
struct ARBGP_Node {
    int from_AS;
    int to_AS ;
    double send_time;
    double total_delay;
    double do_update_time;
    vector<NLRI> NLRI_vector;
    ARBGP_Node * next_node;
    ARBGP_Node()
    {
    next_node = NULL;
    }
    ARBGP_Node(int f_AS, int t_AS,double s_time, double t_delay, vector<NLRI> N_vector)
    {
        from_AS = f_AS;
        to_AS = t_AS;
        send_time = s_time;
        total_delay = t_delay;
        do_update_time = s_time + t_delay;
        for(int i = 0 ; i < N_vector.size(); i++)
        {
            NLRI tmp(N_vector[i]);
            NLRI_vector.push_back(tmp);
        }
        next_node = NULL;
    }
     ARBGP_Node(int f_AS, int t_AS,double s_time, double t_delay)
    {
        from_AS = f_AS;
        to_AS = t_AS;
        send_time = s_time;
        total_delay = t_delay;
        do_update_time = s_time + t_delay;
        next_node = NULL;
    }
};
struct MyComparatorARBGP {
  bool operator() (ARBGP_Node arg1, ARBGP_Node arg2) {
    return arg1.do_update_time >= arg2.do_update_time; //calls your operator
  }
};

struct ARBGP_head_Node {
    int count;
    ARBGP_Node  h ;
};

class ARBGP_queue {
public:
    ARBGP_head_Node hn;
    double earliest_time;

    ARBGP_queue() {
        hn.count = 0;
        earliest_time = DBL_MAX;
    }

    ~ARBGP_queue() {

    }

    void addNode(int fromAS, int toAS, double sendTime, double totalDelay, vector<NLRI> NLRI_vector) {
        if(hn.h.next_node != NULL)
        cout<<"top:"<<hn.h.next_node->to_AS<<endl;
        hn.count++;
        ARBGP_Node * curr = &(hn.h);
        ARBGP_Node  tmp(fromAS,toAS,sendTime,totalDelay,NLRI_vector);
        while (curr->next_node != NULL) {
            //cout<<"top:"<<hn.h.next_node->to_AS<<endl;
            if (tmp.do_update_time <= curr->next_node->do_update_time) {
                tmp.next_node = curr->next_node;
                curr->next_node = &tmp;
                earliest_time = hn.h.next_node->do_update_time;
                
                return;
            }
            curr->next_node = curr->next_node->next_node;
        }
        curr->next_node = &tmp;
        tmp.next_node = NULL;
        cout<<"added:"<<hn.h.next_node->to_AS<<endl;
        earliest_time = hn.h.next_node->do_update_time;
        
    }
 

    ARBGP_Node * popNode() {
        ARBGP_Node * tmp;
        //cout<<"should poped:"<<hn.h.next_node->to_AS<<endl;
        tmp = hn.h.next_node;
        if (hn.h.next_node != NULL) {
            hn.h.next_node = hn.h.next_node->next_node;
            tmp->next_node = NULL;
            hn.count--;
        }
        if (hn.count != 0)
            earliest_time = hn.h.next_node->do_update_time;
        else
            earliest_time = DBL_MAX;
        //cout<<"poped:"<<tmp->to_AS<<endl;
        return tmp;
    }

private:
    

};
///////////---------------------------

////////////implement the ARScheduling queue
struct Inter_AS_Call_Node {
    int from_AS;
    int to_AS;
    int from_node;
    int to_node;
    
    double send_time_in_sec;
    double prop_delay;
    double arrival_instance_in_sec;
    vector<int> AS_path;
    vector<int> AR_vector;
    int next_AS;
    double capacity;
    int duration;
    Inter_AS_Call_Node * next_node = NULL;
        Inter_AS_Call_Node()
    {}
    Inter_AS_Call_Node(int f_AS, int f_node, int t_AS, int t_node, double s_time, double t_delay,double Capacity, int Duration,int artime, vector<int> AS_P,vector<int>ARvec)
    {
        from_AS = f_AS;
        to_AS = t_AS;
        from_node = f_node;
        to_node = t_node;
        send_time_in_sec = s_time;
        prop_delay = t_delay;
        arrival_instance_in_sec = s_time + t_delay;
        capacity = Capacity;
        duration = Duration;
        for(int i = 0 ; i < AS_P.size(); i++)
        {
            AS_path.push_back(AS_P[i]);
        }
        for(int i = 0; i < ARvec.size(); i++)
        {
            AR_vector.push_back(ARvec[i]);
        }
    }
    
};

struct MyComparatorInter_AS_Call {
  bool operator() (Inter_AS_Call_Node arg1, Inter_AS_Call_Node arg2) {
    return arg1.arrival_instance_in_sec >= arg2.arrival_instance_in_sec; //calls your operator
  }
};

struct Inter_AS_Call_head_Node {
    int count;
    Inter_AS_Call_Node * h ;
};

class Inter_AS_Call_queue {
public:
    double earliest_time;

    Inter_AS_Call_queue() {
        hn.count = 0;
        earliest_time = DBL_MAX;
    }


    void addNode(int fromAS,int fromNode, int toAS,int toNode, double sendTime, double totalDelay,double capacity, int duration,int artime, vector<int> AS_path, vector<int> ARvec) {
        hn.count++;
        Inter_AS_Call_Node * curr = hn.h;
        Inter_AS_Call_Node tmp(fromAS,fromNode,toAS, toNode,sendTime,totalDelay,capacity,duration,artime,AS_path,ARvec);
        while (curr->next_node != NULL) {
            if (tmp.arrival_instance_in_sec >= curr->next_node->arrival_instance_in_sec) {
                tmp.next_node = curr->next_node;
                curr->next_node = &tmp;
                earliest_time = tmp.arrival_instance_in_sec;
                return;
            }
        }
        curr->next_node = &tmp;
        tmp.next_node = NULL;
        earliest_time = tmp.arrival_instance_in_sec;
    }

    Inter_AS_Call_Node* popNode() {
        Inter_AS_Call_Node* tmp;
        if(hn.h->next_node == NULL)
        tmp = NULL;
        else {
            hn.h->next_node = hn.h->next_node->next_node;
            tmp->next_node = NULL;
            hn.count--;
        }
        if (hn.count != 0)
            earliest_time = hn.h->next_node->arrival_instance_in_sec;
        else
            earliest_time = DBL_MAX;
        return tmp;
    }

private:
    Inter_AS_Call_head_Node hn;

};
/////////////////////////

//the data structure that stores the delays between ASes
class DelayStruc {
public:

    DelayStruc(int num_of_AS) {
      n_of_AS = num_of_AS;
    }

    bool addDelay(int from_AS, int to_AS, double theDelay) {
        int theindex = from_AS * (n_of_AS + 1) + to_AS;
        if (delayVector.find(theindex) != delayVector.end()) {
            cout << "this delay alread exist! Use change instead" << endl;
            return false;
        } else {
            delayVector[theindex] = theDelay;
            return true;
        }
    }

    double getDelay(int from_AS, int to_AS) {
        int theindex = from_AS * (n_of_AS + 1) + to_AS;
        if (delayVector.find(theindex) == delayVector.end())
            return DBL_MAX;
        else
            return delayVector[theindex];
    }
private:
    int n_of_AS;
    map<int, double> delayVector;

};
//
//////call data structure
struct Call_Node{
    double arrival_instant_in_sec;
    int arrival_instant_in_TS;
    vector<int> AR_vec;
    int from_AS;
    int from_node;
    int to_AS;
    int to_node;
    int duration;
    double capacity;
    int isUSST;
};

struct MyComparatorCALL {
  bool operator() (Call_Node arg1, Call_Node arg2) {
    return arg1.arrival_instant_in_sec >= arg2.arrival_instant_in_sec; //calls your operator
  }
};

/////////
