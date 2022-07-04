#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
//#include <iostream>

void check_host_name(int hostname) { //This function returns host name for local computer
	fprintf (stderr, "'hostname: %d\n", hostname);
   if (hostname == -1) {
      perror("gethostname");
      exit(1);
   }
}
void check_host_entry(struct hostent * hostentry) { //find host info from host name
   if (hostentry == NULL){
		fprintf (stderr, "hostentry is NULL\n");
      perror("gethostbyname");
      exit(1);
   }
	else {
		fprintf (stderr, "Host Entry: %s\n", hostentry->h_name);
	}
}
void IP_formatter(char *IPbuffer) { //convert IP string to dotted decimal format
   if (NULL == IPbuffer) {
      perror("inet_ntoa");
      exit(1);
   }
}
main()
{
   char host[256];
   char *IP;
   struct hostent *host_entry;
   int hostname;
	//try {
		printf ("calling gethostname...");
   		hostname = gethostname(host, sizeof(host)); //find the host name
		printf ("called gethostname, hostname=%d, host=%s\n", hostname, host);
   		check_host_name(hostname);
		//printf ("hostname checked\n");
   		host_entry = gethostbyname(host); //find host information
		fprintf (stderr, "host=%s\n", host);
   		if (host_entry == NULL)
			fprintf (stderr, "host_entry is NULL\n");
   		check_host_entry(host_entry);
   		IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
   		printf("Current Host Name: %s\n", host);
   		printf("Host IP: %s\n", IP);
	//}
	//catch (exception &e) {
		//fprintf (stderr, "Error:\n%s\n", e.what());
	//}
}

