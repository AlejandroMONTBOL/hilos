#include <iostream>
#include <cstdlib>
#include <mutex>
#include <chrono>
#include <thread>
#include <semaphore.h>
#define NUM_BARBEROS 1
#define NUM_SILLAS 3
#define NUM_CLIENTES 5
using namespace std;
using namespace chrono;
//variables de estado
int sillasLibres = NUM_SILLAS;
//semaforos
sem_t semBarbero;
sem_t semClientes;
mutex sillas; //variable de exclusion mutua

void hiloBarbero(int id){
 while(true){
  sillas.lock(); //ENTRAR S C 
  while(sillasLibres  == NUM_SILLAS){ //no hay clientes esperando
     sillas.unlock();//SALIR S C
     printf("Barbero-%d: no hay clientes y se duerme.\n", id);//printf es printsafe
     //muestra un mensaje atomico
     //%d es un placeholder 
     sem_wait(&semBarbero); //DORMIR, esperar por clientes
     printf("Barbero-%d: despierta para atender clientes..\n", id);//printf es printsafe
     //imprimimos barbero despiera para atender clientes antes del entrarsc para saber que el
     //barbero este durmiendo
     sillas.lock();//ENTRAR S C
  }
  sillasLibres++;//llamar a cliente para darle atencion
  sem_post(&semClientes);//despertar, notificar a clientes
  sillas.unlock();//SALIR S C
  printf("Barbero-%d: cortando el pelo.\n", id);//printf es printsafe
  this_thread::sleep_for(seconds(1));
 }
}

void hiloCliente(int id){
  
  printf("Cliente-%d: llega a la barberia.\n", id);//printf es printsafe
  sillas.lock();//entrar sc
  if(sillasLibres > 0){
    sillasLibres--; //cliente se espera para esperar atencion
    printf("Cliente-%d: se sienta a esperar atencion. Sillas libreas: %d\n", id, sillasLibres);//printf es printsafe
    sillas.unlock(); //salir sc
    sem_post(&semBarbero); //DESPERTAR, notificar a un barbero
    sem_wait(&semClientes);//DORMIR, clientes esperando atencion
    printf("Cliente-%d: recibiendo atencion.\n", id);//printf es printsafe
    this_thread::sleep_for(seconds(1));

  }
  else{
    sillas.unlock(); //salir sc
    printf("Cliente-%d: se va sin ser atendido.\n", id);//printf es printsafe
  }

}

int main(){
 sem_init(&semBarbero,0,0);//no es un hilo compartido con el sistema
 sem_init(&semClientes, 0,0); //no es un hilo compartido con el sistema, eso es lo que indica el ulitm cero
 thread *barberos = new thread[NUM_BARBEROS];
 thread *clientes = new thread[NUM_CLIENTES];
 for(int i =0; i < NUM_BARBEROS; i++){
    barberos[i] = thread(hiloBarbero, i+1);
 }
 this_thread::sleep_for(seconds(1));
 for(int i =0; i < NUM_CLIENTES; i++){
    clientes[i] = thread(hiloCliente, i+1);
 }
 //espera a que hilos terminen su ejecucion
 for(int i = 0; i < NUM_CLIENTES; i++){
    clientes[i].join();
    //barberos no terminan su ejecucion por que estan en un ciclo infinito
    //en cambio los clientes no
    //
 }
 cout<<"No hay mas clientes por hoy, cerrando barberia."<<endl;//esta linea se imprime de forma atomica
 sem_destroy(&semBarbero);
 sem_destroy(&semClientes);
 //nota extra: Varios clientes despiertan al barbero, esos despertar se acumulan, cada vez que termine
 //de cortar el pelo a un cliente bloqueara su seccion critica y volvera a entrar a su SC a 
 //cortar el pelo a los clientes que los despertaron
 delete [] clientes;
 //no se borran los barberos por que lo barberos seguiran en ejecucion
 //si borramos los barberos habra un error en tiempo de ejecucion
 //barbero esta dentro de un ciclo while infinito, cuando no hayan clientes los barberos van a estar
 //durmiendo, debemos evitar que el sistema lo borre, por eso no se le pone un delete
 return EXIT_SUCCESS;
}

