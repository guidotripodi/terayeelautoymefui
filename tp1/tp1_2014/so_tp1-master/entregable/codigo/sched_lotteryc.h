#ifndef __SCHED_LOT__
#define __SCHED_LOT__

#include "basesched.h"


class SchedLotteryC : public SchedBase {
  public:
    SchedLottery(std::vector<int> argn);
    ~SchedLottery();
    virtual void load(int pid);
    virtual void load(int pid, int deadline);
    virtual void unblock(int pid);
    virtual int tick(int cpu, const enum Motivo m);

private:
	std::vector<int> quantumActual;
	std::vector<int> process;
	std::vector<int> processTickets;
	int semilla;
	int tickets;
	void sacar(int pid);
	int next(int cpu);
};

#endif
