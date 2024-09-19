#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <iostream>

static int interrupted = 0;
static int port = 8080;

// Structure to hold per-session data
struct per_session_data {
    int number;
};

// Function called whenever a WebSocket event occurs
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len) {

    switch (reason) {

        case LWS_CALLBACK_ESTABLISHED:  // New connection
            std::cout << "Connection established" << std::endl;
            break;

        case LWS_CALLBACK_RECEIVE:      // Receiving data
            std::cout << "Received: " << std::string((const char *)in, len) << std::endl;
            lws_write(wsi, (unsigned char *)in, len, LWS_WRITE_TEXT);  // Echo message
            break;

        case LWS_CALLBACK_CLOSED:       // Connection closed
            std::cout << "Connection closed" << std::endl;
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
            "http-only",     // Protocol name
            callback_http,   // Callback function for WebSocket events
            sizeof(struct per_session_data), // Per-session data size
        },
        { NULL, NULL, 0 } // End of list
    };

    memset(&info, 0, sizeof info);
    info.port = port;
    info.protocols = protocols;

    // Create WebSocket context
    context = lws_create_context(&info);
    if (!context) {
        std::cerr << "Failed to create WebSocket context" << std::endl;
        return -1;
    }

    signal(SIGINT, signal_handler);  // Graceful shutdown on Ctrl+C

    std::cout << "WebSocket server running on ws://localhost:" << port << std::endl;

    // Event loop
    while (!interrupted) {
        lws_service(context, 1000); // Wait for events
    }

    // Cleanup
    lws_context_destroy(context);

    std::cout << "WebSocket server stopped" << std::endl;
    return 0;
}
