#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <map>
#include <unordered_set>
#include <vector>

using namespace std;

string sparkPath = "/Users/lilinzhe/Documents/spark-1.6.1/";
string monitorPath = "/Users/lilinzhe/Desktop/netowrk_monitor/src/";
string recordPath = "/Users/lilinzhe/Desktop/netowrk_monitor/record/";
string filesPath = "/Users/lilinzhe/Desktop/netowrk_monitor/files/";

// /Users/lilinzhe/Documents/spark-1.6.1/bin/spark-submit /Users/lilinzhe/Desktop/netowrk_monitor/src/network_monitor.py localhost 9999



map<string, string> IPtoWEB;
unordered_set<string> fileStatus; // read or not


map<string, vector<string>> IPtoRTT;
map<string, vector<string>> IPtoTTL;
map<string, vector<string>> IPtoLOSSRATE;

vector<string> TTL_Deviation;

map<string, vector<string>> TRtoRTT;


void *analysis(void* threadid){
    
    while (true) {
        
        // loss rate
        for (auto& e : IPtoLOSSRATE) {
            if (e.second.back() > 99) {
                cout << "***** " << IPtoWeb[e.first] << " *** link failure *****" << endl;
                
                /*
                string cmd = "traceroute -q 1 " + e.first + " > tr.log";
                for (int i = 0; i < 20; i++) {
                    for (int j = 0; j < 10; j++) {
                        system(cmd.c_str());
                        system("cat tr.log | nc -l 9999");
                    }
                    sleep(1);
                }
                */
                break;
            }
        }
        
        // delay
        for (auto& e : IPtoRTT) {
            if (e.second.size() < 50) {
                continue;
            }
            
            int prev = INT_MAX;
            int decreaseLen = 0;
            for (int i = e.second.size() - 1; i >= 0 ; i--) {
                if (prev >= (e.second)[i]) {
                    decreaseLen++;
                    prev = (e.second)[i];
                }
            }
            
            if (decreaseLen > 50) {
                cout << "***** " << IPtoWeb[e.first] << " *** link congestion *****" << endl;
                
                /*
                 string cmd = "traceroute -q 1 " + e.first + " > tr.log";
                 for (int i = 0; i < 20; i++) {
                 for (int j = 0; j < 10; j++) {
                 system(cmd.c_str());
                 system("cat tr.log | nc -l 9999");
                 }
                 sleep(1);
                 }
                 */
                break;
            }
        }
        
        
        // ttl
        for (auto& e : IPtoTTL) {
            if (e.second.size() < 50) {
                continue;
            }
            
            int prev = 0;
            int changes = -1;
            for (auto& ee : e.second) {
                if (ee == prev) {
                    continue;
                }
                
                prev = ee;
                changes++;
            }
            
            if (e.size() > 2 && changes == 1) {
                cout << "***** " << IPtoWeb[e.first] << " *** link changes *****" << endl;
                
                /*
                string cmd = "traceroute -q 1 " + e.first + " > tr.log";
                for (int i = 0; i < 20; i++) {
                    for (int j = 0; j < 10; j++) {
                        system(cmd.c_str());
                        system("cat tr.log | nc -l 9999");
                    }
                    sleep(1);
                }
                */
                break;
            }
        }
        
    }
}


void storeData(string& line){
    //cout << line << endl;
    
    if(line.size() < 4) return; // filter out invalued statement
    
    int i = 2;
    while (line[i] != '\'') i++;
    int len = i - 2;
    string type = line.substr(2, len);
    
    //cout << "type = " << type << endl;
    
    size_t start = line.find("k:") + 2;
    size_t end = line.find(".k");
    string key = line.substr(start, end - start);
    
    start = line.find("v:") + 2;
    end = line.find(".v");
    string value = line.substr(start, end - start);
    
    if (type == "PING_RTT"){
        cout << key << ", " << value << endl;
        IPtoRTT[key].push_back(value);
        
        if(IPtoRTT[key].size() > 500){
            IPtoRTT[key].erase(IPtoRTT[key].begin());
        }
        
    }else if (type == "PING_RTT_Deviation"){
        
        
    }else if (type == "PING_TTL"){
        //cout << key << ", " << value << endl;
        IPtoTTL[key].push_back(value);
        
        if(IPtoTTL[key].size() > 500){
            IPtoTTL[key].erase(IPtoTTL[key].begin());
        }
        
    }else if (type == "PING_TTL_Deviation"){
        //cout << key << ", " << value << endl;
        if (atoi(value.c_str()) > 1) {
            TTL_Deviation.push_back(key);
        }
        
    }else if (type == "PING_LOSSRATE"){
        //cout << key << ", " << value << endl;
        IPtoLOSSRATE[key].push_back(value);
        
        if(IPtoLOSSRATE[key].size() > 500){
            IPtoLOSSRATE[key].erase(IPtoLOSSRATE[key].begin());
        }
        
    }else if (type == ""){ // traceroute
        TRtoRTT
    }
    
    #ifdef logging
    for (auto e : IPtoRTT) {
        cout << e.first << " -- " << endl;
        for (auto ee : e.second) {
            cout << ee << endl;
        }
    }
    #endif
}


