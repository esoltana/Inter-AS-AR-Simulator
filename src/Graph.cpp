/* 
 * File:   Graph.cpp
 * Author: leo
 * 
 * Created on June 27, 2014, 12:24 AM
 */


#include <limits>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "GraphElements.h"
#include "IPCE.h"
#include "Graph.h"
#include <sstream>


using namespace std;

const double Graph::DISCONNECT = (numeric_limits<double>::max)();


Graph::Graph( const string& file_name)
{
	_import_from_file(file_name);
}

Graph::Graph( const Graph& graph )
{
	m_nVertexNum = graph.m_nVertexNum;
	m_nEdgeNum = graph.m_nEdgeNum;
	m_vtVertices.assign(graph.m_vtVertices.begin(),graph.m_vtVertices.end());
	m_mpFaninVertices.insert(graph.m_mpFaninVertices.begin(),graph.m_mpFaninVertices.end());
	m_mpFanoutVertices.insert(graph.m_mpFanoutVertices.begin(),graph.m_mpFanoutVertices.end());
	m_mpEdgeCodeWeight.insert(graph.m_mpEdgeCodeWeight.begin(),graph.m_mpEdgeCodeWeight.end());
        m_mpEdgeCodeBand.insert(graph.m_mpEdgeCodeBand.begin(),graph.m_mpEdgeCodeBand.end());
        m_mpEdgeCodeBand_table.insert(graph.m_mpEdgeCodeBand_table.begin(),graph.m_mpEdgeCodeBand_table.end());
	m_mpVertexIndex.insert(graph.m_mpVertexIndex.begin(),graph.m_mpVertexIndex.end());
}

Graph::~Graph(void)
{
	clear();
}


void Graph::_import_from_file( const string& input_file_name)
{
	const char* file_name = input_file_name.c_str();

	//1. Check the validity of the file
	ifstream ifs(file_name);
	if (!ifs)
	{
		cerr << "The file " << file_name << " can not be opened!" << endl;
		exit(1);
	}

	//2. Reset the members of the class
	clear();

	//3. Start to read information from the input file. 
	/// Note the format of the data in the graph file.
	//3.1 The first line has an integer as the number of vertices of the graph
        ifs >> AS_num;
	ifs >> m_nVertexNum;

	//3.2 In the following lines, each line contains a directed edge in the graph:
	///   the id of starting point, the id of ending point, the weight of the edge. 
	///   These values are separated by 'white space'. 
	int start_vertex, end_vertex;
	double edge_weight;
        double edge_band;
        string start_AS_vertex;
        string end_AS_vertex;
        int start_AS;
        int end_AS;
	int vertex_id = 0;
    int index_link = 0;
	while(ifs >> start_vertex && start_vertex != 0)
	{
		if (start_vertex == -1)
		{
			break;
		}
		ifs >> end_vertex;
		ifs >> edge_weight;
        ifs >> edge_band;

		///3.2.1 construct the vertices
		BaseVertex* start_vertex_pt = get_vertex(start_vertex);
		BaseVertex* end_vertex_pt = get_vertex(end_vertex);

		///3.2.2 add the edge weight
		//// note that the duplicate edge would overwrite the one occurring before. 
		m_mpEdgeCodeWeight[get_edge_code(start_vertex_pt, end_vertex_pt)] = edge_weight;
        m_mpEdgeCodeBand[get_edge_code(start_vertex_pt, end_vertex_pt)] = edge_band;
        //this table now contains
        m_mpEdgeCodeBand_table[get_edge_code(start_vertex_pt, end_vertex_pt)] = 0;
        m_mpEdgeCodeBand_table[get_edge_code(start_vertex_pt, end_vertex_pt)] = edge_band;
        
        //IPCEModule.intraASLinksAR[index_link].getBand(edge_band);
        //IPCEModule.intraASLinksAR[index_link].constructTable(windowSize,windowExt);
		index_link ++;
		///3.2.3 update the fan-in or fan-out variables
		//// Fan-in
		get_vertex_set_pt(end_vertex_pt, m_mpFaninVertices)->insert(start_vertex_pt);

		//// Fan-out
		get_vertex_set_pt(start_vertex_pt, m_mpFanoutVertices)->insert(end_vertex_pt);

	}	
	//read in the inter-domain links
	while(ifs >> start_AS_vertex)
	{
            int tmpindex = start_AS_vertex.find(":");
            start_AS = atoi(start_AS_vertex.substr(0,tmpindex).c_str());
	    start_vertex = atoi(start_AS_vertex.substr((tmpindex+1),start_AS_vertex.length()-tmpindex-1).c_str());
	    ifs >> end_AS_vertex;
	    tmpindex = end_AS_vertex.find(":");
            end_AS = atoi(end_AS_vertex.substr(0,tmpindex).c_str());
	    end_vertex = atoi(end_AS_vertex.substr((tmpindex+1),end_AS_vertex.length()-tmpindex-1).c_str());
	    ifs >> edge_weight;
	    ifs >> edge_band;
            Inter_AS_Links inter_link_entry;
            inter_link_entry.from_AS = start_AS;
            inter_link_entry.to_AS = end_AS;
            inter_link_entry.start_vertex = start_vertex;
            inter_link_entry.end_vertex = end_vertex;
            //if want to change the strategy of assigning bandwidth to a predefined mode:like all inter_domain links are 
            //64Gbps, please modify here
            inter_link_entry.band = edge_band;
           // inter_link_entry.theLink.constructTable(windowSize,windowExt);
           // InterASLinks_table.push_back(inter_link_entry);
	}

	if(m_nVertexNum != m_vtVertices.size())
	{
		cerr << "The number of nodes in the graph is "<<  m_vtVertices.size() << " instead of " << m_nVertexNum << endl;
		exit(1);
	}

	m_nVertexNum = m_vtVertices.size();
	m_nEdgeNum = m_mpEdgeCodeWeight.size();

	ifs.close();	
}

