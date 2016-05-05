#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

using namespace std;

string path = "/Users/lilinzhe/Desktop/netowrk_monitor/test/LossRate/";

int main(){
    
    int i = -1;
    string fileName;
    ifstream fp;
    string cmd;
    
    while (true) {
        i++;
        
        fileName = path + "ping" + to_string(i) + ".log";
        
        
        fp.open(fileName);
        if (fp.is_open() == false) {
            break;
        }
        fp.close();
        
        cmd = "cat " + fileName + " | nc -l 9999";
        system(cmd.c_str());
        
        
        fileName = path + "tr" + to_string(i) + ".log";
        
        fp.open(fileName);
        if (fp.is_open() == false) {
            break;
        }
        fp.close();
        
        cmd = "cat " + fileName + " | nc -l 9999";
        system(cmd.c_str());
        
        sleep(1);
    }
    
}