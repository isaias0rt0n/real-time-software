/*Monitor responsavel pelo acesso a tela*/
#include <pthread.h>

static pthread_mutex_t tela = PTHREAD_MUTEX_INITIALIZER;

/*trava o usao da tela*/
void aloca_tela(void){
  pthread_mutex_lock(&tela);
}

/*Libera o usao da tela*/
void libera_tela(void){
  pthread_mutex_unlock(&tela);
}