BaseVertex* Graph::get_vertex( int node_id )
{
	if (m_stRemovedVertexIds.find(node_id) != m_stRemovedVertexIds.end())
	{
		return NULL;
	}else
	{
		BaseVertex* vertex_pt = NULL;
		const map<int, BaseVertex*>::iterator pos = m_mpVertexIndex.find(node_id);
		if (pos == m_mpVertexIndex.end())
		{
			int vertex_id = m_vtVertices.size();
			vertex_pt = new BaseVertex();
			vertex_pt->setID(node_id);
			m_mpVertexIndex[node_id] = vertex_pt;

			m_vtVertices.push_back(vertex_pt);
		}else
		{
			vertex_pt = pos->second;
		}

		return vertex_pt;	
	}
}

void Graph::clear()
{
	m_nEdgeNum = 0;
	m_nVertexNum = 0;

	for(map<BaseVertex*, set<BaseVertex*>*>::const_iterator pos=m_mpFaninVertices.begin();
		pos!=m_mpFaninVertices.end(); ++pos)
	{
		delete pos->second;
	}
	m_mpFaninVertices.clear();

	for(map<BaseVertex*, set<BaseVertex*>*>::const_iterator pos=m_mpFanoutVertices.begin();
		pos!=m_mpFanoutVertices.end(); ++pos)
	{
		delete pos->second;
	}
	m_mpFanoutVertices.clear();


	m_mpEdgeCodeWeight.clear();

	//clear the list of vertices objects
	for_each(m_vtVertices.begin(), m_vtVertices.end(), DeleteFunc<BaseVertex>());
	m_vtVertices.clear();
	m_mpVertexIndex.clear();

	m_stRemovedVertexIds.clear();
	m_stRemovedEdge.clear();
}

int Graph::get_edge_code( const BaseVertex* start_vertex_pt, const BaseVertex* end_vertex_pt ) const
{
	/// Note that the computation below works only if 
	/// the result is smaller than the maximum of an integer!
	return start_vertex_pt->getID()*(m_nVertexNum+1)+end_vertex_pt->getID();
}


set<BaseVertex*>* Graph::get_vertex_set_pt( BaseVertex* vertex_, map<BaseVertex*, set<BaseVertex*>*>& vertex_container_index )
{
	BaseVertexPt2SetMapIterator pos = vertex_container_index.find(vertex_);

	if(pos == vertex_container_index.end())
	{
		set<BaseVertex*>* vertex_set = new set<BaseVertex*>();
		pair<BaseVertexPt2SetMapIterator,bool> ins_pos = 
			vertex_container_index.insert(make_pair(vertex_, vertex_set));

		pos = ins_pos.first;
	}

	return pos->second;
}


