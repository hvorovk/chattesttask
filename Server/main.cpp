#include "src/tt.server.h"

#include <iostream>
#include <string>

int main()
{
    TTServer server;

    server.start();

    while (server.currentState() == TTServer::State::Up) {
        sleep(1);
    }

    return 0;
}
