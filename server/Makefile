
#all: lib tst-server tst-client tst-server-conc

server:
	gcc -g -o cus-server main.c so_tcplib.c libcheck.h -pthread

#lib:
#	gcc -g -c so_tcplib.c
#
#tst-server:
#	gcc -g -o so_tcpServer  so_tcplib.c so_tcpServer.c
#
#tst-server-conc:
#	gcc -g -o so_tcpServer-conc  so_tcplib.c so_tcpServer-conc.c
#
#tst-client:
#	gcc -g -o so_tcpClient  so_tcplib.c  so_tcpClient.c
