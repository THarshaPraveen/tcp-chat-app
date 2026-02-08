#include "socketutil.h"

void startListeningAndPrintTheMessagesOnNewThread(int socketFD);

void* listenAndPrint(void *a);

void readConsoleEntriesAndSendToServer(int socketFD);

int main()
{
	int socketFD = createTCPIpv4Socket(); 
    
    struct sockaddr_in *address = createIpv4Address("127.0.0.1", 2000); 

	int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
	
    if (result == 0)
    {
		printf("Connection was succesfull\n");
    }

    startListeningAndPrintTheMessagesOnNewThread(socketFD);
    
    readConsoleEntriesAndSendToServer(socketFD);

    close(socketFD);
	return 0;
}

void readConsoleEntriesAndSendToServer(int socketFD)
{
    char *name = NULL;
    size_t nameSize = 0;
    printf("Please Enter your name?\n");
    ssize_t namecount = getline(&name, &nameSize, stdin);
    name[namecount-1] = 0;


    char *line = NULL;
    size_t linesize = 0;
    printf("type and we will send(type exit)...\n");

    char buffer[1024];

    while(true)
    {
        ssize_t charcount = getline(&line, &linesize, stdin);
        line[charcount-1] = 0;

        sprintf(buffer, "%s:%s", name, line);

        if(charcount > 0)
        {
            if(strcmp(line, "exit") == 0)
            {
                break;
            }

            ssize_t amountwassent = send(socketFD, buffer, strlen(buffer), 0);

            if (amountwassent < 0)
            {
                perror("send failed");
            }
        }
    }
    
    free(name);
    free(line);
}

void startListeningAndPrintTheMessagesOnNewThread(int socketFD)
{
    int *sock = malloc(sizeof(int));
    *sock = socketFD;
    pthread_t id;
    pthread_create(&id, NULL, listenAndPrint, sock);
}

void* listenAndPrint(void *a)
{
    int socketFD = *(int *)a;
    free(a);

    char buffer[1024];

    while(true)
    {
        ssize_t amountreceived = recv(socketFD, buffer, 1024, 0);
        
        if(amountreceived > 0)
        {
            buffer[amountreceived] = 0;
            printf("%s\n", buffer);
        }

        if(amountreceived == 0)
        {
            break;
        }
    }

    return NULL;
}
