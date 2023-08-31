#include "utils.h"

void* serializar_paquete(t_paquete* paquete, int bytes){
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	// Serializar los campos que tenemos en el buffer.
	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Devolver informacion de red sobre la IP y PUERTO.
	getaddrinfo(ip, puerto, &hints, &server_info);

	// Crear el socket del cliente.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Conectar con el socket creado.
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)){
		// Si el servidor no esta en accept, se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en la conexion del cliente al servidor");
		exit(4);
	}

	// Liberar la memoria del server_info.
	freeaddrinfo(server_info);

	// Devolver el socket del cliente.
	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente){
	// Crear  y armar el paquete a enviar.
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	// Tamanio de bytes del mensaje a enviar.
	int bytes = paquete->buffer->size + 2*sizeof(int);

	// Serializar el mensaje a enviar.
	void* a_enviar = serializar_paquete(paquete, bytes);

	// Enviar mensaje por el socket.
	if(send(socket_cliente, a_enviar, bytes, 0) == -1){
		// Si send devuelve un -1 (failure), se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en el envio del mensaje por socket");
		exit(5);
	}

	// Liberar la memoria del mensaje y el paquete ya enviado.
	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente){
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	// Enviar paquete por el socket.
	if(send(socket_cliente, a_enviar, bytes, 0) == -1){
		// Si send devuelve un -1 (failure), se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en el envio del mensaje por socket");
		exit(5);
	}

	// Liberar la memoria del paquete ya enviado.
	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente){
	// Cerrar el socket del cliente.
	close(socket_cliente);
}
