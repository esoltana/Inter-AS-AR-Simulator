/* 
 * File:   ARBGP.cpp
 * Author: leo
 * 
 * Created on June 27, 2014, 12:17 AM
 */

#include "ARBGP.h"
#include "IPCE.h"
//#include "ARserver.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <queue>


#define TIMETAG 1
#define BGPTAG 2
#define CALLTAG 3

/*
 * 
 */

/*--------------------------
update message format:
AS_num1 vertex_num1 timeslot_start timeslot_end AS_path BR MED nextnode...
--------------------------*/
ARBGP::ARBGP() {

}
/*
 * constructor
 * Input: asnum: the AS number of this AS, num_of_vertices: the number of nodes for this AS; MCN stands for MultiConnected Neighbor
 */
ARBGP::ARBGP(int asnum, int num_of_vertices) {
    AS_num = asnum;
    for (int i = 1; i <= num_of_vertices; i++) {
        Node tmp = Node(asnum, i);
        NLRI tmpNLRI;
        tmpNLRI.AS_num = AS_num;
        tmpNLRI.node_num = i;
        //tmpNLRI.BR_of_this_AS = 0;
        //tmpNLRI.BR_of_MCN = 0;
        tmpNLRI.as_path.push_back(AS_num);
        vector<NLRI> tmp_entry;
        tmp_entry.push_back(tmpNLRI);
        RIB[tmp] = tmp_entry;
        //RIB_out.push_back(tmpNLRI);
    }
}
/*
 * This function creates NLRI entries advising all neighbors about nodes it can reach.
 */

void ARBGP::ARBGPinit() {
    for (int i = 1; i <= num_vertices; i++) {
        Node tmp = Node(AS_num, i);
        NLRI tmpNLRI;
        tmpNLRI.AS_num = AS_num;
        tmpNLRI.node_num = i;
        //tmpNLRI.BR_of_this_AS = 0;
        //tmpNLRI.BR_of_MCN = 0;
        tmpNLRI.as_path.push_back(AS_num);
        //RIB[tmp].push_back(tmpNLRI);
        //RIB_out.push_back(tmpNLRI);
    }
}
/*
 * Input: as_num: The AS number of the added neighbor AS; egress_ingress: a flag indicating whether this is a egress link or ingress link;
 * node1 and node2: 
 * if it's an egress link, then the direction of the link is from node1(within this AS) to node2(neighbor AS)
 * if it's an ingress link, then the direction of the link is from node1(neighbor AS) to node2(within this AS)
 * Description: Add a neighbor into MCNlist with the inter AS link; for example: 
 * if the current AS is 1 and an egress inter-AS link to AS 3 from node 1:2 to 3:7, then we could use addMCNAccess(3,1,2,7)
 */ 
void ARBGP::addMCNAccess(int as_num, int egress_ingress, int node1, int node2) {
    MCNlist[as_num].AS_ID = as_num;
    // egress
    if (egress_ingress == 1) {
        MCNlist[as_num].egress_links.push_back(make_pair(node1, node2));
    }//ingress
    else {
        MCNlist[as_num].ingress_links.push_back(make_pair(node1, node2));
    }
}
/*
 * function description: generate a 2D vector BRlist (Border Routers List) from MCNlist 
 * each row is a 1D vector of BRs to a specific neighbor for example, 
 * if the current AS is one and the row corresponds to neighbor AS 3 is : {2,3}; also the index will be pushed into MCN_IDs.
 *                                                           
 */

void ARBGP::getMCNList() {
    for (std::map<int, MCN>::iterator iter = MCNlist.begin(); iter != MCNlist.end(); ++iter) {
        int asnum = iter->first;
        MCN_IDs.push_back(asnum);
        vector<int> tmp_BR;
        for (int i = 0; i < iter->second.egress_links.size(); i++) {
            int tmp_BR_num = iter->second.egress_links[i].first;
            tmp_BR.push_back(tmp_BR_num);
        }
        BRlist.push_back(tmp_BR);

    }
    /*
    cout <<"BRlist for AS "<<AS_num<<":"<<endl;
    for(int i = 0; i < BRlist.size(); i++)
    {
        cout <<"to AS " << MCNlist[i].as_num
    }
     */
}
/*This function is for MPI. you can decide whether to keep this function or not*/
void ARBGP::getMCNProcessNumber(int offset) {
    for (int i = 0; i < MCN_IDs.size(); i++)
        MCN_process_num.push_back(MCN_IDs[i] + offset);
}

/*
 * This function 
 */

