/* Referência: chatgpt (uso do select com sockets) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10

int
main()
{
    int serverSocket, clientSocket, maxSocket, activity, i, valread;
    int clientSockets[MAX_CLIENTS] = {0};
    fd_set readfds;
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    
    // Criação do socket do servidor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
      {
        perror("socket failed");
        exit(EXIT_FAILURE);
      }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);
    
    // Associação do socket à porta 8888
    if (bind(serverSocket,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
      {
        perror("bind failed");
        exit(EXIT_FAILURE);
      }
    
    printf("Listening on port 8888...\n");
    
    // Habilita o socket para aceitar conexões
    if (listen(serverSocket, 3) < 0)
      {
        perror("listen failed");
        exit(EXIT_FAILURE);
      }
    
    int addrlen = sizeof(serverAddr);
    
    while (1)
      {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        maxSocket = serverSocket;
        
        for (i = 0; i < MAX_CLIENTS; i++)
          {
            clientSocket = clientSockets[i];
            
            if (clientSocket > 0)
              FD_SET(clientSocket, &readfds);
            
            if (clientSocket > maxSocket)
              maxSocket = clientSocket;
          }
        
        activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        
        if ((activity < 0) && (errno != EINTR))
          perror("select error");
        
        // Verifica se há uma nova conexão de cliente
        if (FD_ISSET(serverSocket, &readfds))
          {
            if ((clientSocket = accept(serverSocket,
                                       (struct sockaddr *)&serverAddr,
                                       (socklen_t*)&addrlen)) < 0)
              {
                perror("accept error");
                exit(EXIT_FAILURE);
              }
            
            printf("New connection, socket fd: %d, IP: %s, port: %d\n",
                   clientSocket,
                   inet_ntoa(serverAddr.sin_addr),
                   ntohs(serverAddr.sin_port));
            
            // Adiciona o novo socket à lista de clientes
            for (i = 0; i < MAX_CLIENTS; i++)
              if (clientSockets[i] == 0)
                {
                  clientSockets[i] = clientSocket;
                  break;
                }
          }
        
        // Verifica se há dados recebidos de algum cliente
        for (i = 0; i < MAX_CLIENTS; i++)
          {
            clientSocket = clientSockets[i];
            
            if (FD_ISSET(clientSocket, &readfds))
              {
                if ((valread = read(clientSocket, buffer, 1024)) == 0)
                  {
                    // Cliente desconectado
                    getpeername(clientSocket, (struct sockaddr*)&serverAddr, (socklen_t*)&addrlen);
                    printf("Client disconnected, IP: %s, port: %d\n",
                           inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
                    close(clientSocket);
                    clientSockets[i] = 0;
                  }
                else
                  {
                    // Processa os dados recebidos do cliente
                    // ...
                    // Você pode implementar o tratamento dos dados recebidos aqui
                    // ...
                  }
              }
          }
      }
    
    return 0;
}

