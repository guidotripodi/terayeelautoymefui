#include "sched_dynamic.h"
#include <iostream>
#include <iterator>  

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn) {
}

SchedDynamic::~SchedDynamic() {
}

void SchedDynamic::initialize() {
		primera_pasada =1;
		tareas.clear();
}

void SchedDynamic::load(int pid) {
	tarea_d nueva_tarea;
	nueva_tarea.pid = pid;
	nueva_tarea.run_time_actual = declared_cputime(pid);
	nueva_tarea.deadline = period(pid) - declared_cputime(pid);
	//printf("Tarea: %i\n", pid);
	//printf("Deadline: %i\n", nueva_tarea.deadline);
	//printf("ESPACIOOOO%s\n");
	nueva_tarea.periodo = period(pid);
	if (!tareas.empty()){
		insertarOrdenado(nueva_tarea);
	}else{
		tareas.push_back(nueva_tarea);
	}
}


void SchedDynamic::unblock(int pid) {
}

int SchedDynamic::tick(int cpu, const enum Motivo m) {
	std::list<tarea_d>::iterator it;
	std::list<tarea_d>::iterator it2;
	switch (m) {
		case EXIT:
			it = tareas.begin();
			/* SI TERMINO LA TAREA CHEQUEO SI FUE SU ULTIMA REPETICION O NO
			SI NO FUE LA ULTIMA DESCUENTO UNA REPETICION Y PONGO EL
			PERIODO EN SU VALOR INICIAL */
			for (it=tareas.begin(); it!=tareas.end(); ++it)	{
				if (it->pid == current_pid(cpu)){
						it2 = it;
						it2++;
						tareas.erase(it,it2);	
			for (it=tareas.begin(); it!=tareas.end(); ++it){
				}
					return next(cpu); 
				}
			}
			break;
		case BLOCK:
		case TICK:
		/* SI TERMINO EL RUN TIME DE LA TAREA ESTANDO BLOQUEADA CHEQUEO 
			SI FUE SU ULTIMA REPETICION O NO
			SI NO FUE LA ULTIMA DESCUENTO UNA REPETICION Y PONGO EL
			PERIODO EN SU VALOR INICIAL 
			ADEMAS RESTO EL PERIODO DE LAS QUE YA CORRIERON ALGUNA VEZ*/
			tarea_d tarea_desalojada;
			if (primera_pasada == 1){
				primera_pasada = 0;
				return next(cpu);
			}else{
				for (it=tareas.begin(); it!=tareas.end(); ++it)	{
					if (it->pid == current_pid(cpu)){
						it->run_time_actual--;
						it->periodo--;
					}else{
						it->periodo--;
						it->deadline--;
					}
				}
				if(chequearPeriodos()){
					if (current_pid(cpu) != -1){
							return next(cpu);
					}else{
							for (it=tareas.begin(); it!=tareas.end(); ++it)	{
								if (it->pid == current_pid(cpu)){
									it2 = it;
									tarea_desalojada = *it;
									it2++;
									tareas.erase(it,it2);
								}
							}
							insertarOrdenado(tarea_desalojada);
							return next(cpu);
						}
				}else{
					return current_pid(cpu);
					}
			}
			break;
	
	
		}
	return 0;
}
	


int SchedDynamic::next(int cpu){
	/* SI LA LISTA DE TAREAS SE VACIO VOY A LA IDLE SINO RECORRO Y ME FIJO
	CUAL ES EL PRIMERO CON runtime casi igual al periodo */
	std::list<tarea_d>::iterator it;
	int pid =0;
	if (tareas.empty()){ 
		pid= IDLE_TASK;  //no hay mas tareas -.-> idle_task
	}else{
		pid = tareas.begin()->pid;
	}
	return pid;
}

bool SchedDynamic::chequearPeriodos(){
		if (tareas.begin()->deadline <= 0)	{ //si tenemos problemas con la vulta de los ciclos cambiar esto a 0
			return true;
		}else{
			return false;
		}
}


void SchedDynamic::insertarOrdenado(tarea_d tarea){
		bool inserto = false;
		std::list<tarea_d>::iterator it;
		for (it=tareas.begin(); it!=tareas.end(); ++it)	{
			if (it->deadline > tarea.deadline){
				tareas.insert(it, tarea);
				inserto = true;
				break;
			}
		}
		if (!inserto) {
			tareas.push_back(tarea);
		}
}	