void ARBGP::constructUpdate(int flag) {


    if (flag == 0) {
        RIB_out.clear();
        /*
                for(int i = 1; i <= num_vertices; i++)
                {
                        Node tmp(as_num,i);
                        NLRI tmp_NLRI;
                        tmp_NLRI.AS_num = as_num;
                        tmp_NLRI.node_num = i;
			
                        RIB_out[tmp] = 
                }
         */
        //cout<<"AS:"<<AS_num<<":"<<num_vertices<<endl;
        for (int i = 0; i < MCN_IDs.size(); i++) {
            vector<NLRI> tmp_entry;
            for (int k = 0; k < BRlist[i].size(); k++) {

                for (int j = 1; j <= num_vertices; j++) {
                    NLRI tmp_NLRI;
                    tmp_NLRI.AS_num = AS_num;
                    tmp_NLRI.node_num = j;
                    //this is the lead time(5 in this case for test only)
                    tmp_NLRI.timeSlotsVector.push_back(5);
                    //this is the window size for test only
                    tmp_NLRI.timeSlotsVector.push_back(1440);
                    tmp_NLRI.as_path.push_back(AS_num);
                    tmp_NLRI.BR_of_this_AS = 0;
                    //test only(should read the common file)
                    tmp_NLRI.med = 50;
                    tmp_entry.push_back(tmp_NLRI);
                }

            }
            RIB_out[MCN_IDs[i]] = tmp_entry;
        }
    } else if (flag == 1) {
        stringstream ss;
        for (int i = 0; i < RIB_out.size(); i++) {
            //ss << RIB_out[i].AS_num << RIB_out[i].node_num << RIB_out[i].timeSlotsVector[0] << RIB_out[i].timeSlotsVector[1] << RIB_out[i].
        }
    }
    //RIB_out.clear();
}

/*
void ARBGP::sendUpdate(double send_time, priority_queue<ARBGP_Node, vector<ARBGP_Node>, MyComparatorARBGP>& ARBGP_Q, DelayStruc DFile, int recvwindow, IPCE& IPCE_M) {

    for (std::map<int, vector<NLRI> >::iterator iter = RIB_out.begin(); iter != RIB_out.end(); ++iter) {
        int MCN_id = iter->first;
        double thedelay = DFile.getDelay(AS_num, MCN_id);
        ARBGP_Node tmp(AS_num, MCN_id, send_time, thedelay);
        //process the vector of NLRI
        int MCN_index = 0;
        //find the corresponding index of the MCN in the MCN_IDs so that we can retrieve the BR in BRlist
        while (MCN_IDs[MCN_index] != MCN_id)
            MCN_index++;
        for (int k = 0; k < iter->second.size(); k++) {
            NLRI tmpB(iter->second[k]);
            for (int i = 0; i < MCNlist[MCN_id].egress_links.size(); i++) {
                int curr = 0;
                int opening = 1;
                int close = 0;
                //find = true: find opening, find = false:find close
                bool find = true;
                while (curr < recvwindow) {
                    bool flag = IPCE_M.findsinglePathR(tmpB.BR_of_this_AS, MCNlist[MCN_id].egress_links[i].first, threshold_capacity, curr, curr + 1);
                    if(!flag)
                    cout<<"the flag:"<<flag<<" the find:"<<find<< " curr:"<<curr<<endl;
                    if (find && !flag) {
                        curr++;
                        continue;
                    } else if (find && flag) {
                        //found the opening
                        find = false;
                        opening = curr + 1;
                        curr++;
                        continue;
                    } else if (!find && !flag) {
                        //found the close

                        close = curr + 1;
                        curr++;
                        NLRI tmp_NLRI(tmpB);
                        tmp_NLRI.BR_of_this_AS = MCNlist[MCN_id].egress_links[i].second;
                        tmp_NLRI.timeSlotsVector.clear();
                        tmp_NLRI.timeSlotsVector.push_back(opening);
                        tmp_NLRI.timeSlotsVector.push_back(close);
                        tmp.NLRI_vector.push_back(tmp_NLRI);
                        //cout<<"send from "<<AS_num<<" to "<< MCN_id << tmp_NLRI.toString()<<endl;
                        curr++;
                        find = true;
                        continue;
                    }
                    else
                        //finding the close
                    curr++;
                }
                if (IPCE_M.findsinglePathR(tmpB.BR_of_this_AS, MCNlist[MCN_id].egress_links[i].first, threshold_capacity, curr, curr + 1)) {
                    close = recvwindow;
                    NLRI tmp_NLRI(tmpB);
                    tmp_NLRI.BR_of_this_AS = MCNlist[MCN_id].egress_links[i].second;
                    tmp_NLRI.timeSlotsVector.clear();
                    tmp_NLRI.timeSlotsVector.push_back(opening);
                    tmp_NLRI.timeSlotsVector.push_back(close);
                    tmp.NLRI_vector.push_back(tmp_NLRI);
                    //cout<<"send from "<<AS_num<<" to "<< MCN_id<<":" << tmp_NLRI.toString()<<endl;
                }
            }
        }




        ARBGP_Q.push(tmp);
        cout << "A new ARBGP update message added into the queue! count:" << ARBGP_Q.size() << endl;
        // cout<<"first:"<<ARBGP_Q.top().to_AS<<endl;

    }

    RIB_out.clear();
}
*/

void ARBGP::recvUpdate(int incomingAS, vector<NLRI> NLRI_vector) {
    for (int i = 0; i < NLRI_vector.size(); i++) {
        parseUpdateMessage(NLRI_vector[i]);
    }
}

