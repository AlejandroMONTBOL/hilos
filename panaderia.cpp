#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <chrono>
#include <semaphore.h>
#define NUM_CLIENTES 5
using namespace std;
using namespace chrono;
mutex tiquetero;
mutex turno;
sem_t sem;
int tiqueteActual = 1;
int turnoActual = 1;
int tomarTicket() {
	const lock_guard<mutex> lock(tiquetero);
	return tiqueteActual++;
	
}
void hiloCliente(int pid) {
	while (true)
	{
		int tiquete = tomarTicket();
		printf("Cliente %d llego a panaderia y tomo ticket: %d", pid, tiquete);
		turno.lock();
		while (tiquete != turnoActual)
		{
			turno.unlock();
			sem_wait(&sem);
			turno.lock();
		}
		printf("Cliente %d con tiquet %d recibiendo atencion", pid, tiquete);
		this_thread::sleep_for(seconds(rand() % 5 + 1));
		turnoActual++;
		sem_post(&sem);
		turno.unlock();
	}
}

int main() {
	sem_init(&sem, 0, 0);
	thread* clientes = new thread[NUM_CLIENTES];
	for (int i = 0; i < NUM_CLIENTES; i++) {
		clientes[i] = thread(hiloCliente, i + 1);
	}
	for (int i = 0; i < NUM_CLIENTES; i++) {
		clientes[i].join();
	}
	sem_destroy(&sem);
	delete[] clientes;
	EXIT_SUCCESS;
}

