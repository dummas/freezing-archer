#include <stdio.h>
#include <stdlib.h>

/**
	* Error printing
	* @message - received message
	* @return - nothing
	*/

void error ( char *message )
{
  printf( message );
  exit(1);
}

/**
	* Socket return checking
	* @n - socket return
	* @return - nothing
	*/

void check_socket_return ( int n )
{

	if ( n < 0 )
	{
		error ( "Error on socket return" );
	}
	
}

/**
	* Client socket checking
	* @client_socket - client socket
	* @return - nothing
	*/

void check_client_socket ( int client_socket )
{

	if ( client_socket < 0 )
	{
		error ( "Error on client socket" );
	}

}

/**
	* Server socket check
	* @server_socket - server socket
	* @return - nothing
	*/

void check_server_socket ( int server_socket )
{

	if ( server_socket < 0 )
	{
		error ( "Error on server socket" );
	}

}

/**
	* Check if port is provided by user as an argument
	* @argc - number of arguments in topic
	* @return - nothing
	*/

void check_if_port_is_provided ( int argc )
{
	if ( argc < 2 )
	{
		fprintf( stderr, "Error on port providing.\n" );
		exit(1);
	}
}

/**
	* Check if file ( database ) opened succesifylly
	* @file_read - file read stream
	* @return - nothing
	*/

void is_file_read_null ( FILE *file_read )
{
	if ( file_read == NULL )
	{
		error ( "Error on opening topics data file" );
	}
}
