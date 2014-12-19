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
   
    ofstream myfile;
    myfile.open("callOutputBaseLine.txt");
    myfile << "Is_Success AS_Num src dest ARoption selectedOptionIndex pathLength Arrival_Rate\n";
    myfile.close();
     cout <<"first"<<endl; 
    
        
        Initializer init= Initializer(50);
        cout << "50" <<endl; 
    
    
    return 0;
}

