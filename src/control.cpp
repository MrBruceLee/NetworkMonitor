#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <map>
#include <vector>

using namespace std;

string sparkPath = "/Users/lilinzhe/Documents/spark-1.6.1/";
string monitorPath = "/Users/lilinzhe/Desktop/netowrk_monitor/src/";
string recordPath = "/Users/lilinzhe/Desktop/netowrk_monitor/record/";

// /Users/lilinzhe/Documents/spark-1.6.1/bin/spark-submit /Users/lilinzhe/Desktop/netowrk_monitor/src/network_monitor.py localhost 9999


/*
void *runSpark(void* threadid){
    cout << sparkPath << endl;
    string startSpark = sparkPath + "bin/spark-submit " + monitorPath + "network_monitor.py localhost 9999";
    
    system(startSpark.c_str());
}
*/

void parseRecord(void) {
    auto dir = opendir(recordPath.c_str());
    if(dir == NULL) {
        std::cout << "could not open directory: " << recordPath.c_str() << std::endl;
        return;
    }
    
    map<string, vector<string>> IPtoRTT;
    
    auto entity = readdir(dir);
    
    while (entity != NULL) {
        cout << "Outer -> " << entity->d_name << endl;
        
        if (entity->d_type == DT_DIR && entity->d_name[0] != '.') {
            string subPath = recordPath + entity->d_name + "/";
            cout << "subPath = " << subPath << endl;
            auto subDir = opendir(subPath.c_str());
            if(subDir == NULL) {
                std::cout << "could not open directory: " << subPath.c_str() << std::endl;
                return;
            }
            auto subEntity = readdir(subDir);
            
            while (subEntity != NULL) {
                cout << "Inner -> " << subEntity->d_name << endl;
                
                if (subEntity->d_type == DT_REG && subEntity->d_name[0] != '.' && subEntity->d_name[0] != '_') {
                    string filePath = subPath + subEntity->d_name;
                    cout << filePath << endl;
                    
                    ifstream fp;
                    fp.open(filePath);
                    string line;
                    while (getline(fp, line)) {
                        cout << line << endl;
                        
                        int i = 0;
                        int j = 0;
                        while (line[j] != '@') j++;
                        string IP = line.substr(i, j-i);
                        
                        i = j + 1;
                        j = j + 1;
                        while (line[j] != '@') j++;
                        string TTL = line.substr(i, j-i);
                        
                        i = j + 1;
                        j = j + 1;
                        while (line[j] != '@') j++;
                        string RTT = line.substr(i, j-i);
                        
                        IPtoRTT[IP].push_back(RTT);
                    }
                    fp.close();
                    
                }
                subEntity = readdir(subDir);
            }
            
            string rmRecordedFolder = "rm -rf " + subPath;
            system(rmRecordedFolder.c_str());
        }
        
        cout << endl;
        entity = readdir(dir);
    }
    
    
    ofstream fp;
    fp.open("info.json");
    
    fp << "rttData =" << endl << "[" << endl;
    
    for (auto e : IPtoRTT) {
        string perIP = "{\"IP\":\"" + e.first + "\",\"RTT\":[";
        
        for (auto eachRTT : e.second)
            perIP += "\"" + eachRTT + "\",";
        
        if(perIP.back() == ',' ) perIP.pop_back();
        perIP += "]},";
        
        fp << perIP << endl;
    }
    
    fp << "];";
    
    fp.close();
}


int main(){
    /*
    pthread_t sparkThread;
    int rc = pthread_create(&sparkThread, NULL, runSpark, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }
    sleep(2);
    */


    while (true) {
        
        parseRecord();
    }

    //string rmPingInformationLog = "rm pingInfor.log";
    //system(rmPingInformationLog.c_str());
 
}