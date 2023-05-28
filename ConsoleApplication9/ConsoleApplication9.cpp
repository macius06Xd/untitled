#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <string>
#include <mutex>
#include <ws2tcpip.h>
#include <vector>
#include <sstream>
#pragma warning(disable : 4996)

const int BUFFER_SIZE = 1024;
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8020;
struct point
{
    int x, y;
    bool operator==(const point& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const point& other) const
    {
        return !(*this == other);
    }
    
};

std::mutex position_mutex;
class robot
{
public:
    std::mutex blok;
    SOCKET socketother;
    SOCKET socketServer;
    point position;
    bool captain = false;;
    std::vector<point> path;
    bool hold_block = false;
    int current_path_point;
    robot(SOCKET socketServer,SOCKET socket)
    {
        this->socketServer = socketServer;
        this->socketother = socket;
        get_position();
        if (captain)
        {
            captain_behaviour();
        }
        else
        {
            other_behaviour();
        }
    }
    void send_point(point p)
    {
        int a, b;
        a = p.x;
        b = p.y;
        std::string message = "n{" + std::to_string(a) + " , " + std::to_string(b) + "}";
        send(socketServer, message.c_str(), message.length(), 0);
    }
    void ask_for_path()
    {
        send(socketServer, "paths\0", 7, 0);
        char buffer[10000];
        int a = recv(socketServer, buffer, 10000, 0);
        parse_path(buffer);
    }
    void parse_path(char pathbuffer[10000])
    {
        current_path_point = 0;
        // Clear the path vector before parsing
        path.clear();

        // Skip the "path" prefix
        std::string pathStr(pathbuffer + 4);

        // Create a string stream to extract coordinates
        std::istringstream iss(pathStr);

        std::string coordinate;
        while (std::getline(iss, coordinate, ';'))
        {
            std::istringstream issCoord(coordinate);
            std::string xStr, yStr;

            // Extract x and y coordinates
            if (std::getline(issCoord, xStr, ',') && std::getline(issCoord, yStr, ','))
            {
                point p;
                // Convert x and y coordinates to integers
                p.x = std::stoi(xStr);
                p.y = std::stoi(yStr);
                path.push_back(p);
            }
        }
        for (auto path_part : path)
        {
            printf("%d : %d\n", path_part.x, path_part.y);
        }
   
    }
    bool can_move()
    {
        std::string message;
        message = "ask_move\0";
        send(socketServer, message.c_str(), message.length()+1, 0);
        char return_message[20];
       int a = recv(socketServer, return_message, 20, 0);
       return_message[a] = '\0';
        printf("odpowiedz %s \n", return_message);
        if (return_message[0]=='y')
        {
            printf("Confirm");
            return true;
        }
        else return false;
    }
    bool send_move(point p)
    {
        std::string message;
        message += "move";
        message += " {" + std::to_string(p.x) + "," + std::to_string(p.y) + "}";
        send(socketServer, message.c_str(), message.length(),0);
        char answer[3];
        recv(socketServer, answer, 3, 0);
        printf("%s",answer);
        return answer[0] == 'y';
    }
    void send_holding()
    {
        send(socketServer, "hold\0",6 , 0);
    }
    void send_push()
    {
        send(socketServer, "push\0", 6, 0);
    }
    bool is_parner_holding()
    {
        std::string message;
        message += "partner_hold\0";
        send(socketServer, message.c_str(), message.length(), 0);
        char answer[3];
        recv(socketServer, answer, 3, 0);
        printf("czekam na chlopa %d\n", answer[0] == 'y');
        return answer[0] == 'y';
    }
    void captain_behaviour()
    {
        while (1)
        {
            ask_for_path();
            if (path.size() == 0)
                break;
            while (position != path[path.size() - 1])
            {

                printf("Jeszcze jak");
                printf("%d , %d\n", position.x, position.y);
                if (send_move(path[current_path_point]))
                {
                    printf("ruszam sie");
                    position = path[current_path_point];
                    current_path_point += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                else
                {
                    ask_for_path();
                    current_path_point = 0;
                }

            }
            hold_block = true;
            send_holding();
            while (!is_parner_holding())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            printf("ja i partner zlapalismy");
            ask_for_path();
            printf("path size : %d", path.size());
            current_path_point = 0;
            while (current_path_point != path.size() - 1)
            {

                if (can_move())
                {
                    if (send_move(path[current_path_point]))
                    {
                        printf("ruszam sie");
                        position = path[current_path_point];
                        current_path_point += 1;
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    else
                    {
                        ask_for_path();
                        current_path_point = 0;
                    }
                }

            }
            hold_block = false;
            send_holding();
             while (is_parner_holding())
             {
                 std::this_thread::sleep_for(std::chrono::milliseconds(50));
              }
             send_push();
             char buffer[10000];
             int size = recv(socketServer, buffer, 10000, 0);
             printf("koniec");
        }
    }
   
    void other_behaviour()
    {
        while (1)
        {
            ask_for_path();
            if (path.size() == 0)
                break;
            while (position != path[path.size() - 1])
            {

                printf("Jeszcze jak");
                printf("%d , %d\n", position.x, position.y);
                if (send_move(path[current_path_point]))
                {
                    printf("ruszam sie");
                    position = path[current_path_point];
                    current_path_point += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

            }
            hold_block = true;
            send_holding();
            printf("zlapalem");
            while (1)
            {
                char buffer[10000];
                int size = recv(socketServer, buffer, 10000, 0);
                buffer[size] = '\0';
                printf("%s", buffer);
                if (strncmp(buffer, "path", std::strlen("path")) == 0)
                {
                    parse_path(buffer);
                }
                if (strcmp(buffer, "ask_move") == 0)
                {
                    printf("%d", path.size());
                    if (path.empty())
                    {
                        send(socketServer, "NO\0", 4, 0);
                    }
                    else
                    {


                        send(socketServer, "yes\0", 5, 0);
                        if (send_move(path[current_path_point + 1]))
                        {
                            position = path[current_path_point + 1];
                            current_path_point += 1;
                        }

                    }
                }
                if (strcmp(buffer, "drop") == 0)
                {
                    printf("Rzucam kamien");
                    hold_block = false;
                }
                if (strcmp(buffer, "push_end") == 0)
                {
                    printf("Rzucam kamieniem");
                    break;
                    printf("koniec");
                }
                buffer[0] = '\0';
            }
        }

    }
    void get_position()
    {
       
            char buffer[2048];
            int nr_bytes = recv(socketServer, buffer, 2048, 0);
            int number1, number2;
            sscanf_s(buffer, "n{%d , %d}", &number1, &number2);
            blok.lock();
            position.x = number1;
            position.y = number2;
            if (number1 == 2)
                captain = true;
            blok.unlock();
            send(socketServer, "  ", 3, 0);
    }
    


};
int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a TCP socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up server address
    SOCKADDR_IN serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server" << std::endl;
    char buffer[BUFFER_SIZE];

 
   // Receive response from the server
    int numBytes = recv(serverSocket, buffer, BUFFER_SIZE, 0);
    if (numBytes == SOCKET_ERROR) {
        std::cerr << "Receive failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    buffer[numBytes] = '\0';
    printf("%d : %s", numBytes,buffer);

    robot ja(serverSocket,serverSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
