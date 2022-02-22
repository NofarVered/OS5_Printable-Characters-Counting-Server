#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <signal.h>

#define BUFFSIZE 1024
#define ARRSIZE 127
#define ARRSTART 32

// Nofar Vered Nahum 315391367

int keep_running = 0;
int numOFclients = 0;
uint32_t pcc_total[ARRSIZE] = {0};
uint32_t pcc_temp[ARRSIZE] = {0};

void handler(int signal_num)
{
  int i;
  if (numOFclients == 0)
  {
    for (i = ARRSTART; i < ARRSIZE; i++)
    {
      printf("char '%c' : %u times\n", i, pcc_total[i]);
    }
    exit(0);
  }
  else
  {
    keep_running = 1;
  }
}

int main(int argc, char *argv[])
{

  uint16_t port;
  int listenfd;
  int connectfd;
  uint32_t N;
  uint32_t N_net;
  char message[BUFFSIZE];
  int i;
  int cnt;
  uint32_t C;
  if (argc != 2)
  {
    perror("ERROR number of arguments pcc_client.\n");
    exit(1);
  }
  struct sigaction sig_handler;
  sig_handler.sa_handler = &handler;
  sigemptyset(&sig_handler.sa_mask);
  sig_handler.sa_flags = SA_RESTART;
  if (sigaction(SIGINT, &sig_handler, 0) != 0)
  {
    perror("ERROR SIGINT .\n");
    exit(1);
  }

  port = atoi(argv[1]);
  struct sockaddr_in serv_addr;
  struct sockaddr_in peer_addr;
  socklen_t addrsize = sizeof(struct sockaddr_in);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
  {
    perror("ERROR socket .\n");
    exit(1);
  }
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
  {
    perror("ERROR setsockopt .\n");
    exit(1);
  }

  memset(&serv_addr, 0, addrsize);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (bind(listenfd, (struct sockaddr *)&serv_addr, addrsize) != 0)
  {
    perror("ERROR bind  .\n");
    exit(1);
  }
  if (listen(listenfd, 10) != 0)
  {
    perror("ERROR listen  .\n ");
    exit(1);
  }

  while (keep_running != 1)
  {
    connectfd = accept(listenfd, (struct sockaddr *)&peer_addr, &addrsize);
    if (connect < 0)
    {
      perror("ERROR failed .\n");
      exit(1);
    }

    numOFclients = 1;
    if (recv(connectfd, &N_net, 4, 0) < 0)
    {
      if ((errno == EPIPE) || (errno == ETIMEDOUT) || (errno == ECONNRESET))
      {
        numOFclients = 0;
        perror("ERROR read N  .\n");
        close(connectfd);
        continue;
      }
      if ((errno != SIGINT) && (errno != EINTR))
      {
        numOFclients = 0;
        perror("ERROR read N .\n");
        exit(1);
      }
    }

    N = ntohl(N_net);

    if (recv(connectfd, message, sizeof(char) * N, 0) < 0)
    {
      if ((errno == EPIPE) || (errno == ETIMEDOUT) || (errno == ECONNRESET))
      {
        numOFclients = 0;
        perror("ERROR message .\n");
        close(connectfd);
        continue;
      }
      if ((errno != SIGINT) && (errno != EINTR))
      {
        numOFclients = 0;
        perror("ERROR message .\n");
        exit(1);
      }
    }

    C = 0;
    for (i = 0; i < strlen(message); i++)
    {
      cnt = (int)message[i];
      if (cnt > 31 && cnt < ARRSIZE)
      {
        pcc_temp[cnt] += 1;
        C += 1;
      }
    }

    if (send(connectfd, &C, sizeof(C), 0) < 0)
    {
      if ((errno == EPIPE) || (errno == ETIMEDOUT) || (errno == ECONNRESET))
      {
        for (i = ARRSTART; i < ARRSIZE; i++)
        {
          pcc_temp[i] = pcc_total[i];
        }
        numOFclients = 0;
        perror("ERROR message .\n");
        close(connectfd);
        continue;
      }
      if ((errno != SIGINT) && (errno != EINTR))
      {
        numOFclients = 0;
        perror("ERROR message .\n");
        exit(1);
      }
    }
    for (i = ARRSTART; i < ARRSIZE; i++)
    {
      pcc_total[i] = pcc_temp[i];
    }
    close(connectfd);
    numOFclients = 0;
  }
  for (i = ARRSTART; i < ARRSIZE; i++)
  {
    printf("char '%c' : %u times\n", i, pcc_total[i]);
  }
  exit(0);
}
