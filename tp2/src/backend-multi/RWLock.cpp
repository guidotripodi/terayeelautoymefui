#include "RWLock.h"

RWLock :: RWLock() {
}

void RWLock :: rlock() {
	/*
	1 turnstile.wait()
2 turnstile.signal()
3
4 readSwitch.lock(roomEmpty)
5 # critical section for readers
6 readSwitch.unlock(roomEmpty)*/

}

void RWLock :: wlock() {

/*	turnstile.wait()
2 roomEmpty.wait()
3 # critical section for writers
4 turnstile.signal()
5
6 roomEmpty.signal()*/
}

void RWLock :: runlock() {
	/*def lock(self, semaphore):
7 self.mutex.wait()
8 self.counter += 1
9 if self.counter == 1:
10 semaphore.wait()
11 self.mutex.signal()*/
}

void RWLock :: wunlock() {
/*def unlock(self, semaphore):
14 self.mutex.wait()
15 self.counter -= 1
16 if self.counter == 0:
17 semaphore.signal()
18 self.mutex.signal()*/
}

