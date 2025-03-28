module;
#include "libraries.h"

export module app;

export namespace dx12 {

    class Application {
    public:
        Application() {
            std::cout << "Hello from Application()\n";
        }
    };

}