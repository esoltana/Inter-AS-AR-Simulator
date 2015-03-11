/* 
 * File:   main.cpp
 * Author: leo
 *
 * Created on June 26, 2014, 8:56 PM
 */

//Revised by Elahe on September 7, 2014

#include "Initializer.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>
using namespace std;


int main(int argc, char* argv[]) {
   
    
    string resultFilename,outputFilename;
    stringstream ss;
    ss << "Output-files/Output-USST-EST"<<argv[1]<<".txt";
    outputFilename=ss.str();

    stringstream sp;
    sp << "Output-files/Result-USST-EST"<<argv[1]<<".txt";
    resultFilename=sp.str();

    ofstream myfile;
    myfile.open(outputFilename.c_str());
    myfile << "Arr_rate number Arr_Time  arr_TS  is_acc  is_USST src  dest  dur  rate  ARopt  selectedOp  pathLen  path\n";
    myfile.close();
    
    
    ofstream results;
    results.open(resultFilename.c_str());
    results << "Arate N N1 CBP CBPU CBPE U waitEST waitUSST nUSST nEST USSTblocked ESTblocked FirstUSST secUSST thirdUSST avgLeUSST avgLeEST simTime" << endl;
    results.close();
    
    
    //cout << "Arrival_rate max_link_util numOfCalls numOfUSST numOfEST USSSTblocked ESTblocked FirstUSST secUSST thirdUSST waitEST avgLengthUSST avgLengthEST " << endl;
    

    /*
    ofstream myfile;
    //myfile.open("Output-files/1-USST-singleLink-oneOption.txt");
    myfile.open("Output-files/7-EST-3nodes.txt");
    myfile << "Topology Info:  One Link with Two nodes, link-rate=2Gbps"<<endl;
    myfile << "System Info:  TimeslotSize=1sec, WindowSize=3sec=3TS, leadTime=1TS"<<endl;
    myfile << "Simulation Info:  simulationTime=2sec=2TS" << endl;
    myfile << "Call Info:  USST, Call-rate=1Gbps, Call-Duration=1sec, one-ARoption, arrival-rate=5calls/sec" << endl;
    myfile << "---------------" <<endl <<endl;
    myfile << "Arr_Time  arr_TS  is_acc  src  dest  dur  rate  ARopt  selectedOp  pathLen  path"<< endl;
    */
    
   
    double simulTime=0, arrate=1;
    
    double rate;
    double k=24;
    for( double i=1; i<=15; i++)
    {
        cout << k*60*60<<endl;
        rate=i/3600;
        for(int j=10000; j<=10000; j*=10)
        {
            arrate=rate;
            simulTime=j/arrate;
            Initializer init=Initializer(arrate, simulTime,outputFilename, k*60*60); 
           
            results.open(resultFilename.c_str(), ios::app);
            results <<rate <<" " << j << " " << init.callNumber <<" "<< init.CBP*100 << " " << init.CBPU*100 << " " <<init.CBPE*100 << " " << init.max_link_Util*100 << " " << init.meanWait<<" " << init.waitUSST<<" " <<init.callNumberUSST<< " " << init.callNumberEST << " " << init.blockedUSST <<" " << init.blockedEST <<" " << init.firstAR << " " << init.secondAR << " " << init.thirdAR << " " <<  init.avgpathLengthUSST << " " << init.avgpathLengthEST<<" " << simulTime << endl;
            results.close();
        }
        //results.open(resultFilename.c_str(), ios::app);
        //results << endl;
        //results.close();
    }
    
    return 0;
}

