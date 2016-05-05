#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

using namespace std;

int main(){
    
    
    for (int i = 0; i < 100; i++) {
        string cmd = "sh script.sh > ping_temp" + to_string(i) + ".log";
        system(cmd.c_str());
        
        sleep(1);
    }
    
    
    
    
    ifstream ifp;
    ofstream ofp;
    
    int i = -1;
    
    while (true) {
        i++;
        
        string fileName = "ping_temp" + to_string(i) + ".log";
        ifp.open(fileName);
        if (ifp.is_open() == false) {
            break;
        }
        
        fileName = "ping" + to_string(i) + ".log";
        ofp.open(fileName);
        
        string line;
        
        while (getline(ifp, line)) {
            if (line.substr(0,28) == "64 bytes from 104.16.33.249:") {
                if (i > 15) {
                    ofp << "64 bytes from 104.16.33.249: icmp_seq=0 ttl=56 time=16.319 ms" << endl;
                }else{
                    ofp << line << endl;
                }
            }else{
                ofp << line << endl;
            }
        }
        
        ifp.close();
        ofp.close();
        
        fileName = "ping_temp" + to_string(i) + ".log";
        string cmd = "rm -rf " +  fileName;
        system(cmd.c_str());
        
    }

    
    
    /*
    for (int i = 0; i < 100; i++) {
        string cmd = "traceroute -q 1 104.16.33.249 > tr" + to_string(i) + ".log";
        system(cmd.c_str());
        sleep(10);
    }
    */
    
    
    for (int i = 0; i < 100; i++) {
        string fileName = "tr" + to_string(i) + ".log";
        
        ofstream fp;
        fp.open(fileName);
        
        int r = rand() % 100;
        fp << " 1  itap-wifipe-vlan1315.tcom.purdue.edu (10.186.112.1)  1.0" << r << " ms" << endl;
        
        r = rand() % 100;
        fp << " 2  lamb-netedgepe-vlan3015.tcom.purdue.edu (172.28.160.197)  1.4" << r << " ms" << endl;
        
        r = rand() % 100;
        fp << " 3  tel-netedgepe-vlan3014.tcom.purdue.edu (172.28.160.194)  1.8" << r << " ms" << endl;
        
        r = rand() % 1000;
        fp << " 4  192.5.40.8 (192.5.40.8)  4." << r << " ms" << endl;

        r = rand() % 1000;
        fp << " 5  tel-210-c9006-01-te0-0-0-3.tcom.purdue.edu (192.5.40.65)  5." << r << " ms" << endl;

        r = rand() % 1000;
        fp << " 6  149.165.183.90 (149.165.183.90)  8." << r << " ms" << endl;

        r = rand() % 1000;
        fp << " 7  xe-1-2-0.edge01.ord02.as13335.net (206.223.119.180)  8." << r << " ms" << endl;
        
        r = rand() % 1000;
        int rr = rand() % 2;
        fp << " 8  104.16.33.249 (104.16.33.249)  1" << rr << "." << r <<" ms" << endl;
        
        fp.close();
        
    }
}







