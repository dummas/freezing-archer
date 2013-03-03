#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#include "main.h"
#include "so_tcplib.h"

int main ( int argc, char *argv[] )
{

  if ( argc < 3 )
    {
      error ("Error on providing server information.\nUsage: cus-client [IP] [port]");
    }
  
  // Setting server hostname/IP
  char *server_host = argv[1];
  // Setting server port
  int server_port = atoi(argv[2]);
  // Input buffer
  char buffer[255];
  // Input buffer string ( needed for fgets )
  char *input;
  // Socket return
  int n = 0;

  pthread_t thread;
  
  server_socket = tcpConnectSocket ( server_host, server_port );
  
  if ( server_socket < 0 )
    {
      error ( "Error on connecting to server" );
    }
  
  printf("Connected successfully\n");

  n = 1;
  pthread_create (&thread, NULL, catch_server_stream, (void *) n );

  /*
  n = 2;
  pthread_create (&thread, NULL, catch_client_stream, (void *) n );
  */

  for (;;)
    {
      fgets ( buffer, 255, stdin );
      n = write ( server_socket, buffer, strlen ( buffer ) );
    }

  return 0;
  
}

/**
 * Printing error message and exit
 */
void error ( char *message )
{
  printf("%s\n", message );
  exit(1);
}

void *catch_server_stream ( void * id )
{
  char buffer[600];
  int n;

  for (;;)
    {
      n = read ( server_socket, buffer, 600  );
      if ( n < 0 )
	{
	  printf("Connection closed.\n");
	  exit ( EXIT_SUCCESS );
	}
      // Write directly to stdout
      // printf is too slow for this
      write ( 1, buffer, strlen( buffer ) );
      bzero ( buffer, 600 );
    }
}


void *catch_client_stream ( void * id )
{
  char buffer[255];
  int n;
  for (;;)
    {
      bzero ( buffer, 255 );
      fgets ( buffer, 255, stdin );
      printf("Input: %s\n", buffer);
      printf("Input length: %d\n", strlen(buffer));
      n = write ( server_socket, buffer, strlen(buffer)+1 );
    }
}
