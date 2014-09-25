/* 
 * File:   main.cpp
 * Author: Elahe
 *
 * Created on September 7, 2014
 */
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <limits>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <string>
#include <vector>
#include <cstdlib>
#include "CallGenerator.h"
#include "randgen.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
    //assign the input variables
    //the AS number that this call generator belongs to
    int ASnum = 1;
    //the rate of arriving calls
    double arrival_rate = 50;
    
    int number_of_call_generated = 1000;
    
    // the period of time slots
    int single_slot_time = 60;	
    
    //the name of output file to save the generated calls
    string output_file_name = "Output_CallGen_IntraDomainCalls";
    
    //the whole period of start time options
    int AR_whole_time = 24 * 60 * 60 / single_slot_time;
    
    //
    int lead_time = 60/single_slot_time*3;
	
    //generate an instance of callgenerator class
    CallGenerator testcallGen(ASnum);
        
    //the number of nodes in each AS (this array is created in the main file of project and I manually insert numbers in the unit test)
    int nodenumArray[9]={6,9,9,9,9,7,10,10,6}; 
        
        
    //read the number of nodes in each AS and same it in a vector    
    testcallGen.readNodeVector(nodenumArray);
    cout << "read node vector";
    //read the common file for the common parameters
    testcallGen.readCommonFile("call-gen-input-params//");
    cout << "read common file.";

    
    /* Test the correctness of these small functions
     * readNodeVector
     * 
     * readCommonFile
     * 
     * readMatrix
     * 
     * mapIndex
     * 
     * mapNode
     */
    
    //test call generating function
    
    //produce a random arrival time
    double arrival_time = 0;
    
    //save the generated calls in the output file
    ofstream ofs(output_file_name.c_str());
    
    ofs << "// Arrival time, [AR Options], Duration, Capacity, src AS, src node, dest AS, Dest node"<<endl;
    if(ofs.is_open())
    {
        //relative to the number of call generated    
        for(int j = 1; j <= number_of_call_generated; j++)
    	{
           
            //generate a call for that arrival time arrival time
			//TODO: chagne generate call to get arrival rate as an input and produce arrival time within it. 
            testcallGen.generateCall(arrival_time,AR_whole_time,lead_time,single_slot_time,1);
            
            //save the arrival time in file
            ofs << arrival_time << " [ ";
            
            //print all the AR options if it is a USST call
            for(int i = 0; i < testcallGen.ARvec.size(); i++)
                ofs << testcallGen.ARvec[i] << " ";
            ofs << "] ";
            //print Duration, Capacity, ASnum, source node, dest AS, Dest node in the same line
            ofs << testcallGen.Duration << " " << testcallGen.Capacity << " " << ASnum << " " << testcallGen.source_node << " " << testcallGen.dest_AS << " " << testcallGen.dest_node<<endl; 
            
            arrival_time=testcallGen.arrival_time;

        }

    }
    else cout << "Unable to open file"<<endl;
}

