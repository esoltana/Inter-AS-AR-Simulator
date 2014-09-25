/* 
 * File:   Src_Dst_matrix_generator.cpp
 * Author: Elahe
 * 
 * Created on July 25, 2014, 7:49 AM
 */

#include "Src_Dst_Matrix_Generator.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

Src_Dst_Matrix_Generator::Src_Dst_Matrix_Generator(int ASnum) {
    
    AS_num=ASnum;
    int nodeNum [AS_num];
    string asfile;
    
    for(int i=1; i<=AS_num; i++)
    {
        stringstream asname;
        asname << "inputdata/AS" << i << "/AS" << i;
        asfile=asname.str();
        nodeNum[i-1]=read_ASfile(asfile);
    }
    
    writeFile_TwoLevel("1-two_level_src_dst_prob_matrix", nodeNum);
   writeFile_completelyUniform("2-completelyUniform_src_dst_prob_matrix", nodeNum);
   writeFile_IntraDomainCalls("3-IntraDomainCalls_src_dst_prob_matrix", nodeNum);
}

int Src_Dst_Matrix_Generator::read_ASfile(string dir){

    int ASNum=0;
     ifstream inf(dir.c_str());
    
    if (inf.is_open()) {
    
       string line;
       getline(inf, line);
       getline(inf, line);
       cout << "Line: "<<line << endl;
       ASNum=atoi(line.c_str());
        
    }
     inf.close();
     return ASNum;
}

void Src_Dst_Matrix_Generator::writeFile_TwoLevel(string dir, int nodeNum[]){
    
    double  probValue;
    ofstream outputFile;
    int tnodeNum=0;
    
    outputFile.precision(16);
    outputFile.setf(ios::fixed);
    outputFile.setf(ios::showpoint); 


    outputFile.open(dir.c_str());
        //outputFile << "//The file contains the probabilities of having a call between each src and dst; " << endl;
        outputFile << "//number of AS; number of nodes in each AS; " << endl;
        for(int i=1; i<=AS_num; i++)
        {
            for(int j=1; j<=nodeNum[i-1]; j++)
            {
                outputFile << "//"<<i << ";  " << j <<"; " << endl;
                
                for(int k=1; k<=AS_num; k++)
                {
                    if(i==k)
                        tnodeNum=nodeNum[k-1]-1;
                    else
                        tnodeNum=nodeNum[k-1];
                    
                    for(int m=1; m<=nodeNum[k-1]; m++)
                    {
                         if(i==k && j==m)
                                outputFile << "0 ";
                        else
                        {
                                probValue=(double)1/(double)AS_num/(double)tnodeNum;
                                cout << probValue <<endl;
                                outputFile << probValue << " ";
                        }
                    }
                }
                outputFile << endl;
            }
        }
     outputFile.close();
}


void Src_Dst_Matrix_Generator::writeFile_completelyUniform(string dir, int nodeNum[]){
    
    double  probValue;
    ofstream outputFile;
    int tnodeNum=0;
    
    outputFile.precision(16);
    outputFile.setf(ios::fixed);
    outputFile.setf(ios::showpoint); 
    outputFile.open(dir.c_str());
    
    outputFile << "//number of AS; number of nodes in each AS; " << endl;
    for(int i=0; i<AS_num; i++)
        tnodeNum +=nodeNum[i];
    
    
    probValue=(double)1/(double)(tnodeNum-1);
    cout << probValue <<endl;
    
    for(int i=1; i<=AS_num; i++)
    {
        for(int j=1; j<=nodeNum[i-1]; j++)
        {
            outputFile << "//"<<i << ";  " << j <<"; " << endl;
            for(int k=1; k<=AS_num; k++)
            {
                for(int m=1; m<=nodeNum[k-1]; m++)
                {
                    if(i==k && j==m)
                            outputFile << "0 ";
                    else
                    {
                            outputFile << probValue << " ";
                    }
                }
            }
            outputFile << endl;
        }
    }
    outputFile.close();
}

void Src_Dst_Matrix_Generator::writeFile_IntraDomainCalls(string dir, int nodeNum[]){
    
    double  probValue;
    ofstream outputFile;
    int tnodeNum=0;
    
    outputFile.precision(16);
    outputFile.setf(ios::fixed);
    outputFile.setf(ios::showpoint); 
    outputFile.open(dir.c_str());
    outputFile << "//number of AS; number of nodes in each AS; " << endl;
    
    for(int i=1; i<=AS_num; i++)
    {
        for(int j=1; j<=nodeNum[i-1]; j++)
        {
            outputFile << "//"<<i << ";  " << j <<"; " << endl;
            for(int k=1; k<=AS_num; k++)
            {
                tnodeNum=nodeNum[k-1]-1;
                
                for(int m=1; m<=nodeNum[k-1]; m++)
                {
                    if(i==k && j==m)
                            outputFile << "0 ";
                    else if(i==k)
                    {
                         probValue=(double)1/(double)(tnodeNum);
                         cout << probValue <<endl;
                         outputFile << probValue << " ";
                    }else
                            outputFile << "0 ";
                            
                }
            }
        
        outputFile << endl;
        }
    }
    
    outputFile.close();
}
