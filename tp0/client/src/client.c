#include "client.h"

/* Codigos de exit
 * 1 - Error en la creacion del log.
 * 2 - Error en la creacion del config.
 * 3 - Error en la existencia de una KEY.
 * 4 - Error en la conexion del cliente al servidor.
 * 5 - Error en el envio del mensaje o paquete por socket.
*/

int main(void){
	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;


	/* ------------------------------------------------ LOGGING ------------------------------------------------ */
	// Crear e iniciar logger.
	logger = iniciar_logger();

	// Loggear "Hola! Soy un log"
	log_info(logger, "Hola! Soy un log");


	/* --------------------------------------- ARCHIVOS DE CONFIGURACION --------------------------------------- */
	// Crear e iniciar config.
	config = iniciar_config();

	// Leer los valores del config y almacenar en las variables 'ip', 'puerto' y 'valor'.
	// Verificar las existencias de la KEY en la config.
	if(!config_has_property(config, "IP") ||  !config_has_property(config, "PUERTO") || !config_has_property(config, "CLAVE")){
		// Si no existe alguna de las KEYS, se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en la existencia de una KEY");
		exit(3);
	}
	// Si existen las KEYS, se almacenan el las variables correspondientes.
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggear el valor de la CLAVE del config.
	log_info(logger, "CLAVE: %s", valor);

	/* -------------------------------------------- LEER DE CONSOLA -------------------------------------------- */
	// Leer de consola.
	leer_consola(logger);

	/* ------------------------------------------- CLIENTE - SERVIDOR ------------------------------------------ */
	// ADVERTENCIA: Antes de continuar, asegurar que el servidor esté corriendo para poder conectarse a él.

	// Crear una conexión hacia el servidor.
	conexion = crear_conexion(ip, puerto);

	// Enviar al servidor el valor de CLAVE como mensaje.
	enviar_mensaje(valor, conexion);

	// Armar y enviar el paquete.
	paquete(conexion);

	// Terminamos el programa.
	terminar_programa(conexion, logger, config);
}

t_log* iniciar_logger(void) {
	// Se utilizara log_create para generar los logs.
	/* Sus parametros son los siguiente:
		- 1º parametro: file (Direccion del archivo log)
		- 2º parametro: process name (Nombre del log)
		- 3º parametro: is active console (0 -> no, 1 -> si)
		- 4º parametro: log level (Nivel de detalle del log)
	*/
	t_log* nuevo_logger = log_create("tp0.log", "TP0", 1, LOG_LEVEL_INFO);

	// Revisar que el log no sea NULL.
	if(nuevo_logger == NULL){
		// Si es igual a NULL, se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en la creacion del log");
		exit(1);
	}
	// Si es distinto a NULL, se retorna una instancia de log.
	return nuevo_logger;
}

t_config* iniciar_config(void){
	// Se utilizara config_create para generar las configs.
	/* Su parametro es el siguiente:
		- 1º parametro: patch (Direccion del archivo patch)
	*/
	t_config* nuevo_config = config_create("cliente.config");

	// Revisar que la config no sea NULL.
	if(nuevo_config == NULL){
		// Si es igual a NULL, se lanzara por consola un 'Error' y terminara el programa.
		printf("Error en la creacion del config");
		exit(2);
	}
	// Si es distinto de NULL, se retornara una instancia de config.
	return nuevo_config;
}

void leer_consola(t_log* logger){
	char* leido;

	// Leer de consola hasta recibir un string vacio.
	while(1){
		// Hacer que el programa espere a que se ingrese una línea y devolverla en un string ya listo para loggear.
		leido = readline("> ");

		// Si se ingresa un string vacio, se interrumpe el while.
		if(!strcmp(leido, "")) break;

		// Loggear hasta recibir un string vacío.
		log_info(logger, "Se leyo el String: %s", leido);
		// Liberar la linea de memoria leido.
		free(leido);
	}

	// Liberar la linea de memoria de leido.
	free(leido);
}

void paquete(int conexion){
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leer y agregar las lineas al paquete.
	while(1){
		// Hacer que el programa espere a que se ingrese una línea y devolverla en un string ya listo para loggear.
		leido = readline("> ");

		// Si se ingresa un string vacio, se interrumpe el while.
		if(!strcmp(leido, "")) break;

		// Agregar el String al paquete.
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		// Liberar la linea de memoria leido.
		free(leido);
	}

	// Enviar paquete.
	enviar_paquete(paquete, conexion);
	
	// Liberar las líneas y el paquete antes de regresar!
	free(leido);
	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config){
	// Si existe un log, lo destruimos.
	if(logger != NULL) log_destroy(logger);

	// Si existe una config, lo detruimos.
	if(config != NULL) config_destroy(config);

	// Si existe una conexion, lo liberamos.
	if(conexion) liberar_conexion(conexion);
}