void ARBGP::parseUpdateMessage(NLRI new_NLRI) {
    int tmp_AS, tmp_vertex, slot_start, slot_end, BR, MED;
    string str_AS;
    string tmp_ASpath;
    bool flag = true;
    bool loop_flag = true;
    bool add_new = false;
    int incoming_AS;
    flag = true;
    loop_flag = false;
    add_new = false;

    //make sure there are no loops in the AS path
    istringstream ASP(tmp_ASpath);
    for (int i = 0; i < new_NLRI.as_path.size(); i++) {
        if (new_NLRI.as_path.size() == AS_num) {
            //loop
            return;
        }
    }
    Node tmp_node = Node(new_NLRI.AS_num, new_NLRI.node_num);
    int t_AS;
    NLRI tmp_NLRI;
    tmp_NLRI.AS_num = new_NLRI.AS_num;
    tmp_NLRI.node_num = new_NLRI.node_num;
    tmp_NLRI.BR_of_this_AS = new_NLRI.BR_of_this_AS;
    tmp_NLRI.med = new_NLRI.med;
    istringstream sas(tmp_ASpath);
    for (int i = 0; i < new_NLRI.as_path.size(); i++)
        tmp_NLRI.as_path.push_back(new_NLRI.as_path[i]);
    incoming_AS = tmp_NLRI.as_path[tmp_NLRI.as_path.size() - 1];
    tmp_NLRI.as_path.push_back(AS_num);
    tmp_NLRI.timeSlotsVector.push_back(new_NLRI.timeSlotsVector[0]);
    tmp_NLRI.timeSlotsVector.push_back(new_NLRI.timeSlotsVector[1]);
    bool theflag = true;
    for (map<Node, vector<NLRI>, NodeCompare >::iterator iter = RIB.begin(); iter != RIB.end(); iter++) {
        if (iter->first.as_num == tmp_NLRI.AS_num && iter->first.vertex_num == tmp_NLRI.node_num) {
            theflag = false;
            break;
        }
    }
    if (theflag) {
        //This is a new entry

        //add this new entry in RIB as well as RIB_out
        vector<NLRI> thetmp;
        thetmp.push_back(tmp_NLRI);
        RIB[tmp_node] = thetmp;
        //no intra_algo is implemented here. it will just send NLRI to every MCNs
        for (int i = 0; i < MCN_IDs.size(); i++) {
            if (MCN_IDs[i] != incoming_AS) {
                if (RIB_out.find(MCN_IDs[i]) != RIB_out.end()) {
                    RIB_out[MCN_IDs[i]].push_back(tmp_NLRI);
                } else {
                    vector<NLRI> tmp;
                    RIB_out[MCN_IDs[i]] = tmp;
                    RIB_out[MCN_IDs[i]].push_back(tmp_NLRI);
                }
            }
        }
    } else {

        //This is an existing entry, check whether this is an update
        //cout<<"size!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<RIB[tmp_node].size()<<endl;
        for (int i = 0; i < RIB[tmp_node].size(); i++) {
            //if it's the same NLRI, just terminate
            if (sameNLRI(RIB[tmp_node][i], tmp_NLRI))
                return;
            //if the two time range is overlapped, then delete the old NLRI and use the new NLRI 
            if (checkNLRI(RIB[tmp_node][i], tmp_NLRI)) {
                //delete the old one
                RIB[tmp_node].erase(RIB[tmp_node].begin() + i);
                //add_new = true;
            }
        }
        if (true) {
            for (int i = 0; i < MCN_IDs.size(); i++) {
                if (MCN_IDs[i] != incoming_AS) {
                    if (RIB_out.find(MCN_IDs[i]) != RIB_out.end()) {
                        RIB_out[MCN_IDs[i]].push_back(tmp_NLRI);
                    } else {
                        vector<NLRI> tmp;
                        RIB_out[MCN_IDs[i]] = tmp;
                        RIB_out[MCN_IDs[i]].push_back(tmp_NLRI);
                    }
                }
            }
            RIB[tmp_node].push_back(tmp_NLRI);
        }
        //cout<<"size after adding!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<RIB[tmp_node].size()<<endl;
    }


}

bool ARBGP::sameNLRI(NLRI n1, NLRI n2) {
    //did not implement the overlap of time range yet
    if (n1.AS_num == n2.AS_num && n1.node_num == n2.node_num && n1.BR_of_this_AS == n2.BR_of_this_AS && n1.as_path == n2.as_path && (n1.timeSlotsVector[0] != n2.timeSlotsVector[0] || n1.timeSlotsVector[1] != n2.timeSlotsVector[1])) {
        return true;
    }
    return false;
}

bool ARBGP::checkNLRI(NLRI n1, NLRI n2) {
    //did not implement the overlap of time range yet
    if (n1.AS_num == n2.AS_num && n1.node_num == n2.node_num && n1.BR_of_this_AS == n2.BR_of_this_AS && n1.as_path == n2.as_path) {
        return true;
    }
    return false;
}



