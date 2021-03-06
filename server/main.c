#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>

#include "so_tcplib.h"
#include "libcheck.h"
#include "main.h"

int main ( int argc, char *argv[] )
{
  // Set all topics
  set_topics();
  // Reading file descriptor
  int n;
  // Buffer
  char buffer[255];
  // Port number
  int port_number;
  // Check if variable is passed

  check_if_port_is_provided ( argc );

  // Providing port number
  port_number = atoi ( argv[1] );
  // Server socket
  server_socket = tcpCreateServerSocket ( NULL, port_number );
  // Client socket
  int client_socket;
  // Reading line from command output
  char line[255];
  // For topic choose
  small_int topic_selected = 0;
  // Child threads
  pthread_t thread;
  
  check_server_socket ( server_socket );

  connection_counter = 0;

  global_connection_counter = 0;

  for ( ;; )
    {
      // Listen connection
      printf("Waiting for connection..\n");
      client_socket = tcpAccpetSocket ( server_socket );
      // Connection received
      printf("Client connection received.\n");
      check_client_socket ( client_socket );
      printf("Starting chat server for client.\n");
      data[connection_counter].client_socket = client_socket;
      data[connection_counter].nick = " ";
      data[connection_counter].topic = 0;

      pthread_mutex_init(&mu_lock, NULL);
      
      pthread_create ( &thread, NULL, create_client_server, (void *) connection_counter );
      connection_counter += 1;
      global_connection_counter += 1;
      printf("Number of connections: %d\n", connection_counter);
    }
  
  close ( server_socket );
  pthread_mutex_destroy(&mu_lock);
  
  return 0;
}

/** 
 * Messages
 */
void message ( char *message )
{
  if ( DEBUG == 1 )
    {
      printf("DEBUG: %s\n", message);
    }
}

/**
 * New memory space and set 
 */
char *set_new_memory_space_for_topic ( char *line )
{
  char *buf = ( char* ) calloc ( MAX_TOPIC_SIZE, sizeof( char ) );
  strncat ( buf, line, strlen(line) - 1 );
  return buf;
}
/**
 * Just debug information
 */
void print_all_topics()
{
  size_t i = 0;
  for ( i = 0; i <= amount_of_topics; i++ )
    {
      printf("Topic %d:\n", i);
      printf("\tName: %s\n", topic[i]);
      printf("\tLength: %d\n", strlen(topic[i]));
      printf("\tAddress: %d\n", topic[i]);
    }
}

/**
 * Function reads all topics in topics databas
 * @amount_of_topics - tatal amount of discovered topics
 * @topic - an array of topics
 */
void set_topics()
{
  // Line
  char line[80];
  // File reading stream
  FILE *file_read;
  // File data
  char *file_name = "topics.dat";
  // for loop
  int i = 0;
  
  amount_of_topics = 0;
  
  // Setting file stream
  file_read = fopen ( file_name, "r");
  
  // Checking if file is ok
  is_file_read_null ( file_read );
  
  // Reading and showing the file
  while ( fgets( line, sizeof line, file_read ) != NULL  )
    {
      topic[ amount_of_topics ] = set_new_memory_space_for_topic ( line );
      amount_of_topics += 1;
    }
  
  amount_of_topics -= 1;
  
  if ( DEBUG == 1 )
    {
      message("Start topics print");
      print_all_topics();
      message("End topics print");
    }
  
  fclose( file_read );
}

/**
 * Displays all topics to client
 */
void display_all_topics_to_client ( int client_socket )
{
  int n = 0;
  char message[255];

  // Send 
  strcpy ( message, "Aviable topics:\n" );
  n = write ( client_socket, message, strlen(message) );
  
  check_socket_return ( n );
  
  size_t i = 0;
  
  for ( i = 0; i <= amount_of_topics; i++ )
    {
      strcpy ( message, "");
      strcat ( message, topic[i] );
      strcat ( message, "\n" );
      n = write ( client_socket, message, strlen ( message ) );
      check_socket_return ( n );
    }
}

/**
 * Send message to other threads
 */
