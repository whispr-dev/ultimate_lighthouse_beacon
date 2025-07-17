#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <iomanip>

void print_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") << "] ";
}

int main(int argc, char* argv[]) {
    std::string server_ip = "localhost";  // Default to localhost
    int server_port = 9876;
    
    // Allow custom IP from command line
    if (argc > 1) {
        server_ip = argv[1];
    }
    if (argc > 2) {
        server_port = std::stoi(argv[2]);
    }
    
    std::cout << "🔥 TCP Lighthouse Listener v3.0\n";
    std::cout << "===============================\n";
    std::cout << "📡 Connecting to: " << server_ip << ":" << server_port << "\n";
    std::cout << "🏰 Receiving PUBLIC lighthouse data\n\n";

    // Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "❌ Socket creation failed\n";
        return 1;
    }

    // Connect to lighthouse server
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        // Try localhost if IP conversion fails
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    }
    
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "❌ Connection to lighthouse failed\n";
        std::cerr << "   Make sure lighthouse is running on " << server_ip << ":" << server_port << "\n";
        close(sock);
        return 1;
    }
    
    std::cout << "✅ Connected to PUBLIC lighthouse!\n";
    std::cout << "📡 Receiving live lighthouse data...\n\n";
    
    char buffer[1024];
    int packet_count = 0;
    
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            std::cout << "📡 Lighthouse disconnected\n";
            break;
        }
        
        buffer[bytes_received] = '\0';
        packet_count++;
        
        print_timestamp();
        std::cout << "🏰 LIGHTHOUSE BEACON #" << packet_count << std::endl;
        std::cout << "📦 " << bytes_received << " bytes received" << std::endl;
        std::cout << "📄 Data: " << buffer << std::endl;
        std::cout << "═══════════════════════════════════════════\n";
    }
    
    close(sock);
    return 0;
}
