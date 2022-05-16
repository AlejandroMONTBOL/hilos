#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <chrono>
#include <semaphore.h>
#define CANT_FILOS 5
using namespace std;
using namespace chrono;
mutex mtx;
sem_t sem;
class Tenedor {
public:
	bool estado;
	Tenedor() {
		this->estado = 0;
	}
	~Tenedor() {

	}
};
class Filosofo {
public:
	string nom;
	int cantCom;
	Tenedor* t1;
	Tenedor* t2;
	Filosofo(string nom, int cantCom, Tenedor* c1, Tenedor* c2)
	{
		this->nom = nom;
		this->cantCom = cantCom;
		this->t1 = c1;
		this->t2 = c2;
	}
	~Filosofo()
	{

	}
};
void hiloFilosofo(Filosofo& f) {
	while (f.cantCom > 0) {
		printf("Filosofo: %s esta pensando", f.nom.c_str());
		this_thread::sleep_for(seconds(1));
		mtx.lock();
		while (f.t1->estado != 0 || f.t2->estado != 0) {
			mtx.unlock();
			sem_wait(&sem);
			mtx.lock();
		}
		f.t1->estado = 0;
		f.t2->estado = 0;
		mtx.unlock();
		printf("Filosofo %s esta comiendo", f.nom.c_str());
		f.cantCom--;
		this_thread::sleep_for(seconds(1));
		if (f.cantCom == 0) {
			printf("Al filosofo %s se le acabo la comida", f.nom.c_str());
		}
		this_thread::sleep_for(seconds(2));
		mtx.lock();
		f.t1->estado = 0;
		f.t2->estado = 0;
		sem_post(&sem);
		mtx.unlock();
	}
}
int main() {
	thread* threads = new thread[CANT_FILOS];
	sem_init(&sem, 0, 0);
	Tenedor t1;
	Tenedor t2;
	Tenedor t3;
	Tenedor t4;
	Tenedor t5;
	Filosofo a("Jose", 7, &t1, &t2);
	Filosofo b("Nicolas", 6, &t2, &t3);
	Filosofo c("Sebastian", 8, &t3, &t4);
	Filosofo d("Camavinga", 9, &t4, &t5);
	Filosofo e("Armando paredes", 3, &t5, &t1);
	threads[0] = thread(hiloFilosofo, ref(a));
	threads[1] = thread(hiloFilosofo, ref(b));
	threads[2] = thread(hiloFilosofo, ref(c));
	threads[3] = thread(hiloFilosofo, ref(d));
	threads[4] = thread(hiloFilosofo, ref(e));
	for (int i = 0; i < CANT_FILOS; i++) {
		threads[i].join();
	}
	sem_destroy(&sem);
	delete[] threads;
	EXIT_SUCCESS;
}
