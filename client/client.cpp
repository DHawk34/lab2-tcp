// Client side C program to demonstrate Socket
// programming
#include <iostream>
#include <fstream>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
 
int main(int argc, char const* argv[])
{
    int status, valread, client_socket;
    struct sockaddr_in serv_addr;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    std::string address;
    uint port;

    connection:

    std::cout << "Введите адрес и порт сервера" << std::endl;
    std::cin >> address >> port;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form

    // std::cout << address;

    //"127.0.0.1"
    if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)
        <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        goto connection;
    }
     
    if ((status
         = connect(client_socket, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");

        goto connection;
    }

    std::cout << "Успешно подключено" << std::endl;

    sending:

    std::string filename;
    std::cout << "Введите путь до файла" << std::endl;
    std::cin >> filename;

    // std::string filename = "file.txt";  // Имя файла для отправки
    // std::ifstream file(filename, std::ios::binary | std::ios::ate);
    // if (!file.is_open()) {
    //     std::cout << "Error opening file" << std::endl;
    //     goto sending;
    // }

    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
	
    // Отправляем содержимое файла на сервер
    
	while ((fgets(buffer, sizeof(buffer), file)) != NULL) {
        send(client_socket, buffer, strlen(buffer), 0);
    }
    
    // if(send(client_socket, buffer, file_size, 0) <= 0){
        // std::cout << "Error sending file" << std::endl;
        // return -1;
    // }

    std::cout << "File sent successfully" << std::endl;

    // Закрываем соединение и сокет
    close(client_socket);
    fclose(file);

    return 0;
}