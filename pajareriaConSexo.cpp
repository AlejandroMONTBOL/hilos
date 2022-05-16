#include <iostream>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <chrono>
#include <semaphore.h>
#define MAX_COMIENDO 3
#define NUM_CANARIOS 5
using namespace std;
using namespace chrono;

mutex mtx;
sem_t sem;
//variables compartidas o de estado
int comiendo = 0;
int columpiando = 0;
class Canario {
public:
	string nombre;
	char sexo;
	Canario(string nom, char sexo)
	{
		this->nombre = nom;
		this->sexo = sexo;
	}
	~Canario()
	{

	}

};
class Columpio {
public:
	Canario* hembra;
	Canario* macho;
	Columpio(Canario *hembra, Canario * macho) {
		this->hembra = hembra;
		this->macho = macho;
	}
	~Columpio()
	{

	}

};
Columpio columpio(NULL, NULL);//columpio es variable global
void comer(Canario& canario) {
	mtx.lock();
	while (comiendo == MAX_COMIENDO) {
		mtx.unlock();
		sem_wait(&sem);
		mtx.lock();
	}
	comiendo++;
	printf("Canarios comiendo: %d. \n", comiendo);
	mtx.unlock();
	printf("%s esta comiendo.\n", canario.nombre.c_str());
	this_thread::sleep_for(seconds(2));
	mtx.lock();
	comiendo--;
	sem_post(&sem);
	mtx.unlock();
}
void columpiar(Canario& canario) {
	bool esHembra = canario.sexo == 'H';
	bool esMacho = !esHembra;
	mtx.lock();
	while (esHembra and columpio.hembra != NULL || esMacho and columpio.macho != NULL) {
		mtx.unlock();
		sem_wait(&sem);
		mtx.lock();
	}
	if (esHembra)
	{
		columpio.hembra = &canario;
	}else{
		columpio.macho = &canario;
	}
	columpiando++;
	printf("Hay %d pajaros(s) columpiando \n", columpiando);
	mtx.unlock();
	printf("%s esta columpiando, su sexo es: %s \n", canario.nombre.c_str(),canario.sexo);
	this_thread::sleep_for(seconds(4));
	mtx.lock();
	if (esHembra)
	{
		columpio.hembra = NULL;
	}
	else {
		columpio.macho = NULL;
	}
	columpiando--;
	sem_post(&sem);
	mtx.unlock();

}
void hiloCanario(Canario& c) {
	while (true) {
		comer(c);
		columpiar(c);
	}

}
int main() {
	thread* threads = new thread[NUM_CANARIOS];
	sem_init(&sem, 0, 0); //el ultimo 0 nos dice que no es un semaforo compartid
	Canario c("Juan", 'M');
	Canario c1("Andrea", 'H');
	Canario c2("Samantha", 'H');
	Canario c3("Andres", 'M');
	Canario c4("Joan", 'M');
	threads[0] = thread(hiloCanario, ref(c));//ref retorna la direccion de memoria del objeto que le estamos pasando, protege esa seccion de memoria, lo usamos por que puede que cuando le pasemos un canario a un hilo, su direccion de memoria haya cambiado
	//Esperar a que los hilos terminen su ejecucion
	threads[1] = thread(hiloCanario, ref(c1));
	threads[2] = thread(hiloCanario, ref(c2));
	threads[3] = thread(hiloCanario, ref(c3));
	threads[4] = thread(hiloCanario, ref(c4));
	for (int i = 0; i < NUM_CANARIOS; i++)
	{
		threads[i].join();
	}
	//liberamos recursos
	sem_destroy(&sem);
	delete[] threads;
	return EXIT_SUCCESS;

}
