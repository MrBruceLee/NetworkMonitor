#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(){
    ifstream ifp;
    ifp.open("valid_addr.txt");
    if (ifp.is_open() == false) {
        cout << "cannot open file: " << "valid_addr.txt" << endl;
        exit(0);
    }
    
    ofstream ofp;
    ofp.open("web_ip.txt");
    
    string line;
    while (getline(ifp, line)) {
        //cout << line << endl;
        if (line.substr(0,3) != "www")
            continue;
        
        string cmd = "ping -c 1 " + line + " > log";
        system(cmd.c_str());
        
        sleep(1);
        
        ifstream logfile;
        logfile.open("log");
        if (logfile.is_open() == false)
            continue;
        
        string containt;
        bool isValid = false;
        while (getline(logfile, containt)) {
            //cout << containt << endl;
            if (containt.substr(0,2) != "64")
                continue;
            
            int begin = 0;
            int end = 0;
            for (int i = 0; i < containt.size(); i++) {
                if (containt.substr(i, 5) == "from ")
                    begin = i + 5;
                
                if (containt.substr(i, 1) == ":")
                    end = i;

                if (begin != 0 && end != 0)
                    break;
            }
            
            if (begin != 0 || end != 0)
                ofp << line << " " << containt.substr(begin, end-begin) << endl;
        }
        
        logfile.close();
    }
    
    
    system("rm log");

    ofp.close();
    ifp.close();
}