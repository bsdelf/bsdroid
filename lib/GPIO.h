#pragma once

#include <libgpio.h>

namespace bsdroid {
    class GPIO {
        friend class Pin;

        public:
        class Pin {
            friend class GPIO;

            public:
            gpio_config_t config() const {
                gpio_config_t config = { .g_pin = _pin };
                gpio_pin_config(_gpioc, &config);
                return config;
            }

            void setName(const std::string& name) {
                gpio_pin_set_name(_gpioc, _pin, const_cast<char*>(name.c_str()));
            }

            void setFlags(int flags) {
                auto&& pinConf = config();
                pinConf.g_flags = flags;
                gpio_pin_set_flags(_gpioc, &pinConf);
            }

            int value() const {
                return gpio_pin_get(_gpioc, _pin);
            }

            void toggle() {
                gpio_pin_toggle(_gpioc, _pin);
            }

            void setValue(int value) {
                gpio_pin_set(_gpioc, _pin, value);
            }

            void setHigh() {
                gpio_pin_low(_gpioc, _pin);
            }

            void setLow() {
                gpio_pin_low(_gpioc, _pin);
            }

            private:
            Pin(int gpioc, int pin, std::shared_ptr<int> ref):
                _gpioc(gpioc), _pin(pin), _ref(ref) { }

            public:
            ~Pin() {
                if (_ref.unique()) {
                    gpio_close(_gpioc);
                }
            }

            private:
            const int _gpioc;
            const gpio_pin_t _pin;
            const std::shared_ptr<int> _ref;
        };

        public:
        explicit GPIO(int ith = 0) {
            _gpioc = gpio_open(ith);
            _ref = std::make_shared<int>();
        }

        explicit GPIO(const char* device) {
            _gpioc = gpio_open_device(device);
            _ref = std::make_shared<int>();
        }

        ~GPIO() {
            if (_ref.unique()) {
                gpio_close(_gpioc);
            }
        }

        Pin pin(gpio_pin_t pin) {
            return Pin(_gpioc, pin, _ref);
        }

        private:
        int _gpioc;
        std::shared_ptr<int> _ref;
    };
}
