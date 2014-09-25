/* 
 * File:   main.cpp
 * Author: es3ce
 *
 * Created on September 25, 2014, 10:28 AM
 */


#include "ARserver.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    ARserver ARServer=ARserver(1,"InputParameters//");
    ARServer.printTopology();
    return 0;
}

