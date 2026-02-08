#include "socketutil.h"

struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD);

void* receiveAndPrintIncomingData(void *b);

void startAcceptingIncomingConnections(int serverSocketFD);

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *psocket);

void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD);

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketsCount = 0;

int main()
{   
    
    int serverSocketFD = createTCPIpv4Socket();
    
    struct sockaddr_in *serveraddress = createIpv4Address("", 2000);
    
    int result = bind(serverSocketFD, (struct sockaddr *)serveraddress, sizeof(*serveraddress));

    if(result == 0)
    {
        printf("socket was bound successfully!\n");
    }

    int listenresult = listen(serverSocketFD, 10);

    startAcceptingIncomingConnections(serverSocketFD);
    
    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD)
{
    struct sockaddr_in clientaddress;
    int clientaddresssize = sizeof(struct sockaddr_in);
    int clientsocketFD = accept(serverSocketFD, (struct sockaddr *)&clientaddress, &clientaddresssize);
    

    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientaddress;
    acceptedSocket->acceptedSocketFD = clientsocketFD;
    acceptedSocket->acceptedSuccessfully = clientsocketFD > 0;

    if(!acceptedSocket->acceptedSuccessfully)
    {
        acceptedSocket->error = clientsocketFD;
    }

    return acceptedSocket;
};


void startAcceptingIncomingConnections(int serverSocketFD)
{
    while(true)
    {
        struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
        
        if (acceptedSocketsCount >= 10)
        {
            close(clientSocket->acceptedSocketFD);
            free(clientSocket);
            continue;
        }

        acceptedSockets[acceptedSocketsCount++] = *clientSocket;

        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *psocket)
{
    pthread_t id;
    pthread_create(&id, NULL, receiveAndPrintIncomingData, psocket);
}

void* receiveAndPrintIncomingData(void *b)
{
    struct AcceptedSocket *psocket = b;
    int socketFD = psocket->acceptedSocketFD;

    char buffer[1024];
    while(true)
    {
        ssize_t amountreceived = recv(socketFD, buffer, 1024, 0);

        if(amountreceived > 0)
        {
            buffer[amountreceived] = 0;
            printf("%s\n", buffer);

            sendReceivedMessageToTheOtherClients(buffer, socketFD);
        }

        if(amountreceived == 0)
        {
            break;
        }
    }

    free(psocket);
    close(socketFD);
    return NULL;
}

void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD)
{

    for(int i = 0; i < acceptedSocketsCount; i++)
    {
        if(acceptedSockets[i].acceptedSocketFD != socketFD)
        {
            send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0);
        }
    }
}
