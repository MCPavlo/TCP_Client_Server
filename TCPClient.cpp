#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

class Client {
public:
    Client(const std::string& name, int port, int period)
        : name(name), port(port), period(period) {}

    void start() {
        while (true) {
            int sock = 0;
            struct sockaddr_in serv_addr;
            char buffer[1024] = {0};

            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cerr << "Socket creation error" << std::endl;
                return;
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);

            if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
                std::cerr << "Invalid address/ Address not supported" << std::endl;
                return;
            }

            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << "Connection Failed" << std::endl;
                return;
            }

            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            std::tm buf;
            localtime_r(&in_time_t, &buf);

            std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S", &buf);
            std::sprintf(buffer + std::strlen(buffer), ".%03d] %s", static_cast<int>(ms.count()), name.c_str());

            send(sock, buffer, std::strlen(buffer), 0);
            close(sock);

            std::this_thread::sleep_for(std::chrono::seconds(period));
        }
    }

private:
    std::string name;
    int port;
    int period;
};

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <name> <port> <period>" << std::endl;
        return 1;
    }

    std::string name = argv[1];
    int port = std::stoi(argv[2]);
    int period = std::stoi(argv[3]);

    Client client(name, port, period);
    client.start();

    return 0;
}

