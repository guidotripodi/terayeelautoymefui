#ifndef __SCHED_DYNAMIC_
#define __SCHED_DYNAMIC_

#include <vector>
#include <stdio.h>
#include <queue>
#include <list>
#include "basesched.h"

using namespace std;

typedef struct tarea_dinamic {
    int pid;
    int run_time_actual;
    int periodo;
    int repeticiones;
    int deadline;
} tarea_d;


class SchedDynamic : public SchedBase {
	public:
		SchedDynamic(std::vector<int> argn);
        	~SchedDynamic();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		std::queue<int> q;
		std::list<tarea_d> tareas; 
		int primera_pasada;
		int next(int cpu);
		void insertarOrdenado(tarea_d tarea);
		bool chequearPeriodos();
};

#endif
