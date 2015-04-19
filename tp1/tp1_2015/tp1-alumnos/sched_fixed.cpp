#include "sched_fixed.h"
#include <iostream>
#include <iterator>   

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
}

SchedFixed::~SchedFixed() {
}

void SchedFixed::initialize() {
	mayor = 0;
}

void SchedFixed::load(int pid) {
	tarea_t nueva_tarea;
	nueva_tarea.pid = pid;
	nueva_tarea.run_time_actual = declared_cputime(pid);
	nueva_tarea.periodo = period(pid);
	if (period(mayor)< period(pid))	{
		mayor = pid;
		tareas.push_back(nueva_tarea);
	}else{
		insertarOrdenado(nueva_tarea);
	}
}

void SchedFixed::unblock(int pid) {
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
	return -2;
}

void SchedFixed::insertarOrdenado(tarea_t tarea){
	std::list<tarea_t>::iterator it;
	it = tareas.begin();
	for (it=tareas.begin(); it!=tareas.end(); ++it)	{
		if (period(it->pid) < period(tarea.pid)){
			tareas.insert(it, tarea);
		}
	}
}