double Graph::get_edge_weight( const BaseVertex* source, const BaseVertex* sink )
{
	int source_id = source->getID();
	int sink_id = sink->getID();

	if (m_stRemovedVertexIds.find(source_id) != m_stRemovedVertexIds.end()
		|| m_stRemovedVertexIds.find(sink_id) != m_stRemovedVertexIds.end()
		|| m_stRemovedEdge.find(make_pair(source_id, sink_id)) != m_stRemovedEdge.end())
	{
		return DISCONNECT;
	}else
	{
		return get_original_edge_weight(source, sink);
	}
}


void Graph::get_adjacent_vertices( BaseVertex* vertex, set<BaseVertex*>& vertex_set )
{
	int starting_vt_id = vertex->getID();

	if (m_stRemovedVertexIds.find(starting_vt_id) == m_stRemovedVertexIds.end())
	{
		set<BaseVertex*>* vertex_pt_set = get_vertex_set_pt(vertex, m_mpFanoutVertices);
		for(set<BaseVertex*>::const_iterator pos=(*vertex_pt_set).begin();
			pos != (*vertex_pt_set).end(); ++pos)
		{
			int ending_vt_id = (*pos)->getID();
			if (m_stRemovedVertexIds.find(ending_vt_id) != m_stRemovedVertexIds.end()
				|| m_stRemovedEdge.find(make_pair(starting_vt_id, ending_vt_id)) != m_stRemovedEdge.end())
			{
				continue;
			}
			//
			vertex_set.insert(*pos);
		}
	}
}

void Graph::get_precedent_vertices( BaseVertex* vertex, set<BaseVertex*>& vertex_set )
{
	if (m_stRemovedVertexIds.find(vertex->getID()) == m_stRemovedVertexIds.end())
	{
		int ending_vt_id = vertex->getID();
		set<BaseVertex*>* pre_vertex_set = get_vertex_set_pt(vertex, m_mpFaninVertices);
		for(set<BaseVertex*>::const_iterator pos=(*pre_vertex_set).begin(); 
			pos != (*pre_vertex_set).end(); ++pos)
		{
			int starting_vt_id = (*pos)->getID();
			if (m_stRemovedVertexIds.find(starting_vt_id) != m_stRemovedVertexIds.end()
				|| m_stRemovedEdge.find(make_pair(starting_vt_id, ending_vt_id)) != m_stRemovedEdge.end())
			{
				continue;
			}
			//
			vertex_set.insert(*pos);
		}
	}
}

double Graph::get_original_edge_weight( const BaseVertex* source, const BaseVertex* sink )
{
	map<int, double>::const_iterator pos = 
		m_mpEdgeCodeWeight.find(get_edge_code(source, sink));

	if (pos != m_mpEdgeCodeWeight.end())
	{
		return pos->second;
	}else
	{
		return DISCONNECT;
	}
}
void Graph::testPrint()
{
/*
    //ofstream testTableFile;
    //testTableFile.open("TestTable.txt");
    ofstream testfile;
	stringstream outputfilename;
	outputfilename << "output" << AS_num;
	testfile.open(outputfilename.str().c_str());
    testfile<<"This is AS "<< AS_num<<", the Intra AS links are:"<<endl;
     for(map<int,EdgeTable>::const_iterator it = m_mpEdgeCodeBand_table.begin(); it != m_mpEdgeCodeBand_table.end(); it++)
    {
        EdgeTable value = it->second;
        int code = it->first;
        int firstV = code / (m_nVertexNum+1);
        int secondV = code % (m_nVertexNum+1);
        testfile<<"From:"<<firstV<<" to "<<secondV<<":"<<endl;
        testfile<<value.reservationTable.size()<<endl;
        for(int i = 0; i < value.reservationTable.size(); i++)
        {
            testfile << value.reservationTable[i]<< " ";
        }
        testfile<<endl;
    }
    testfile<<"The Inter AS links are:"<<endl;
    for(int i = 0; i < InterASLinks_table.size(); i++)
    {
        testfile<<"From AS "<<InterASLinks_table[i].from_AS<<" vertex "<<InterASLinks_table[i].start_vertex<<" to "<<InterASLinks_table[i].to_AS<<" vertex "<<InterASLinks_table[i].end_vertex<<endl;
        EdgeTable value = InterASLinks_table[i].theLink;
        testfile<<value.reservationTable.size()<<endl;
        for(int i = 0; i < value.reservationTable.size(); i++)
        {
            testfile << value.reservationTable[i]<< " ";
        }
        testfile<<endl;
        
    }
    
    
  */  
    
   
}


