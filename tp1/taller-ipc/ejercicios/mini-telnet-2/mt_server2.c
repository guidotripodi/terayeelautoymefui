#include "mt.h"

int main(int argc, char* argv[]) {

	int sock, sock_write, t;
	struct sockaddr_in name;
	char buf[MAX_MSG_LENGTH];

	/* Crear socket sobre el que se lee: dominio INET, protocolo TCP (STREAM). */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("abriendo socket");
		exit(1);
	}

	/* Crear nombre, usamos INADDR_ANY para indicar que cualquiera puede enviar aquí. */
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = INADDR_ANY;
	name.sin_port = htons(PORT);
	if (bind(sock, (void*) &name, sizeof(name))) {
		perror("binding datagram socket");
		exit(1);
	}
	//escucho al socket q recibo
	 if (listen(sock, 1) < 0) {
        perror("escuchando");
        exit(1);
  	 }
  	 t = sizeof(name);
  	 sock_write = accept(sock, (struct sockaddr*) &name, (socklen_t*) &t);


	/* Recibimos mensajes hasta que alguno sea el que marca el final. */
	for (;;) {
		ssize_t n = recv(sock_write, buf, MAX_MSG_LENGTH, 0);
		buf[n] = 0; //termina la cadena
		
		if (strncmp(buf, END_STRING, MAX_MSG_LENGTH) == 0)
			break;

		/* Reemplazo stdout y stderr por el socket de escritura. */
        dup2(sock_write, STDOUT_FILENO);
        dup2(sock_write, STDERR_FILENO);

        /* Ejecuto el comando recibido. */
        system(buf);

        /* Fuerzo la escritura de los buffers de stdout y stderr. */
        fflush(stdout);
        fflush(stderr);
	}

	/* Cerrar socket de recepción. */
	close(sock);
	close(sock_write);

	return 0;
}

