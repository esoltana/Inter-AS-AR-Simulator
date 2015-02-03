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
   
    
    
    ofstream results;
    results.open("Output-files/Results-USST-EST.txt");
    results << "Arrival_rate maxlinkU numCall numOfUSST numOfEST USSTblocked ESTblocked FirstUSST secUSST thirdUSST waitEST avgLeUSST avgLeEST" << endl;
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
    
    
        //TODO: arrival_rate and simulation_Time should be defined in a way to calculate 10000 calls
    /*
    arrate=0.05;
    simulTime=100/arrate;
    Initializer(arrate, simulTime);

    arrate=0.1;
    simulTime=100/arrate;
    Initializer(arrate, simulTime);
    */
    ofstream myfile;
    myfile.open("Output-files/Output-USST-EST0.txt");
    myfile << "Arr_rate Arr_Time  arr_TS  is_acc  is_USST src  dest  dur  rate  ARopt  selectedOp  pathLen  path\n";
    myfile.close();
    
    string filename ="Output-files/Output-USST-EST0.txt";
    /*
    arrate=1;
    simulTime=100000/arrate;
    
    Initializer(arrate, simulTime,filename);
    */
    for( double i=0.1; i<=150; i*=2)
    {
        ofstream results;
        results.open("Output-files/Results-USST-EST.txt", ios::app);
        results << i << " ";
        results.close();
        
        for(int j=100000; j<=100000; j*=2)
        {
            
            stringstream ss;
            ss << "Output-files/Output-USST-EST" <<i <<".txt";
            filename=ss.str();

            ofstream myfile;
            myfile.open(filename.c_str());
            myfile << "Arr_Time  arr_TS  is_acc  is_USST src  dest  dur  rate  ARopt  selectedOp  pathLen  path\n";
            myfile.close();
            arrate=i;
            simulTime=j/arrate;
            Initializer init=Initializer(arrate, simulTime,filename); 
            
            
            
            results.open("Output-files/Results-USST-EST.txt", ios::app);

            
            //cout << arrRate << " " << CBP<< " " << avgpathLength << " " << firstPerc << " " << secondPerc << " " << thirdPerc << " " <<endl;
            results << fixed << setprecision(2) << init.CBP*100 << " " << init.arrivalNum[0]<< " " << init.arrivalNum[1] << " " <<init.arrivalNum[2]<< " " <<init.callNumber<< " " <<simulTime << " ";//" " << max_link_Util<< " " << callNumber << " " << callNumberUSST<< " " << callNumberEST << " " << blockedUSST <<" " << blockedEST <<" " << firstAR << " " << secondAR << " " << thirdAR << " " <<meanWait<<" " << avgpathLengthUSST << " " << avgpathLengthEST<<endl;
            results.close();
        }
        
        
        results.open("Output-files/Results-USST-EST.txt", ios::app);
        results << endl;
        results.close();
        
    }
    
    return 0;
}

