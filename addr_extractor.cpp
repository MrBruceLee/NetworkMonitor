#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <unistd.h>

#define BUF 4096

using namespace std;

int main(){
    
    ofstream ofp;
    ofp.open("addr.txt");
    
    int cnt = 0;
    while (true) {
        cnt++;
        char name[10];
        sprintf(name, "%d%s", cnt, ".html");
        FILE *fp = fopen(name, "r");
        if(fp == NULL){
            cout << "cannot open file: " << name << endl;
            break;
        }
        
        char record[BUF];
        while( fgets (record, BUF, fp) != NULL ){
            if(strlen(record) > BUF){
                cout << "buffer overflow" << endl;
                exit(0);
            }
            
            string line(record);
            
            bool find = false;
            int i;
            for (i = 0; i < (int)line.size()-10; i++) {
                if (line.substr(i, 10) == "/siteinfo/") {
                    find = true;
                    break;
                }
            }
            i += 10;
            
            if (find) {
                int cnt = 0;
                while (line[i+cnt] != '\"') {
                    cnt++;
                }
                cout << line.substr(i, cnt) << endl;
                ofp << line.substr(i, cnt) << endl;
                ofp << "www." << line.substr(i, cnt) << endl;
            }
            
        }
        
        fclose(fp);
    }
    ofp.close();

    


    ifstream ifp;
    ifp.open("addr.txt");
    if(ifp.is_open() == false){
        cout << "could not open file addr.txt"<< endl;
        exit(0);
    }
    
    ofp.open("valid_addr.txt");
    if(ifp.is_open() == false){
        cout << "could not open file valid_addr.txt"<< endl;
        exit(0);
    }
    
    string line;
    while (getline(ifp, line)) {
        char cmd[BUF];
        //strcpy(cmd, line.c_str());
        sprintf(cmd, "%s%s%s", "traceroute -q 1 -m 21 -w 1 ", line.c_str(), " > log");
        cout << cmd << endl;
        system(cmd);
        sleep(25);

        ifstream logfp;
        logfp.open("log");
        if (logfp.is_open() == false) {
            cout << "could not open file log" << endl;
            exit(0);
        }
        
        string record;
        bool valid = true;
        while (getline(logfp, record)) {
            cout << record << endl;

            int i;
            for (i = 0; i < (int)record.size(); i++) {
                if (record[i] != ' ') {
                    break;
                }
            }
            int hop = atoi(record.substr(0,i).c_str());
            if (hop > 21) {
                valid = false;
                break;
            }
            
            for (; i < (int)record.size(); i++) {
                if (record[i] == '*') {
                    valid = false;
                    break;
                }
            }
        }
        
        if (valid) {
            ofp << line << endl;
        }
    }
    
    ifp.close();
    ofp.close();
 
}
