void send_message_to_other_threads ( int connection_id, char *message )
{
  int n = 0;
  int i = 0;
  char buffer[255];
  for ( i = 0; i <= connection_counter; i++ )
    {
      if ( data[i].client_socket != data[connection_id].client_socket && data[i].client_socket != 0 )
	{
	  if ( DEBUG == 1 )
	    {
	      printf("Different socket discovered\n");
	      printf("Scaning socket: %d sending socket: %d \n", 
		     data[i].client_socket, data[connection_id].client_socket);
	      printf("Scaning topic: %s sending topic: %s \n",
		     data[i].topic, data[connection_id].topic);
	    }
	  
	  if ( strcmp ( data[i].topic, data[connection_id].topic ) == 0 )
	    {

	      if ( DEBUG == 1 )
		{
		  printf("Same topic discovered\n");
		}

	      bzero ( buffer, 255 );
	      strcpy ( buffer, data[connection_id].nick );
	      strcat ( buffer, ": " );
	      strcat ( buffer, message );
	      strcat ( buffer, "\n" );
	      n = write ( data[i].client_socket, buffer, strlen(buffer) );
	      check_socket_return ( n );
	    }
	}
    }
}

/**
 * Initialize all important data about user
 */
void *create_client_server ( void * _connection_id )
{
  // Buffer
  char buffer[255];
  // Return socket
  int n;
  // Connection id
  int connection_id = (int ) _connection_id;
  // Assign a space for a topic
  char *topic = (char *) calloc ( 80, sizeof(char) );
  // Sending messages
  char *message = "Welcome !\n\n";

  // Locking 
  pthread_mutex_lock(&mu_lock);

  // Require nick
  if ( DEBUG == 1 )
    {
      printf("Connection id: %d\n", connection_id);
      printf("connection socket: %d\n", data[connection_id].client_socket);
    }

  // Send welcome message
  if ( DEBUG == 1 )
    {
      printf("Sending welcome message\n");
    }
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );

  // Display all topics to client
  if ( DEBUG == 1 )
    {
      printf("Display topics to client\n");
    }
  display_all_topics_to_client ( data[connection_id].client_socket );

  // Send Topic enter input
  message = "Topic: ";
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );

  // Read client topic
  bzero ( buffer, 255 );
  n = read ( data[connection_id].client_socket, topic, 255 );
  check_socket_return ( n );
  topic[n-1] = '\0';
  data[connection_id].topic = topic;
  printf("Client topic: %s\n", data[connection_id].topic);
  
  // Send nick enter input
  message = "Nick: ";
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );
  
  // Read client nick
  bzero ( buffer, 255 );
  n = read ( data[connection_id].client_socket, buffer, 255 );
  check_socket_return ( n );
  // Just to sleep well
  buffer[n-1] = '\0';
  // Modify
  data[connection_id].nick = buffer;

  printf("Client nick name: %s\n", data[connection_id].nick);

  // Send message about new connection to all users
  message = "New client connected\n";
  send_message_to_other_threads ( connection_id, message );
  
  // Sending final welcoming message
  message = "\n\nWelcome to char host!\n\n";
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );

  pthread_mutex_unlock(&mu_lock);
  
  // Start chat service
  chat_service ( connection_id );

  pthread_exit ( NULL );
}

/**
 * Operates as chat service provider
 */
void chat_service ( int connection_id )
{
  // Socket return
  int n;
  // Buffer
  char buffer[255];
  
  for (;;)
    {
      // Zeroing buffer ( just to sleep well )
      bzero ( buffer, 255 );
      // Waiting for message
      n = read ( data[connection_id].client_socket, buffer, 255 );
      // Checking return
      check_socket_return ( n );
      // Just to sleep well
      buffer[n-1] = '\0';

      // If received data length is 2 ( char + end ), so this is command
      if ( n == 2 )
	{
	  command_interface ( connection_id, buffer );
	}
      // Else, this is normal message, so send it to all folks
      else
	{
	  send_message_to_other_threads ( connection_id, buffer );
	}
    }
}

/**
 * How many customers are connected
 * Name of the clients connected by theme
 * a List of themes in use
 * Stop the server
 * Reload
 */

void command_interface ( int connection_id, char *buffer )
{

  char command = buffer[0];
  switch ( command )
    {
      // How many customers are connected
    case 'C':
      print_how_many_customers_are_connected( connection_id );
      break;
      // Name of the clients connected by theme
    case 'N':
      print_names_by_theme ( connection_id );
      break;
      // A list of themes in use
    case 'L':
      print_themes_in_use ( connection_id );
      break;
      // Stop the server, making notice to all clients that will close within a specified time
    case 'S':
      stop_the_server();
      break;
      // Reload the file with the themes
    case 'R':
      reload_themes_file( connection_id );
      break;
      // Disconect user
    case 'Q':
      disconnect_client ( connection_id );
      break;
    }
  
}

/**
 * Prints number of connected clients
 */

