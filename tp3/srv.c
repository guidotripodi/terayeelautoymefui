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
    int faltan_responderme = n_ranks - 1;
    int mi_rank = mi_cliente - 1;
    int mi_numero_de_secuencia;
    int secuencia_maxima = 0;
    int buffer;
    /*INICIALIZO EN FALSE RESPUESTAS_PENDIENTES*/
    int i;
    for (i = 0; i < n_ranks; ++i) {
        respuestas_pendientes[i] = FALSE;
    }


    while( ! listo_para_salir ) {
        
        MPI_Recv(&buffer, 4, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
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
                hay_pedido_local = TRUE;

                /*INICIA CONEXION CON SERVIDOR PARA PEDIR SECCION CRITICA*/
                pedi_seccion_critica = TRUE;
                mi_numero_de_secuencia = secuencia_maxima + 1;
                for (i = 0; i < n_ranks; i = i+2){
                    MPI_Send(&mi_numero_de_secuencia, 4, MPI_INT, i, TAG_REQUEST, COMM_WORLD);
                }
            }
        }
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            for (i = 0; i < n_ranks; i++){
                if(respuestas_pendientes[i]){
                    MPI_Send(NULL, 0, MPI_INT, i*2, TAG_REPLY, COMM_WORLD);
                    respuestas_pendientes[i] = FALSE;
                }
            }
            pedi_seccion_critica = FALSE;
        }
        
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
        }
        /*me habla un servidor */
        else if (tag == TAG_REPLY) {
            faltan_responderme--;
            if(faltan_responderme == 0) {
                faltan_responderme = n_ranks - 1;
                debug("Dándole permiso (frutesco por ahora)");
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }

        }
        else if (tag == TAG_REQUEST){
            int numero_secuencia_entrante = buffer;
            if (!pedi_seccion_critica || numero_secuencia_entrante < mi_numero_de_secuencia){
                MPI_Send(NULL, 0, MPI_INT, origen, TAG_REPLY, COMM_WORLD);
            }
            else{
                respuestas_pendientes[origen/2] = TRUE;
                secuencia_maxima = numero_secuencia_entrante;
            }

        }
    }
}
    


