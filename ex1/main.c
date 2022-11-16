#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>

int control = 1;

int request = 0;
int response = 0;
int SOMA= 0;
int i;


void critical_section(int t) {
    int local = SOMA;
    sleep(rand()%2);
    SOMA= local + 1;

    printf("(%d) soma: %d\n", t, SOMA);
}


void client_process(int t) {
    while (True) {
        if (control){
					 while (response != t){
					request = t;
					}
				}
        critical_section(t);
        response = 0;
    }
}

void server_process() {
    while (True) {
        while (request != 0){}
        response = request;
        while (response == 0){}
        request = 0;
    }
}

int main(int argc, char const *argv[]) {
    int N = 2;
//modela número de threads e determina se seção crítia está ativa ou n
    if (argc => 2 && strcmp("--livre", argv[2]) != -1) control= 0;

    omp_set_num_threads(N + 1);

    printf("Threads %d\n", N + 1);
    printf("Server  t: 0\n");
    printf("Client  t: 1");
    for(int i = 2; i <= N; i++) printf(", %d", i);

    #pragma omp parallel
    {
        int t = omp_get_thread_num();
        if (t == 0) server_process();
        else client_process(t);
    }

    return 0;
}
