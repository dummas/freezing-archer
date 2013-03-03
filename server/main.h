#define DEBUG 1
#define MAX_TOPICS_NUMBER 80
#define MAX_TOPIC_SIZE 80
#define MAX_CONNECTIONS 20

typedef unsigned short int small_int;

typedef struct _client_data {
  int client_socket;
  char *nick;
  char *topic;
} client_data;

// Global variables
char *topic[MAX_TOPICS_NUMBER];
size_t amount_of_topics = 0;
// Number of connetions
size_t connection_counter;
// Global counter of connections ( need for admin purposes )
size_t global_connection_counter;
// Connected clients data
client_data data[MAX_CONNECTIONS];
// Server socket
int server_socket;
// Thread locker
pthread_mutex_t mu_lock;

void message ( char *);
char *set_new_memory_space_for_topic ( char *);
void print_all_topics();
void set_topics();
void display_all_topics_to_client ( int );
void send_message_to_other_threads ( int , char* );
void *create_client_server ( void * );
void chat_service ( int );
void command_interface ( int, char* );
void print_how_many_customers_are_connected(int);
void reload_themes_file ( int );
void stop_the_server();
void print_themes_in_use ( int );
void print_names_by_theme ( int );
size_t topic_in_use ( char * );
void print_topic_name_to_client ( int, int );
void print_clients_in_topic ( int, int );
void send_message_to_all_users ( char * );
void _sleep ( unsigned long );
void disconnect_client ( int );
void disconnect_all_clients ();
