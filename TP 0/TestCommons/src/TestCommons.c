/*
 ============================================================================
 Name        : TestCommons.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>

int main(void) {
	char * string = string_new();
	string_append(&string, "!!!Hello World!!!");
	puts(string); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
