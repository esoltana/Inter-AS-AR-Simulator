/* 
 * File:  Src_Dst_Matrix_Generator.h
 * Author: hasty
 *
 * Created on July 25, 2014, 7:49 AM
 */

#include <string>

using namespace std;

class Src_Dst_Matrix_Generator {
public:
    
    Src_Dst_Matrix_Generator(int ASnum);
    int read_ASfile(string dir);
    void writeFile_TwoLevel(string dir, int nodeNum[]);
    void writeFile_completelyUniform(string dir, int nodeNum[]);
    void writeFile_IntraDomainCalls(string dir, int nodeNum[]);
    int AS_num;
    
   
private:

};
