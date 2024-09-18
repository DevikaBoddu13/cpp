#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <set>
#include <vector>

static int interrupted = 0;
static int port = 8080;

struct per_session_data {
    int id;
};

std::set<struct lws *> clients;

// Function to broadcast message to all clients
void broadcast_message(const std::string& message, struct lws *sender) {
    for (auto client : clients) {
        //if (client != sender) {  
            size_t message_size = message.size();
            std::vector<unsigned char> buffer(LWS_PRE + message_size);  
            memcpy(&buffer[LWS_PRE], message.c_str(), message_size);   

            lws_write(client, &buffer[LWS_PRE], message_size, LWS_WRITE_TEXT);
        //}
    }
}

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len) {

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:   
            std::cout << "New client connected" << std::endl;
            clients.insert(wsi);  
            break;

        case LWS_CALLBACK_RECEIVE: {     
            std::string message = std::string((const char *)in, len);
            std::cout << "Received: " << message << std::endl;

            broadcast_message(message, wsi);
            break;
        }

        case LWS_CALLBACK_CLOSED:       
            std::cout << "Client disconnected" << std::endl;
            clients.erase(wsi);  
            break;

        default:
            break;
    }

    return 0;
}

// Signal handler to allow graceful termination
void signal_handler(int sig) {
    interrupted = 1;
}

int main(void) {
    struct lws_context_creation_info info;
    struct lws_context *context;

    struct lws_protocols protocols[] = {
        {
            "http-only",     
            callback_http,    
            sizeof(struct per_session_data),  
        },
        { NULL, NULL, 0 }  
    };

    memset(&info, 0, sizeof info);
    info.port = port;
    info.protocols = protocols;

    // Creating WebSocket context
    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Failed to create WebSocket context" << std::endl;
        return -1;
    }

    signal(SIGINT, signal_handler);  // Ctrl+C

    std::cout << "WebSocket chat server running on ws://localhost:" << port << std::endl;

    
    while (!interrupted) {
        lws_service(context, 1000);  
    }

     
    lws_context_destroy(context);

    std::cout << "WebSocket server stopped" << std::endl;
    return 0;
}
