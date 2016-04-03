#pragma once

#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "GPIO.h"

namespace bsdroid {

    class DHT11 {
        public:
            enum RESULT {
                OK = 0,
                BAD_DATA,
                BAD_CHECKSUM,
            };

        private:
            enum STATE {
                INIT_PULL_DOWN = 1,
                INIT_PULL_UP,
                DATA_FIRST_PULL_DOWN,
                DATA_PULL_UP,
                DATA_PULL_DOWN
            };

        public:
            DHT11(GPIO::Pin pin): _pin(pin), _vals(1000, 0) { }

            RESULT read(int& humidity, int& temperature) {
                collect();
                return parse(humidity, temperature);
            };

        private:
            void collect() {
                using namespace std::chrono_literals;

                _pin.setFlags(GPIO_PIN_OUTPUT);
                _pin.setHigh();
                std::this_thread::sleep_for(10ms);
                _pin.setLow();
                std::this_thread::sleep_for(18ms);

                _pin.setFlags(GPIO_PIN_INPUT | GPIO_PIN_PULLUP);
                for (size_t i = 0; i < _vals.size(); ++i) {
                    _vals[i] = _pin.value();
                }
            }

            RESULT parse(int& humidity, int& temperature) {
                _hist.clear();
                _hist.reserve(40);
                int max = 0, min = _vals.size();
                auto state = STATE::INIT_PULL_DOWN;
                for (size_t i = 0, n = 0; i < _vals.size(); ++i, ++n) {
                    switch (state) {
                        case STATE::INIT_PULL_DOWN:
                            if (_vals[i] == GPIO_PIN_LOW) {
                                state = STATE::INIT_PULL_UP;
                            }
                            break;

                        case STATE::INIT_PULL_UP:
                            if (_vals[i] == GPIO_PIN_HIGH) {
                                state = STATE::DATA_FIRST_PULL_DOWN;
                            }
                            break;

                        case STATE::DATA_FIRST_PULL_DOWN:
                            if (_vals[i] == GPIO_PIN_LOW) {
                                state = STATE::DATA_PULL_UP;
                            }
                            break;

                        case STATE::DATA_PULL_UP:
                            if (_vals[i] == GPIO_PIN_HIGH) {
                                n = 0;
                                state = STATE::DATA_PULL_DOWN;
                            }
                            break;

                        case STATE::DATA_PULL_DOWN:
                            if (_vals[i] == GPIO_PIN_LOW) {
                                _hist.push_back(n);
                                if (n > max) {
                                    max = n;
                                } else if (n < min) {
                                    min = n;
                                }
                                state = STATE::DATA_PULL_UP;
                            }
                            break;

                        default:
                            break;
                    }
                }
                if (_hist.size() != 40) {
                    return RESULT::BAD_DATA;
                }

                unsigned char bytes[5];
                const int mid = min + (max - min) / 2.f;
                for (size_t i = 0, ibyte = 0; i < _hist.size() && ibyte < 5; ++i) {
                    bytes[ibyte] <<= 1;
                    if (_hist[i] > mid) {
                        bytes[ibyte] |= 1;
                    }
                    if ((i+1) % 8 == 0) {
                        ++ibyte;
                    }
                }
                const unsigned char checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
                if (checksum != bytes[4]) {
                    return RESULT::BAD_CHECKSUM;
                }
                humidity = bytes[0];
                temperature = bytes[2];
                return RESULT::OK;
            }

        private:
            GPIO::Pin _pin;
            std::vector<int> _vals;
            std::vector<int> _hist;
    };

}
