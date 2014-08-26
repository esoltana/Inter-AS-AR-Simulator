/* 
 * File:   readcall.cpp
 * Author: leo
 * 
 * Created on June 27, 2014, 12:45 AM
 */

#include <vector>

#include "readcall.h"
void readcall::init(char const* inputfile){
    linecount = 0;
    callfile = inputfile;
    ifstream infile(callfile);
    if (infile.is_open()){
        while(getline(infile,line))
            linecount ++;
    }
    //linecount = linecount;
    infile.close();
    callarrive = new double[linecount];
    callstart.clear();
    band = new int[linecount];
    dura = new int[linecount];
    sourceAS = new int[linecount];
    sourceNode = new int[linecount];
    destinationAS = new int[linecount];
    destinationNode = new int[linecount];
    res = new int[linecount];
}

void readcall::ReadCall(int BAn) {
    int ccount = 0;
    ifstream infile(callfile);
if (infile.is_open())
{
    /*
    infile>> callarrive[ccount];
        callstart.push_back(callstartentry);
        for (int i = 0; i < BAn; i++){
        infile >> callstartN;
        callstart[ccount].push_back(callstartN);
        }
        infile >> band[ccount] >> dura[ccount] >> sourceAS[ccount] >> sourceNode[ccount] >> destinationAS[ccount] >> destinationNode[ccount];
        ccount++;
     */
    //getline(infile, line);
    while(infile>> callarrive[ccount])
    {
        
        callstart.push_back(callstartentry);
        for (int i = 0; i < BAn; i++){
        infile >> callstartN;
        callstart[ccount].push_back(callstartN);
        }
        infile >> band[ccount] >> dura[ccount] >> sourceAS[ccount] >> sourceNode[ccount] >> destinationAS[ccount] >> destinationNode[ccount];
        ccount++;
    }
    // process pair (a,b)
    infile.close();
}
else
{
    cout << "Error opening file";
}
    
}



