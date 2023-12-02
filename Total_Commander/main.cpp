#include "app.hpp"

int main() {

    app::Total_Commander application;

    do{
        application.run();
    }while(application.closed());

    return 0;
}
