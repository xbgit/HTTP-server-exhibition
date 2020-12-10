#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
char* htmlstr(char* buffer)
{
    char rst[20] = {0};
    for (int i = 0;;i++) {
        if(buffer[i] == '/')
        {
            for (int j = 0;;j++) {
                if(buffer[i+j+1] == ' ') {
                    rst[j]='\0';
                    char* str = (char*)malloc((j-i)*sizeof(char));
                    strcpy(str,rst);
                    return str;
                }
                rst[j] = buffer[i+j+1];
            }
        }
    }
}
int main()
{
    //Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);//fd:file descriptor
    if(server_fd < 0)
    {
        perror("can't create socket");
        return 0;
    }

    //Name the socket
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    const int PORT = 8080;
    address.sin_port = htons(PORT);
    address.sin_family = AF_INET;
    memset(address.sin_zero,'\0',sizeof(address.sin_zero));

    if(bind(server_fd,(struct sockaddr*)&address,addrlen) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //Wait for an incoming connection
    if(listen(server_fd,3) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    int new_socket;
    while (1)
    {
        printf("\n++++++++Waiting for new connection +++++++++\n");
        if((new_socket = accept(server_fd,(struct sockaddr*)&address,
                                (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        //Send and receive messages
        char buffer[1024] = {0};
        int valread = read(new_socket,buffer,1024);
        printf("%s\n",buffer);
        if(valread < 0)printf("No bytes are there to read");

        //analysis html request string and get the URL
        char* url = htmlstr(buffer);

        //write html response string
        FILE* htmlFile = fopen(url,"r");
        char* htmlBody = (char*)malloc(1000*sizeof(char));
        char* temp = htmlBody;
        char c;
        for (int i = 0;(c=fgetc(htmlFile))!=EOF; ++i)
            *(temp++)=c;
        char html[1000] = "HTTP/1.1 200 OK\nContent-Type: text/html\n"
                      "Content-Length: 1000\n\n";
        strcat(html, htmlBody);
        write(new_socket,html,strlen(html));

        //Close the socket
        close(new_socket);
    }


    return 0;
}
