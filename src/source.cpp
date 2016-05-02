#include <stdlib.h>
#include <unistd.h>

int main(){
    while (true) {
        
        system("sh script.sh >> log.log");
        system("cat log.log | nc -l 9999");
        sleep(2);
        system("rm -rf log.log");
    }
    
}
