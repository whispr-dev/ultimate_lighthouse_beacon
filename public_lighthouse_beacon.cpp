#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include <iomanip>
#include <sstream>

// 🌍 PUBLIC LIGHTHOUSE BEACON SYSTEM 🌍
// Anyone on Earth can connect to YOUR_SERVER_IP:9876 and listen!

class PublicLighthouseBeacon {
private:
    std::atomic<bool> running{true};
    std::vector<int> connected_listeners;
    std::mutex listeners_mutex;
    int server_socket{-1};
    int beacon_port{9876};
    
    struct BeaconData {
        std::string lighthouse_id{"public-lighthouse-001"};
        std::string status{"healthy"};
        std::string fastping_status{"unknown"};
        uint64_t timestamp{0};
        double ping_latency{0.0};
        int signal_age{0};
        int connected_listeners{0};
    };
    
    BeaconData current_beacon;
    std::mutex beacon_mutex;
    
public:
    PublicLighthouseBeacon() {
        std::cout << "🌍 PUBLIC LIGHTHOUSE BEACON SYSTEM 🌍\n";
        std::cout << "=====================================\n";
        std::cout << "🏰 Broadcasting to the ENTIRE INTERNET!\n";
        std::cout << "📡 Anyone can connect to: YOUR_SERVER:9876\n\n";
    }
    
    bool start() {
        // Create server socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
            std::cerr << "❌ Failed to create socket\n";
            return false;
        }
        
        // Allow address reuse
        int opt = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // Bind to public port
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept from ANYONE
        server_addr.sin_port = htons(beacon_port);
        
        if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "❌ Failed to bind to port " << beacon_port << "\n";
            return false;
        }
        
        if (listen(server_socket, 50) < 0) {  // Allow 50 concurrent listeners
            std::cerr << "❌ Failed to listen\n";
            return false;
        }
        
        std::cout << "🚀 PUBLIC LIGHTHOUSE OPERATIONAL!\n";
        std::cout << "🌍 Listening on port " << beacon_port << " for GLOBAL connections\n";
        std::cout << "📡 Broadcasting lighthouse data to ALL connected listeners\n\n";
        
        // Start worker threads
        std::thread(&PublicLighthouseBeacon::connectionHandler, this).detach();
        std::thread(&PublicLighthouseBeacon::fastpingMonitor, this).detach();
        std::thread(&PublicLighthouseBeacon::beaconBroadcaster, this).detach();
        
        return true;
    }
    
private:
    // Accept connections from anywhere in the world
    void connectionHandler() {
        while (running) {
            sockaddr_in client_addr{};
            socklen_t addr_len = sizeof(client_addr);
            
            int client_socket = accept(server_socket, (sockaddr*)&client_addr, &addr_len);
            if (client_socket >= 0) {
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                
                std::cout << "🌍 NEW LISTENER CONNECTED: " << client_ip << std::endl;
                
                {
                    std::lock_guard<std::mutex> lock(listeners_mutex);
                    connected_listeners.push_back(client_socket);
                }
                
                // Send welcome message
                std::string welcome = "🏰 Welcome to PUBLIC LIGHTHOUSE BEACON!\n";
                welcome += "📡 You are now receiving live lighthouse data\n\n";
                send(client_socket, welcome.c_str(), welcome.length(), 0);
            }
        }
    }
    
    // Monitor FastPing (or any external service)
    void fastpingMonitor() {
        while (running) {
            // Simple HTTP check (you can enhance this)
            CURL* curl = curl_easy_init();
            if (curl) {
                std::string response;
                curl_easy_setopt(curl, CURLOPT_URL, "http://httpbin.org/ip");
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
                
                CURLcode res = curl_easy_perform(curl);
                
                std::lock_guard<std::mutex> lock(beacon_mutex);
                if (res == CURLE_OK) {
                    current_beacon.fastping_status = "ok";
                    current_beacon.status = "healthy";
                    current_beacon.ping_latency = 15.5; // Mock latency
                } else {
                    current_beacon.fastping_status = "failed";
                    current_beacon.status = "warning";
                }
                
                current_beacon.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                current_beacon.connected_listeners = connected_listeners.size();
                
                curl_easy_cleanup(curl);
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    
    // Broadcast to ALL connected listeners
    void beaconBroadcaster() {
        while (running) {
            std::string beacon_json;
            
            {
                std::lock_guard<std::mutex> lock(beacon_mutex);
                beacon_json = createBeaconJson(current_beacon);
            }
            
            // Broadcast to ALL listeners
            {
                std::lock_guard<std::mutex> lock(listeners_mutex);
                auto it = connected_listeners.begin();
                
                while (it != connected_listeners.end()) {
                    int client_socket = *it;
                    
                    ssize_t sent = send(client_socket, beacon_json.c_str(), beacon_json.length(), MSG_NOSIGNAL);
                    
                    if (sent <= 0) {
                        // Client disconnected
                        std::cout << "📡 Listener disconnected\n";
                        close(client_socket);
                        it = connected_listeners.erase(it);
                    } else {
                        ++it;
                    }
                }
                
                if (!connected_listeners.empty()) {
                    std::cout << "📡 Broadcasted to " << connected_listeners.size() << " listeners\n";
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
    
    std::string createBeaconJson(const BeaconData& beacon) {
        std::ostringstream json;
        json << "{\n";
        json << "  \"lighthouse_id\": \"" << beacon.lighthouse_id << "\",\n";
        json << "  \"timestamp\": " << beacon.timestamp << ",\n";
        json << "  \"status\": \"" << beacon.status << "\",\n";
        json << "  \"fastping_status\": \"" << beacon.fastping_status << "\",\n";
        json << "  \"ping_latency_ms\": " << beacon.ping_latency << ",\n";
        json << "  \"signal_age_seconds\": " << beacon.signal_age << ",\n";
        json << "  \"connected_listeners\": " << beacon.connected_listeners << ",\n";
        json << "  \"lighthouse_type\": \"PUBLIC_INTERNET_BEACON\"\n";
        json << "}\n\n";
        
        return json.str();
    }
};

int main() {
    PublicLighthouseBeacon lighthouse;
    
    if (!lighthouse.start()) {
        std::cerr << "❌ Failed to start public lighthouse\n";
        return 1;
    }
    
    std::cout << "🏰 PUBLIC LIGHTHOUSE IS BROADCASTING TO THE WORLD!\n";
    std::cout << "🌍 Anyone can connect with: nc YOUR_SERVER_IP 9876\n";
    std::cout << "Press Enter to stop...\n";
    
    std::cin.get();
    return 0;
}
