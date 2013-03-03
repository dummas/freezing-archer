#ifndef SO_TCPLIB_H_
#define SO_TCPLIB_H_

/*
 * Estabelece uma liga��o com um servidor devolvendo o handle da liga��o.
 * 
 *  char *host: stringo com o nome DNS do ponto de escuta ou uma string com o IP em "doted notation"
 *						Exs:  "192.168.1.12" ou  "localhost" ou "colombo.deetc.isel.local", etc.
 *  int porto :    N� do porto do ponto de escuta no servidor.
 * 
 * Devolve:  handle da liga��o
 * Se erro :  devolve um valor negativo. Usar erro para obter o c�digo do erro
 */
int tcpConnectSocket(char *host, int porto ) ;

/*
 * Criar um socket de escuta num servidor
 * 
 * char *IPinterface: NULL para todas, ou ent�o a string em ip-dot-notation do IP da interface a usar
 * 
 * int porto 			  : N� inteiro com valor entre 1 e  65535. Usar um valor >= 1024
 * 
 * Devolve:  handle do ponto de escuta no servidor.
 * Se erro :  devolve um valor negativo. Usar erro para obter o c�digo do erro
 */

int tcpCreateServerSocket(char *IPinterface, int porto );


/*
 * Espera por uma liga��o no ponto de escuta srv_socket, previamente criado com a fun��o anterior.
 * 
 * Devolve:  handle da liga��o obtida
 * Se erro :  devolve um valor negativo. Usar erro para obter o c�digo do erro
 */
int tcpAccpetSocket(int srv_socket);
 

#endif /*SO_TCPLIB_H_*/
