#include "server.h"

int main(void)
{

	uint32_t handshake;
	uint32_t resultOk = 0;
	uint32_t resultError = -1;

	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	// Iniciar server
	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");

	// Aceptar cliente
	int cliente_fd = esperar_cliente(server_fd);

	// Handshake
	handshake = recibir_operacion(cliente_fd);

	if (handshake == 1)
	{
		send(cliente_fd, &resultOk, sizeof(uint32_t), NULL);
		log_info(logger, "Handshake recibido");
	}
	else
	{
		send(cliente_fd, &resultError, sizeof(uint32_t), NULL);
		log_error(logger, "Error en el handshake");
		exit(1);
	}

	// Recibir mensajes y guardarlos en el log
	
	t_list *lista;
	while (1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op)
		{
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void *)iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char *value)
{
	log_info(logger, "%s", value);
}
