/* 
 * File:   Graph.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:24 AM
 */

#ifndef GRAPH_H
#define	GRAPH_H


#pragma once

using namespace std;

class Path : public BasePath
{
public: 

	Path(const std::vector<BaseVertex*>& vertex_list, double weight):BasePath(vertex_list,weight){}

	// display the content
	void PrintOut(std::ostream& out_stream) const
	{
		out_stream << "Cost: " << m_dWeight << " Length: " << m_vtVertexList.size() << std::endl;
		for(std::vector<BaseVertex*>::const_iterator pos=m_vtVertexList.begin(); pos!=m_vtVertexList.end();++pos)
		{
			out_stream << (*pos)->getID() << " ";
		}
		out_stream << std::endl <<  "*********************************************" << std::endl;	
	}
};

class Graph
{

public: // members

	const static double DISCONNECT; 

	typedef set<BaseVertex*>::iterator VertexPtSetIterator;
	typedef map<BaseVertex*, set<BaseVertex*>*>::iterator BaseVertexPt2SetMapIterator;
        map<int, double> m_mpEdgeCodeBand_table;
        int AS_num;
        vector<Inter_AS_Links> InterASLinks_table;

protected: // members

	// Basic information
	map<BaseVertex*, set<BaseVertex*>*> m_mpFanoutVertices;
	map<BaseVertex*, set<BaseVertex*>*> m_mpFaninVertices;
	map<int, double> m_mpEdgeCodeWeight; 
        map<int, double> m_mpEdgeCodeBand;
        
	vector<BaseVertex*> m_vtVertices;
	int m_nEdgeNum;
	int m_nVertexNum;
        int windowSize;
        int windowExt;
        //a flag indicating whether this is bi-dircetional or uni-directional 0:bi;1:uni
        int direction;
	map<int, BaseVertex*> m_mpVertexIndex;

	// Members for graph modification
	set<int> m_stRemovedVertexIds;
	set<pair<int,int> > m_stRemovedEdge;

public:

	// Constructors and Destructor
	Graph(const string& file_name);
	Graph(const Graph& rGraph);
	~Graph(void);

    void testPrint();
	void clear();

	BaseVertex* get_vertex(int node_id);
	
	int get_edge_code(const BaseVertex* start_vertex_pt, const BaseVertex* end_vertex_pt) const;
	set<BaseVertex*>* get_vertex_set_pt(BaseVertex* vertex_, map<BaseVertex*, set<BaseVertex*>*>& vertex_container_index);

	double get_original_edge_weight(const BaseVertex* source, const BaseVertex* sink);

	double get_edge_weight(const BaseVertex* source, const BaseVertex* sink);
	void get_adjacent_vertices(BaseVertex* vertex, set<BaseVertex*>& vertex_set);
	void get_precedent_vertices(BaseVertex* vertex, set<BaseVertex*>& vertex_set);

	/// Methods for changing graph
	void remove_edge(const pair<int,int> edge)
	{
		m_stRemovedEdge.insert(edge);
	}

	void remove_vertex(const int vertex_id)
	{
		m_stRemovedVertexIds.insert(vertex_id);
	}

	void recover_removed_edges()
	{
		m_stRemovedEdge.clear();
	}

	void recover_removed_vertices()
	{
		m_stRemovedVertexIds.clear();
	}

	void recover_removed_edge(const pair<int,int> edge)
	{
		m_stRemovedEdge.erase(m_stRemovedEdge.find(edge));
	}

	void recover_removed_vertex(int vertex_id)
	{
		m_stRemovedVertexIds.erase(m_stRemovedVertexIds.find(vertex_id));
	}
        //method for setting the reservation window size


        void set_direction(int theDirection){direction = theDirection;}
private:
	void _import_from_file(const std::string& file_name);

};

#endif	/* GRAPH_H */

