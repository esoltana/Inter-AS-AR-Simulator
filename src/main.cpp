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
    myfile.open("Output-files/Out-ShortestPossible-multiple-m20k24.txt");
    myfile << "Is_Success AS_Num src dest ARoption selectedOptionIndex pathLength Arrival_Rate path\n";
    myfile.close();
    
    ofstream results;
    results.open("Output-files/Results-ShortestPossible-multiple-m20k24.txt");
    results << "ARrate CBP avgLength FirstARperc SecondARperc ThirdARperc " << endl;
    results.close();
    
    
    cout << "ARrate CBP avgLength FirstARperc SecondARperc ThirdARperc " << endl;
    
     
    for( int i=0; i<16; i++)
    {
        Initializer init= Initializer(50+i*10);
        //cout << 90+i*5 <<endl; 
    }
    
    return 0;
}

