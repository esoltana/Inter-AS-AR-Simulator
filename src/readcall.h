/* 
 * File:   readcall.h
 * Author: leo
 *
 * Created on June 27, 2014, 12:45 AM
 */

#ifndef READCALL_H
#define	READCALL_H

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;
class readcall {
public:
    void init(char const* inputfile);
    void ReadCall(int BAn);
    double *callarrive;
    int *band, *dura, *sourceAS, *sourceNode, *destinationAS, *destinationNode, *res;
    vector <vector <int> > callstart;
    vector <int> callstartentry;
    int linecount,theN,callstartN;
private:
    char const*callfile;
    string line;
    int num;

};

#endif	/* READCALL_H */

