#include "utils.h"

t_log *logger;

int iniciar_servidor(void)
{
	//! Quitar esta línea cuando hayamos terminado de implementar la funcion
	assert(!"no implementado!");

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	socket_servidor = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if (socket_servidor == -1)
	{
		log_error(logger, "Error al crear el socket");
		abort();
	}

	bind(sockect_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	uint32_t handshake;
	uint32_t resultOk = 0;
	uint32_t resultError = -1;

	int codigo_operacion = recibir_operacion(esperar_cliente()); // Recibe handshake del cliente

	if (codigo_operacion == 1)
	{
		send(socket_cliente, &resultOk, sizeof(uint32_t), NULL);
		log_info(logger, "Handshake recibido");
	}
	else
	{
		send(socket_cliente, &resultError, sizeof(uint32_t), NULL);
		log_error(logger, "Error en el handshake");
		exit(1);
	}

	int esperar_cliente(int socket_servidor)
	{

		// Aceptamos un nuevo cliente
		int socket_cliente;

		socket_cliente = accept(socket_servidor, NULL, NULL);

		if (socket_cliente == -1)
		{
			log_error(logger, "Error al aceptar un cliente");
			abort();
		}

		log_info(logger, "Se conecto un cliente!");

		return socket_cliente;
	}

	int recibir_operacion(int socket_cliente)
	{
		int cod_op;
		if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
			return cod_op;
		else
		{
			close(socket_cliente);
			return -1;
		}
	}

	void *recibir_buffer(int *size, int socket_cliente)
	{
		void *buffer;

		recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
		buffer = malloc(*size);
		recv(socket_cliente, buffer, *size, MSG_WAITALL);

		return buffer;
	}

	void recibir_mensaje(int socket_cliente)
	{
		int size;
		char *buffer = recibir_buffer(&size, socket_cliente);
		log_info(logger, "Me llego el mensaje %s", buffer);
		free(buffer);
	}

	t_list *recibir_paquete(int socket_cliente)
	{
		int size;
		int desplazamiento = 0;
		void *buffer;
		t_list *valores = list_create();
		int tamanio;

		buffer = recibir_buffer(&size, socket_cliente);
		while (desplazamiento < size)
		{
			memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
			desplazamiento += sizeof(int);
			char *valor = malloc(tamanio);
			memcpy(valor, buffer + desplazamiento, tamanio);
			desplazamiento += tamanio;
			list_add(valores, valor);
		}
		free(buffer);
		return valores;
	}