// make it a thread
void writeJsonFile(string IP){
    ofstream fp;
    fp.open("info.json");
    
    fp << "pingData =" << endl;
    fp << "{" << endl;
    
    fp << "\"IP\":\"" << IP << "\"," << endl;
    
    fp << "\"WEB\":\"" << IPtoWEB[IP] << "\"," << endl;
    
    fp << "\"RTT\":[";
    for (auto e : IPtoRTT[IP]) {
        fp << "\"" << e << "\",";
    }
    fp << "]," << endl;
    
    fp << "\"TTL\":[";
    for (auto e : IPtoTTL[IP]) {
        fp << "\"" << e << "\",";
    }
    fp << "]," << endl;
    
    fp << "\"LOSS\":[";
    for (auto e : IPtoLOSSRATE[IP]) {
        fp << "\"" << e << "\",";
    }
    fp << "]" << endl;
    
    fp << "};" << endl;
    
    fp.close();
}


void parseRecord(void) {
    
    TTL_Deviation.resize(0);
    
    auto dir = opendir(recordPath.c_str());
    if(dir == NULL) {
        #ifdef logging
        std::cout << "could not open directory: " << recordPath.c_str() << std::endl;
        #endif
        return;
    }
    
    auto entity = readdir(dir);
    
    while (entity != NULL) {
        
        #ifdef logging
        cout << "Outer-> " << entity->d_name << endl;
        #endif
        
        if (entity->d_type == DT_DIR && entity->d_name[0] != '.') {
            
            string subPath = recordPath + entity->d_name + "/";
            
            // avoid spark crash
            string fileName(entity->d_name);
            //cout << fileName << "   11111" << endl;
            
            if (fileStatus.find(fileName) == fileStatus.end()){
                fileStatus.insert(fileName);
                
            }else{
                string rmRecordedFolder = "rm -rf " + subPath;
                //cout << rmRecordedFolder << endl;
                system(rmRecordedFolder.c_str());
                
                fileStatus.erase(fileName);
                
                entity = readdir(dir);
                
                continue;
            }
            
            //cout << fileName << "     2222" << endl;
            
            
            #ifdef logging
            cout << "subPath = " << subPath << endl;
            #endif

            auto subDir = opendir(subPath.c_str());
            if(subDir == NULL) {
                #ifdef logging
                std::cout << "could not open directory: " << subPath.c_str() << std::endl;
                #endif
                return;
            }
            
            auto subEntity = readdir(subDir);
            
            while (subEntity != NULL) {
                
                #ifdef logging
                cout << "Inner -> " << subEntity->d_name << endl;
                #endif
                
                if (subEntity->d_type == DT_REG && subEntity->d_name[0] != '.' && subEntity->d_name[0] != '_') {
                    string filePath = subPath + subEntity->d_name;
                    
                    #ifdef logging
                    cout << filePath << endl;
                    #endif
                    
                    ifstream fp;
                    fp.open(filePath);
                    string line;
                    while (getline(fp, line)) {
                        
                        //cout << line << endl;
                        storeData(line);
                    }
                    fp.close();
                    
                }
                subEntity = readdir(subDir);
            }
            
        }
        
        #ifdef logging
        cout << endl;
        #endif
        entity = readdir(dir);
    }
    
    /* need a control function here to determine which IP should be wrote */
    
    // write data to a json file
    if (IPtoRTT.begin() != IPtoRTT.end()) {
        cout << "IP is " << (IPtoRTT.begin())->first << endl;
        writeJsonFile((IPtoRTT.begin())->first);
    }

}


void IPWebMapping(){
    string fileName = filesPath + "web_ip.txt";
    
    ifstream fp;
    fp.open(fileName);
    
    string line;
    while (getline(fp, line)) {
        size_t space = line.find(" ");
        string web = line.substr(0, space);
        string ip = line.substr(space+1);

        IPtoWEB[ip] = web;
    }
    
    #ifdef logging
    for (auto e : IPtoWEB) {
        cout << e.first << " -- " << e.second << endl << endl;
    }
    #endif
}



int main(){
    IPWebMapping();
    
    pthread_t analysisThread;
    int rc = pthread_create(&analysisThread, NULL, analysis, NULL);
    if (rc) {
        cout << "Error:unable to create thread," << rc << endl;
        exit(-1);
    }

    while (true) {
        
        parseRecord();
        sleep(1);
    }

    //string rmPingInformationLog = "rm pingInfor.log";
    //system(rmPingInformationLog.c_str());
 
}
