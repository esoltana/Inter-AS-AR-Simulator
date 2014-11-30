/* 
 * File:   main.cpp
 * Author: leo
 *
 * Created on June 26, 2014, 8:56 PM
 */

//Revised by Elahe on September 7, 2014

#include "Initializer.h"
#include <fstream>
using namespace std;

int main(int argc, char* argv[]) {
   /*
    ofstream myfile;
    myfile.open("callOutput.txt");
    myfile << "Is_Success AS_Num selectedOptionIndex pathLength Arrival_Rate\n";
    myfile.close();
     */
    for(int j=1000; j<=1000; j*=2)
    {
        Initializer init= Initializer(j);
        cout << j <<endl; 
    }
    
    return 0;
}