void print_how_many_customers_are_connected( int connection_id )
{
  int n;
  char message[255];
  char buffer[10];
  sprintf( buffer, "%d", global_connection_counter );
  strcpy ( message, "Number of connected customers: " );
  strcat ( message, buffer );
  strcat ( message, "\n" );
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );
}

/**
 * Function reloads the themes
 */
void reload_themes_file ( int connection_id )
{
  int n;
  char *message = "Reloading themes file\n";
  n = write ( data[connection_id].client_socket, message, strlen(message ));
  check_socket_return ( n );

  set_topics();

  message = "Done\n";
  n = write ( data[connection_id].client_socket, message, strlen(message));
  check_socket_return ( n );
}

/**
 * Function stops the server
 */
void stop_the_server()
{
  int i;
  char message[255];
  
  bzero ( message, 255 );

  strcpy ( message, "Server is going to shut down\n" );

  send_message_to_all_users ( message );

  for ( i = 5; i > 0; i-- )
    {
      sprintf(message, "In %d second(-s)\n", i);
      send_message_to_all_users ( message );
      _sleep ( 1000000 );
    }

  disconnect_all_clients();
  
  exit ( EXIT_SUCCESS );
}
/**
 * Displays witch topics are in use
 */
void print_themes_in_use ( int connection_id )
{
  int n;
  int i;
  char message[255];
  strcpy(message, "Topics in use:\n");
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );
  for ( i = 0; i <= amount_of_topics; i++ )
    {
      if ( topic_in_use ( topic[i] ) == 1 )
	{
	  bzero ( message, 255 );
	  strcpy ( message, topic[i] );
	  strcat ( message, "\n" );
	  n = write ( data[connection_id].client_socket, message, strlen(message) );
	  check_socket_return ( n );
	}
    }
}

/**
 * Operates for clients by topic printing
 */
void print_names_by_theme ( int connection_id )
{
  // Socket return
  int n;
  // Loop variables
  int i;

  // Scroll topics
  for ( i = 0; i <= amount_of_topics; i++ )
    {
      if ( topic_in_use ( topic[i] ) == 1 )
	{
	  print_topic_name_to_client ( i, connection_id );
	  print_clients_in_topic ( i, connection_id );
	}

    }

}

/**
 * Prints all clients by topic
 */
void print_clients_in_topic ( topic_id, connection_id )
{
  int n;
  int i;
  char message[255];

  for ( i = 0; i < connection_counter; i++ )
    {
      if ( strcmp ( topic[topic_id], data[i].topic ) == 0 )
	{
	  bzero ( message, 255 );
	  strcpy ( message, data[i].nick );
	  strcat ( message, "\n" );
	  n = write ( data[connection_id].client_socket, message, strlen(message) );
	  check_socket_return ( n );
	}
    }
}

/**
 * Prints topic name to client
 */

void print_topic_name_to_client ( topic_id, connection_id )
{
  int n;
  char message[255];

  bzero ( message, 255 );
  strcpy ( message, "Users in topic: " );
  strcat ( message, topic[topic_id] );
  strcat ( message, "\n" );
  n = write ( data[connection_id].client_socket, message, strlen(message) );
  check_socket_return ( n );
}

/**
 * Function checks which topics are in use
 * @returns 1 - if is in use; 0 - if not in use
 */
size_t topic_in_use ( char *topic )
{
  int i = 0;
  for ( i = 0; i < connection_counter; i++ )
    {
      printf("Topic: %s\n", topic);
      printf("User topic: %s\n", data[i].topic);
      if ( strcmp ( topic, data[i].topic ) == 0 )
	{
	  return 1;
	}
    }
  return 0;
}

/**
 * Send message to all users, no matter the theme
 */
void send_message_to_all_users ( char *message )
{
  int n;
  int i;

  for ( i = 0; i <= connection_counter; i++ )
    {
      n = write ( data[i].client_socket, message, strlen(message));
      check_socket_return( n );
    }
}

/**
 * Sleep function for time count
 */
void _sleep ( unsigned long u )
{
  clock_t start = clock();
  if ( start != (clock_t)(-1) )
    {
      clock_t end = start + CLOCKS_PER_SEC * ( u / 1000000.0 );
      while ( clock() != end );
    }
}


/**
 * Disconnect specific client
 */
void disconnect_client ( int connection_id )
{
  char *message;
  message = "Disconnecting..";
  send_message_to_other_threads ( connection_id, message );
  close ( data[connection_id].client_socket );
}

/**
 * Disconnecting all clients
 */
void disconnect_all_clients ()
{
  int i;
  for ( i = 0; i <= connection_counter; i++ )
    {
      close ( data[i].client_socket );
    }
}
