#include "client.h"
#include "readline/readline.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	char *ip;
	char *puerto;
	char *valor;

	int conexion;

	t_log *logger;
	t_config *config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	log_info(logger, "Hola! Soy un log");
	printf("hola");
	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config
	log_info(logger, "%s", valor); //! Le paso %s por temas de seguridad en C

	/* ---------------- LEER DE CONSOLA ---------------- */

	// leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje "Handshake"

	uint32_t handshake = 1;
	uint32_t result;

	send(conexion, &handshake, sizeof(uint32_t), NULL);		// Manda handshake al servidor
	recv(conexion, &result, sizeof(uint32_t), MSG_WAITALL); // Recibe handshake del servidor

	if (result == 0)
	{
		log_info(logger, "Handshake recibido");
	}
	else
	{
		log_error(logger, "Error en el handshake");
		exit(1);
	}

	// TODO: Armamos y enviamos el paquete

	enviar_mensaje(valor, conexion);
	paquete(conexion);
	terminar_programa(conexion, logger, config);

	valor = NULL;
	ip = NULL;
	puerto = NULL;

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log *iniciar_logger(void)
{
	t_log *nuevo_logger;

	nuevo_logger = log_create("tpo.log", "tp0", true, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el Logger");
		exit(1);
	}

	return nuevo_logger;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config = NULL;

	nuevo_config = config_create("cliente.config");

	if (nuevo_config == NULL)
	{
		printf("No se pudo crear el config! \n");
		exit(1);
	}

	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	char *leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strlen(leido) > 0)
	{
		log_info(logger, "%s", leido);
		free(leido);
		leido = readline("> ");
	}

	free(leido);

	leido = NULL;
}

void paquete(int socket_cliente)
{
	// Ahora toca lo divertido!
	char *leido;
	t_paquete *paquete;
	{
		// Ahora toca lo divertido!
		char *leido;
		t_paquete *paquete;

		paquete = crear_paquete();

		leido = readline("> ");

		while (strlen(leido) > 0)
		{

			// Agrega lineas leidas al buffer
			agregar_a_paquete(paquete, leido, strlen(leido) + 1); // +1 para el \0
			free(leido);
			leido = readline("> ");
		}

		// Envia Buffer
		enviar_paquete(paquete, socket_cliente);
		eliminar_paquete(paquete);

		free(leido);

		// Seteamos en NULL para que no se libere dos veces
		leido = NULL;
		paquete = NULL;
	}
}
void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}
