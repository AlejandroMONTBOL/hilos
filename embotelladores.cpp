#include <iostream>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore.h>
#define CAPACIDAD_CAJA 10
using namespace std;
using namespace chrono;

mutex mtx;
sem_t sem;
//variables compartidas o de estado
int botellasEnCaja = 0;

void prepararBotella(int pid) {
    printf("Embotellador: %d preparando botella \n", pid);
}
void colocarBotellasEnCaja(int pid) {
    printf("Embotellador: %d coloca botella en caja \n", pid);
}
void hiloEmbotellador(int pid) {
    while (true)
    {
        prepararBotella(pid);
        mtx.lock();
        while (botellasEnCaja == CAPACIDAD_CAJA)
        {
            mtx.unlock();
            sem_wait(&sem);
            mtx.lock();
        }
        colocarBotellasEnCaja(pid);
        botellasEnCaja++;
        if (botellasEnCaja == CAPACIDAD_CAJA)
        {
            sem_post(&sem);
        }
        mtx.unlock();
    }
}

void tomarCaja(int pid)
{
    printf("Empaquetador: %d toma una caja\n", pid);
}
void sellarCaja(int pid)
{
    printf("Empaquetador: %d sella la caja\n", pid);
}
void guardarCajaEnAlmacen(int pid)
{
    printf("Empaquetador: %d guarda caja en almacen\n", pid);
}
void ponerCajaVacia(int pid) 
{
    printf("Empaquetador: %d pone caja vacia\n", pid);
}


void hiloEmpaquetador(int pid)
{
    while (true)
    {
        mtx.lock();
        while (botellasEnCaja < CAPACIDAD_CAJA)
        {
            mtx.unlock();
            set_wait(&sem);
            mtx.lock();
        }
        tomarCaja(pid);
        sellarCaja(pid);
        guardarCajaEnAlmacen(pid);
        ponerCajaVacia(pid);
        botellasEnCaja = 0;
        sem_post(&sem);
        mtx.unlock();
    }
}

int main() {
    sem_init(&sem, 0, 0);
    thread empaquetador(hiloEmpaquetador);
    thread embotellador(hiloEmbotellador);
    embotellador.join();
    empaquetador.join();
    /*for (int i = 0; i < NUM_CANARIOS; i++)
    {
        threads[i].join();
    }*/
    //liberamos recursos
    sem_destroy(&sem);
    //delete[] threads;
    return EXIT_SUCCESS;
}
