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
    int n_ranks;

    MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    int cantidad_servidores = n_ranks/2;

    /*VARIABLES PARA COMUNICARSE CON SERVIDORES*/
    int pedi_seccion_critica = FALSE;
    int respuestas_pendientes[n_ranks];
    int faltan_responderme = cantidad_servidores - 1;
    int mi_rank = mi_cliente - 1;
    int mi_numero_de_secuencia;
    int secuencia_maxima = 0;
    int buffer;
    int tengo_salida = FALSE;
    MPI_Request request;
    /*INICIALIZO EN FALSE RESPUESTAS_PENDIENTES*/
    int i;
    for (i = 0; i < n_ranks; ++i) {
        respuestas_pendientes[i] = FALSE;
    }


    while( ! listo_para_salir ) {
        
        MPI_Recv(&buffer, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;

            /*Me habla mi cliente*/
        if (tag == TAG_PEDIDO) {
            if (n_ranks == 2){
                  assert(origen == mi_cliente);
                  debug("Mi cliente solicita acceso exclusivo");
                  assert(hay_pedido_local == FALSE);
                  hay_pedido_local = TRUE;
                  debug("Dándole permiso (frutesco por ahora)");
                  MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }else{
                assert(origen == mi_cliente);
                debug("Mi cliente solicita acceso exclusivo");
                assert(hay_pedido_local == FALSE);
                assert(tengo_salida == FALSE);
                hay_pedido_local = TRUE;

                /*INICIA CONEXION CON SERVIDOR PARA PEDIR SECCION CRITICA*/
                pedi_seccion_critica = TRUE;
                mi_numero_de_secuencia = secuencia_maxima + 1;
                for (i = 0; i < n_ranks; i = i+2){
                    if (i != mi_rank){
                        debug("Pido acceso a todos");
                        MPI_Isend(&mi_numero_de_secuencia, 1, MPI_INT, i, TAG_REQUEST, COMM_WORLD, &request);
                    }

                }
            }
        }
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (i = 0; i < n_ranks; i++){
                if(respuestas_pendientes[i] == TRUE){
                    if (i != mi_rank) {
                        MPI_Isend(NULL, 0, MPI_INT, i*2, TAG_REPLY, COMM_WORLD, &request);
                        respuestas_pendientes[i] = FALSE;
                        printf("el servidor da %d\n", mi_rank);
                    }
                }
            }
            tengo_salida = FALSE;
            pedi_seccion_critica = FALSE;
        }
        
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
        }
        /*me habla un servidor */
        else if (tag == TAG_REPLY) {
            printf("llega aca? %d\n", mi_rank);
            assert(hay_pedido_local == TRUE);
            assert(tengo_salida == FALSE);
            faltan_responderme--;
            if(faltan_responderme == 0) {
                faltan_responderme = cantidad_servidores - 1;
                tengo_salida = TRUE;
                debug("Dándole permiso (frutesco por ahora)");
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }

        }
        else if (tag == TAG_REQUEST){
            printf("aca si? %d\n",mi_rank );
            int numero_secuencia_entrante = buffer;
            if (!pedi_seccion_critica){
                debug("Lo otorgo por que no lo necesito");
                MPI_Isend(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD, &request);
            }
            else {
                if (numero_secuencia_entrante < mi_numero_de_secuencia || (numero_secuencia_entrante == mi_numero_de_secuencia && origen < mi_rank)) {
                    debug("Lo otorgo xq tiene MAYOR prioridad");                   
                    MPI_Isend(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD, &request);
                }
                else {
                    printf("viene aca el servidor:%d\n", mi_rank);
                    debug("No lo otorgo, tiene que esperar");
                    assert(respuestas_pendientes[origen/2] == FALSE);
                    respuestas_pendientes[origen/2] = TRUE;
                    secuencia_maxima = numero_secuencia_entrante;
                }
            }
        }
    }
}


