#!/bin/bash
# -*- ENCODING: UTF-8 -*-

clear 

#en el for le ponemos desde donde hasta donde (cuantos casos)

for i in {1..10}
	do	
	#aca podemos poner la instruccion que quieren que se repita. 
	#con $i ponemos el valor de $i
	./simusched lote.tsk 1 1 1 SchedLottery 4 $i | ./graphsched.py >> img$i.png 
done 



exit