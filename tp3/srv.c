#include "srv.h"

/*
 *  Ejemplo de servidor que tiene el "sí fácil" para con su
 *  cliente y no se lleva bien con los demás servidores.
 *
 */

void servidor(int mi_cliente)

{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int n_ranks, mi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);

    /*VARIABLES DECLARADAS PARA CONEXION SRV SRV*/
    int tengo_salida = FALSE;
    int mi_secuencia;
    int buffer;
    int secuencia_maxima = 0;
    int cantidad_servidores;
    int clientes =  n_ranks;
    int faltan_responder[n_ranks];

    /*INICIALIZACION DE VARIABLES*/
    mi_rank = mi_rank/2;
    n_ranks = n_ranks/2; //Esta se divide por dos ya que los servidores son solo los pares
    int i;
    for (i = 0; i < n_ranks; i++){
        faltan_responder[i] = FALSE;
    }

    while( ! listo_para_salir ) {
        MPI_Recv(&buffer, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;

        /*CONEXION SRV CLI*/
        if (tag == TAG_PEDIDO) {
            assert(origen == mi_cliente);
            assert(tengo_salida == FALSE);
            assert(hay_pedido_local == FALSE);
            debug("Mi cliente solicita acceso exclusivo");
            hay_pedido_local = TRUE;
            mi_secuencia = secuencia_maxima + 1;
            cantidad_servidores = n_ranks - 1;

/*Una vez que el cliente solicita un pedido para uso de seccion critica se chequea si es el unico 
cli-srv activo o no, si es el unico se le otorga permiso, si no lo es se envia una señal de 
req a todos los servidores con el numero de secuencia */

            if (n_ranks == 1) {
                debug("Dándole permiso");
                tengo_salida = TRUE;
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            } else {
                for (i = 0; i < n_ranks; i++) {
                    if (i != mi_rank){
                        debug("Pido acceso a todos");
                        MPI_Send(&mi_secuencia, 1, MPI_INT, i*2, TAG_REQUEST, COMM_WORLD);
                    }
                }
            }
        } else if (tag == TAG_LIBERO) {

            /*Seccion cuando se liberan los recursos del cliente y se envia señales reply a todos
            los servidores que solicitaron uso exclusivo */
            assert(origen == mi_cliente);
            assert(hay_pedido_local == TRUE);
            assert(tengo_salida == TRUE);
            hay_pedido_local = FALSE;
            tengo_salida = FALSE;
            debug("Mi cliente libera su acceso exclusivo");
            for (i = 0; i < n_ranks; i++){
                if ( faltan_responder[i] == TRUE ) {
                    debug("Respondo pedidos guardados");
                    MPI_Send(NULL, 0, MPI_INT, i*2, TAG_REPLY, COMM_WORLD);
                    faltan_responder[i] = FALSE;
                }
            }
        } else if (tag == TAG_TERMINE) {
            /* Cuando finaliza el cliente se envia una señal de SIN_CLIENTE a los demas
            servidores */
            assert(origen == mi_cliente);
            assert(hay_pedido_local == FALSE);
            assert(tengo_salida == FALSE);
            assert(listo_para_salir == FALSE);
            clientes--;
            debug("Mi cliente avisa que terminó");
            for (i = 0; i < n_ranks; i++){
                if (i != mi_rank) {
                    MPI_Send(NULL, 0, MPI_INT, i*2, TAG_SIN_CLIENTE, COMM_WORLD);
                }
            }
        }

        /*CONEXION SRV SRV*/
        if (tag == TAG_REQUEST) {
            /*Si se recibe un tag del tipo request se chequea el valor de la secuencia,
            y dependiendo si el servidor que recibio el req tiene el lock activo otorga o no
            el pedido. En caso de no otorgarlo por tener hay_pedido_local activo, se 
            chequea los valores de la secuencia y rank respectivo para ver la prioridad,
            en caso de tener mayor prioridad se le otorgara por mas que tenga un pedido activo*/
            if(buffer > secuencia_maxima) secuencia_maxima = buffer;
            debug("Me pidieron el recurso");

           
            if (tengo_salida == TRUE){
                debug("Tengo el lock, tiene que esperar, guardo el pedido");
                assert(faltan_responder[origen/2] == FALSE);
                faltan_responder[origen/2] = TRUE;
            } else if(!hay_pedido_local) {
                debug("Lo otorgo por que no lo necesito");
                MPI_Send(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD);
            } else {
                if (buffer < mi_secuencia || (buffer == mi_secuencia && (origen/2) < mi_rank) ) {
                    debug("Lo otorgo por que tiene mayor prioridad");
                    MPI_Send(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD);
                } else {
                    debug("Tiene menor prioridad, no lo otorgo y guardo el pedido");
                    assert(faltan_responder[origen/2] == FALSE);
                    faltan_responder[origen/2] = TRUE;
                }
            }
        } else if (tag == TAG_REPLY) {

            /*Si se recibe un reply, se chequea la cantidad que ya se obtuvieron, en caso de
            tener todos los reply de los srv se otorga el permiso al cliente*/
            assert(tengo_salida == FALSE); 
            assert(hay_pedido_local == TRUE);
            cantidad_servidores --;
            if (cantidad_servidores == 0) {
                tengo_salida = TRUE;
                debug("Dándole permiso");
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
        } else if (tag == TAG_SIN_CLIENTE) {
            clientes--;
        }
        
        if (clientes == n_ranks) {
            listo_para_salir = TRUE;
        }
    }
    /*ESTO LO HICE YA QUE AL NO TENER CLIENTE EL SERVER QUEDA AL PEDO Y NO OTORGA NADA*/
/*
    while(clientes > 0) {
        MPI_Recv(&buffer, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;

        if (tag == TAG_REQUEST) {
            debug("Otorgo por que ya no tengo cliente");
            MPI_Send(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD);
        }

        if (tag == TAG_SIN_CLIENTE) {
            clientes--;
        }
    }*/

}
