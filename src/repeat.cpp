#include <stdlib.h>
#include <unistd.h>

int main(){
    int i = 0;
    for(int i = 0; i < 10; i++){
        system("sh script.sh >> log.log");
    }
}
