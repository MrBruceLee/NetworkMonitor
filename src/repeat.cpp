#include <stdlib.h>
#include <unistd.h>

int main(){
    int i = 0;
    for(int i = 0; i < 10; i++){
        system("traceroute -q 1 www.apple.com >> tr.log");
        sleep(10);
    }
}
