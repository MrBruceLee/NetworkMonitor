/*
 *  Usage:
 *  g++ -std=c++11 gen_json.cpp -o gen_json
 *  ./gen_json
 *
 */


#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

int main(){
    
    while (true) {
        
        map<string, vector<string>> IPtoRTT;
        
        for (int i = 0; i < 5; i++) {
            string IP = to_string(rand()%256) + "." +  to_string(rand()%256) + "." + to_string(rand()%256) + "." + to_string(rand()%256);
            
            for (int j = 0; j < 5; j++) {
                string RTT = to_string(rand()%100) + "." +  to_string(rand()%1000);
                
                IPtoRTT[IP].push_back(RTT);
            }
        }
        
        ofstream fp;
        fp.open("rttInfo.json");
        
        for (auto e : IPtoRTT) {
            string output = "{\"" + e.first + "\":[";
            
            for (auto eachRTT : e.second)
            output += "{\"RTT\":\"" + eachRTT + "\"},";
            
            if(output.back() == ',' ) output.pop_back();
            output += "]}";
            
            fp << output << endl;
        }
        
        fp.close();
        
        sleep(5);
        
    }
}