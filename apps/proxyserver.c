/* proxyserver.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cnaiapi.h>

#if defined(LINUX) || defined(SOLARIS)
#include <sys/time.h>
#endif

#define BUFFSIZE  256

int  recvln(connection, char *, int);
void getSite(char *site, char *path, connection parent);

/*-----------------------------------------------------------------------
 *
 * Program: proxyserver
 * Purpose: serve as proxy to web clients
 * Usage:   proxyserver <appnum>
 *
 *-----------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{

  connection  conn;
  int    n;
  char    buff[BUFFSIZE], cmd[16], path[64], vers[16];
  char    *timestr, *garbage, *site;
  int    len;

#if defined(LINUX) || defined(SOLARIS)
  struct timeval  tv;
#elif defined(WIN32)
  time_t          tv;
#endif

  if (argc != 2) {
    (void) fprintf(stderr, "usage: %s <appnum>\n", argv[0]);
    exit(1);
  }



  while(1) {
    FILE *f = fopen("log.txt", "a+");
    if (f == NULL)
    {
      printf("Error opening log file!\n");
      exit(1);
    }

    /* wait for contact from a client on specified appnum */
    conn = await_contact((appnum) atoi(argv[1]));
    if (conn < 0)
      exit(1);

    /* read and parse the request line */
    n = recvln(conn, buff, BUFFSIZE);
    sscanf(buff, "%s %*[^:]://%[^/]%s %s", cmd, site, path, vers);

    /* check for unexpected end of file */
    if (n < 1) {
      (void) send_eof(conn);
      continue;
    }

    #if defined(LINUX) || defined(SOLARIS)
          gettimeofday(&tv, NULL);
          timestr = ctime(&tv.tv_sec);
    #elif defined(WIN32)
          time(&tv);
          timestr = ctime(&tv);
    #endif

    fprintf(f, "%s%s%s\n\n", timestr, site, path);
    fclose(f);
    getSite(site, path, conn);

    (void) send_eof(conn);
  }
}

void getSite(char *site, char *path, connection parent) {
  appnum app = 80;
  computer comp;
  connection conn;
  int len;
  char buff[BUFFSIZE];

  /* convert arguments to binary computer and appnum */
  comp = cname_to_comp(site);

  if (comp == -1)
    exit(1);

  /* contact the web server */
  conn = make_contact(comp, app);

  if (conn < 0)
    exit(1);

  /* send an HTTP/1.1 request to the webserver */

  len = sprintf(buff, "GET %s HTTP/1.1\r\nhost: %s\r\n\r\n", path, site);
  (void) send(conn, buff, len, 0);

  /* dump all data received from the server to parent connection */
  while((len = recv(conn, buff, BUFFSIZE, 0)) > 0) {
    //(void) write(STDOUT_FILENO, buff, len);
    (void) send(parent, buff, len, 0);
  }
  (void) send_eof(conn);
}
