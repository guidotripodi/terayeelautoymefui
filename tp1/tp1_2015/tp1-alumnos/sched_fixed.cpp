#include "sched_fixed.h"
#include <iostream>
#include <iterator>   

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
	for (int i = 0; i < argn[0]; i++){
		cores.push_back(-1); //todos los cores arrancan vacíos
	}
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
	nueva_tarea.periodo = 0;// el periodo este va en 0 la tomamos q entra en ready
	//cuando corra una vez ahi si el periodo pasa al valor real
	if (period(mayor) < period(pid))	{
		mayor = pid;
		tareas.push_back(nueva_tarea);
	}else{
		insertarOrdenado(nueva_tarea);
	}
}

void SchedFixed::unblock(int pid) {
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
	std::list<tarea_t>::iterator it;
	std::list<tarea_t>::iterator it2;
	//tarea_t tarea;
	switch (m) {
		case EXIT:
			it = tareas.begin();
			/* SI TERMINO LA TAREA CHEQUEO SI FUE SU ULTIMA REPETICION O NO
			SI NO FUE LA ULTIMA DESCUENTO UNA REPETICION Y PONGO EL
			PERIODO EN SU VALOR INICIAL */
			for (it=tareas.begin(); it!=tareas.end(); ++it)	{
				if (it->pid == current_pid(cpu)){
					if (it->repeticiones == 0){
						it2 = it;
						tareas.erase(it,it2);
					}else{
						it->repeticiones = it->repeticiones-1;
						it->periodo = period(it->pid);
					}
				}
			}
			return next(cpu);
			break;
		case BLOCK:
			/* SI TERMINO EL RUN TIME DE LA TAREA ESTANDO BLOQUEADA CHEQUEO 
			SI FUE SU ULTIMA REPETICION O NO
			SI NO FUE LA ULTIMA DESCUENTO UNA REPETICION Y PONGO EL
			PERIODO EN SU VALOR INICIAL*/ 
			it = tareas.begin();
			for (it=tareas.begin(); it!=tareas.end(); ++it)	{
				if (it->pid == current_pid(cpu)){
					if (it->run_time_actual == 0){
						if (it->repeticiones == 0){
							it2 = it;
							tareas.erase(it,it2);
						}else{
							it->repeticiones = it->repeticiones-1;
							it->periodo = period(it->pid);
						}
					}

				}
			}
			return next(cpu);
			break;
		case TICK:
			/* SI TERMINO EL RUN TIME DE LA TAREA ESTANDO BLOQUEADA CHEQUEO 
			SI FUE SU ULTIMA REPETICION O NO
			SI NO FUE LA ULTIMA DESCUENTO UNA REPETICION Y PONGO EL
			PERIODO EN SU VALOR INICIAL 
			ADEMAS RESTO EL PERIODO DE LAS QUE YA CORRIERON ALGUNA VEZ*/
			it = tareas.begin();
			for (it=tareas.begin(); it!=tareas.end(); ++it)	{
				if (it->pid == current_pid(cpu)){
					if (it->run_time_actual == 0){
						if (it->repeticiones == 0){
							it2 = it;
							tareas.erase(it,it2);
						}else{
							it->repeticiones = it->repeticiones-1;
							it->periodo = period(it->pid);
						}
						next(cpu);
					}else{
						it->run_time_actual = it->run_time_actual -1;
						restarPeriodo(); //resto periodo de las que ya corrieron alguna vez
						return current_pid(cpu);					
					}

				}
			}
			break;
	}
			return 0;
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

void SchedFixed::restarPeriodo(){
	/* SI LA TAREA YA CORRIO ALGUNA VEZ LE RESTO PERIODO SIEMPRE
	QUE NO ESTE YA EN 0 PARA Q NO PASE A NEGATIVO*/
	std::list<tarea_t>::iterator it;
	it = tareas.begin();
	for (it=tareas.begin(); it!=tareas.end(); ++it)	{
		if (it->repeticiones < it->cant_repeticiones_total){
			if (it->periodo > 0) {
				it->periodo--;
			}
					
		}
	}
}

int SchedFixed::next(int cpu){
	/* SI LA LISTA DE TAREAS SE VACIO VOY A LA IDLE SINO RECORRO Y ME FIJO
	CUAL ES EL PRIMERO CON PERIODO = 0*/
	int pid ;
	if (tareas.empty()){ 
		pid= IDLE_TASK;  //no hay mas tareas -.-> idle_task
	}else {
		std::list<tarea_t>::iterator it;
		it = tareas.begin();
		for (it=tareas.begin(); it!=tareas.end(); ++it)	{
			if (it->periodo == 0){
				pid = it->pid;
				it = tareas.end();
			}
		}
		cores[cpu] = pid;
		return pid;
	}
}