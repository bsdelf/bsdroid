// c++ -O2 -std=c++14 -lgpio -I../lib DHT11.cc 

#include <stdio.h>
#include <stdlib.h>
#include <DHT11.h>

int main(int argc, char* argv[]) {
    using namespace bsdroid;
    using namespace std::chrono_literals;

    if (argc < 2) {
        printf("Usage: %s <pin>\n", argv[0]);
        exit(1);
    }

    GPIO::Pin pin = GPIO().pin(atoi(argv[1]));
    auto config = pin.config();
    printf("name: %s, caps: %u, flags: %u\n", config.g_name, config.g_caps, config.g_flags);

    DHT11 dht11(pin);
    for (int humidity, temperature; ; ) {
        auto ok = dht11.read(humidity, temperature);
        if (ok == DHT11::RESULT::OK) {
            printf("%d%%, %d°C\n", humidity, temperature);
        };
        std::this_thread::sleep_for(500ms);
    }

    exit(0);
}
