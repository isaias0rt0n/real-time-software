#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket/socket.h"
#include "monitores/sensores.h"
#include "monitores/tela.h"

#define FALHA 1
#define NSEC_PER_SEC  (1000000000) //numero de nanosessungos em um segundo

void thread_mostra_status(void){
  double t, h;
  while(1){
    t = sensor_get("t");
    h = sensor_get("h");
    aloca_tela();
    system("tput reset"); //sempre apaga informações da tela 
    printf("Temperatura (T) --> %.2lf\n", t);
    printf("Nivel (H) --> %.2lf\n", h);
    libera_tela();
    sleep(1);
  }
}

void thread_le_sensor(void){ //le sensores periodicamente a cada 10ms
  struct timespec t, t_fim; //estrutura de tempo
  long periodo = 10e6; //10ms
  //le a hora atual e colcoar em t
  clock_gettime(CLOCK_MONOTONIC, &t);
  while(1){
    //Espera ate o inicio do proximo periodo
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

    
    double temperatura = mensagem("st-0"); //msg parar retornar valor do sensor de temp
    double nivel = mensagem("sh-0"); //msg parar retornar valor do sensor de nivel
    
    //envia msg via socket para receber valores dos sensores
    sensor_put(temperatura, nivel);

    //calcula inicio do proximo periodo
    t.tv_nsec += periodo;
    while (t.tv_nsec >= NSEC_PER_SEC){
        t.tv_nsec -= NSEC_PER_SEC;
        t.tv_sec++;
    }
  }
}
/*Thread para monitorar a temperatura e emitir alarme quando pssar o limite*/
void thread_alarme(void){
  while (1){
    sensor_alarmT(29);
    aloca_tela();
    printf("Ta pegando fogo, bicho!\n");
    libera_tela();
    sleep(1);
  }
}

int main(int argc, char *argv[]){
	char msg_enviada[1000];
  double valor = 10.0;

  criar_socket(argv[1], atoi(argv[2]));

  pthread_t th1, th2, th3;

  pthread_create(&th1, NULL, (void *) thread_mostra_status, NULL);
  pthread_create(&th2, NULL, (void *) thread_le_sensor, NULL);
  pthread_create(&th3, NULL, (void *) thread_alarme, NULL);

  pthread_join(th1, NULL);
  pthread_join(th2, NULL);
  pthread_join(th3, NULL);

	/*strcpy( msg_enviada, "sta0");
	double msg = mensagem(msg_enviada);
  printf("Mensagem de resposta >>>%lf<<<\n", msg);*/
}
