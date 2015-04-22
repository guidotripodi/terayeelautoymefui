#ifndef __SCHED_FIXED_
#define __SCHED_FIXED_

#include <stdio.h>
#include <vector>
#include <queue>
#include <list>
#include "basesched.h"


using namespace std;
typedef struct tarea {
    int pid;
    int run_time_actual;
    int periodo;
    int repeticiones;
    int cant_repeticiones_total;
} tarea_t;

class SchedFixed : public SchedBase {
	public:
		SchedFixed(std::vector<int> argn);
        	~SchedFixed();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		std::queue<int> q;
		std::list<tarea_t> tareas; //mayor periodo menor prioridad!
		std::vector<int> cores;
		int mayor; // guardamos el pid del mayor prioridad
		int indice;
		int primera_pasada;
		int next(int cpu);
		void insertarOrdenado(tarea_t tarea);
		void restarPeriodo();
};

#endif
