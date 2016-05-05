#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

using namespace std;

//string path = "/Users/lilinzhe/Desktop/netowrk_monitor/test/TTL/";

int main(int argc, char** argv){
    
    if (argc < 2) {
        cout << "Usage run <path>" << endl;
        return 0 ;
    }
    
    string path(argv[1]);
    
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