/*Monitor sensor*/
#include <math.h>
#include <pthread.h>
#include <string.h>

// Variavel de exclusao mutua -> limitar o acesso as variaveis locais
static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t alarme = PTHREAD_COND_INITIALIZER; //variavel de condicao
static double s_temp = 0, s_nivel = 0; //variaveis staticas locais
static double limite_atual = HUGE_VAL;

/* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido*/
void sensor_put(double temp, double nivel){ //inserir valores nas variaveis dos sensores
  pthread_mutex_lock(&exclusao_mutua); //trava a thread q acessa o sensor_put
  s_temp = temp;
  s_nivel = nivel;
  if(s_temp >= limite_atual) // mantem thread travada ate que o 
    pthread_cond_signal(&alarme); // ativa thread q está aguardando
  pthread_mutex_unlock(&exclusao_mutua);
}

/* Chamado por qualquer thread que precise do valor lido do sensor*/
double sensor_get(char s[5]){
  double aux;
  pthread_mutex_lock(&exclusao_mutua);
  if(strncmp(s,"t",1)==0)
    aux = s_temp;
  else if(strncmp(s,"h",1)==0)
    aux = s_nivel;
  pthread_mutex_unlock(&exclusao_mutua);
  return aux;
}

/*Thread fical bloqueando ate o valor do sensor chegar em limite*/
void sensor_alarmT(double limite){
  pthread_mutex_lock(&exclusao_mutua);
  limite_atual = limite;
  while(s_temp < limite_atual)
    pthread_cond_wait(&alarme, &exclusao_mutua); // mantema a thread bloqueada ate a condicao ser falsa
  limite_atual = HUGE_VAL;
  pthread_mutex_unlock(&exclusao_mutua);
}