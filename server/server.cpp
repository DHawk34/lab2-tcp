#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

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

std::queue<int> connectionQueue;
std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> stopThread(false);

int max_threads; 
int port; 
int max_file_size; 
std::string save_path; 

//Обработка клиентского подключения
void handleConnection(int clientSocket)
{
    std::string filename = "received_file.txt";

    FILE *file = fopen(filename.c_str(), "a");
    if (file == NULL) {
        perror("Error opening file for writing");
        close(clientSocket);
        return;
    }

    int recieved_byte_size = 0;
    while (recieved_byte_size != max_file_size) {

        recieved_byte_size += BUFFER_SIZE;
        char buffer[BUFFER_SIZE] = {0};

        int readLen = BUFFER_SIZE;
        if (recieved_byte_size > max_file_size)
        {
            for (size_t i = 0; i < sizeof(buffer); i++)
            {
                buffer[i] = '\0';
            }
            
            readLen = BUFFER_SIZE - (recieved_byte_size - max_file_size);
            recieved_byte_size = max_file_size;
        }

        ssize_t bytesRead = recv(clientSocket, buffer, readLen, 0);
        if (bytesRead <= 0) break;

        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);

    close(clientSocket);
}

void worker()
{
    while (!stopThread)
    {
        int clientSocket;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []
                    { return !connectionQueue.empty(); });
            clientSocket = connectionQueue.front();
            connectionQueue.pop();
			// lock.unlock();
        }
		
        handleConnection(clientSocket);
    }
}

int main(int argc, char* argv[])
{
 
    if (argc != 5) { 
        fprintf(stderr, "Usage: %s <port> <max_threads> <max_file_size> <save_path>\n", argv[0]); 
        exit(EXIT_FAILURE); 
    } 
 
    port = atoi(argv[1]); 
    max_threads = atoi(argv[2]);
    max_file_size = atoi(argv[3]); 
 
    save_path = argv[4];

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Ошибка при создании дочернего процесса" << std::endl;
        exit(1);
    }

    if (pid > 0) {
        exit(0);
    }

    umask(0);

    setsid();

    if (chdir(save_path.c_str()) < 0) {
        std::cerr << "Не удалось изменить текущий каталог" << std::endl;
        exit(1);
    }

    for (int i{}; i < argc; ++i)
    {
        std::cout << argv[i] << std::endl;
    }

    std::cout << "demon start" << std::endl;

    int server_socket, inSocket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_socket, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port
    if (bind(server_socket, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, max_threads) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << port << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < max_threads; ++i)
    {
        threads.emplace_back(worker);
    }

    while (true)
    {
        if ((inSocket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            connectionQueue.push(inSocket);
//			lock.unlock();
        }
        cv.notify_one();
    }

    stopThread = true;
    cv.notify_all();

    for (auto &thread : threads)
    {
        thread.join();
    }

    close(server_socket);
    return 0;
}