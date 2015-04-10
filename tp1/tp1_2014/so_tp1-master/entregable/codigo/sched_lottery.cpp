#include "sched_lottery.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;

SchedLottery::SchedLottery(vector<int> argn) {
	quantumTotal = argn[1];

	for (int i = 0; i < argn[0]; i++){
		quantumActual.push_back(quantumTotal);
	}

	srand(argn[2]);
}

SchedLottery::~SchedLottery() {
}

void SchedLottery::load(int pid) {
  load(pid, 0);
}

void SchedLottery::load(int pid, int deadline) {
	// Lo agrego a la lista de procesos
	process.push_back(pid);
	int nuevoTotal = process.size();

	tickets = quantumTotal * nuevoTotal * 1000;
	int ticketsProceso = tickets/nuevoTotal;

	processTickets.push_back(ticketsProceso);

	for (int i = 0; i < nuevoTotal; i++) {
		processTickets[i] = ticketsProceso;
	}
}

void SchedLottery::unblock(int pid) {
	// SIN COMPENSATION
	// Por ahora lo vuelvo a cargar de nuevo como un proceso nuevo
	// Se le asignara la cantidad de tickets proporcional
	//load(pid);


	// CON COMPENSATION
	// a los procesos que se bloquean, los agregamos mas de una vez
	// (la cantidad de ticks antes de que se complete el quantum)
	// asi se implementa la optimizacion de compensation
	int i, j, length = bloqueados.size();
	for (i = 0; i < length; i++) {
		if (bloqueados[i] == pid) {
			for (j = 0; j < bloqueadosQuantumConsumido[i]; j++) {
				load(pid);
			}
			// lo borramos de la lista de bloqueados
			bloqueados.erase(bloqueados.begin() + i);
			bloqueadosQuantumConsumido.erase(bloqueadosQuantumConsumido.begin() + i);
		}
	}
}

int SchedLottery::tick(int cpu, const enum Motivo m) {
	switch (m) {
		case EXIT:
			return next(cpu);
			break;
		case BLOCK:
			quantumActual[cpu]--;
			bloqueados.push_back(current_pid(cpu));
			bloqueadosQuantumConsumido.push_back(quantumActual[cpu]);
			return next(cpu);
			break;
		case TICK:
			if (current_pid(cpu) == IDLE_TASK) {
				return next(cpu);
			} else {
				quantumActual[cpu]--;
				if (quantumActual[cpu] == 0) {
					load(current_pid(cpu));
					return next(cpu);
				} else {
					return current_pid(cpu);
				}
			}
			break;
	}
}

void SchedLottery::sacar(int pid) {
	int length = process.size(), i;

	for (i = 0; i < length; i++) {
		if (process[i] == pid) {
			process.erase(process.begin() + i);
			processTickets.erase(processTickets.begin() + i);
			if (process.size() == i) {
				break;
			}

			i--;

		}
	}

	int nuevoTotal = process.size();

	if (nuevoTotal > 0) {
		tickets = quantumTotal * nuevoTotal * 1000;
		int ticketsProceso = tickets/nuevoTotal;

		for (i = 0; i < nuevoTotal; i++) {
			processTickets[i] = ticketsProceso;
		}
	}
}

int SchedLottery::next(int cpu) {
	int ticketGanador, procesoGanador, suma = -1, length = processTickets.size();
	
	if (length > 0) {
		ticketGanador = rand()%(tickets + 1);
		quantumActual[cpu] = quantumTotal;

		for (int i = 0; i < length; i++) {
			suma += processTickets[i];

			if (suma >= ticketGanador) {
				procesoGanador = process[i];
				break;
			}
		}
		// lo saco de la lista de procesos disponibles
		sacar(procesoGanador);
		return procesoGanador;
	} else  {
		return IDLE_TASK;
	}

}