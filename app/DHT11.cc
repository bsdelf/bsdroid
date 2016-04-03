// c++ -O2 -std=c++14 DHT11.cc -lgpio -I../lib

#include <iostream>
#include <DHT11.h>

int main(int argc, char* argv[]) {
    using namespace bsdroid;

    GPIO::Pin pin = GPIO().pin(121);
    auto config = pin.config();
    printf("name: %s, caps: %u, flags: %u\n", config.g_name, config.g_caps, config.g_flags);

    DHT11 dht11(pin);
    for (int humidity, temperature; ; ) {
        auto ok = dht11.read(humidity, temperature);
        if (ok == DHT11::RESULT::OK) {
            std::cout << humidity << ", " << temperature << std::endl;
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);
        };
    }

    return 0;
}
