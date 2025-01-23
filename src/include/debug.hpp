#pragma once
#include <fstream>

namespace debug {
    inline void log(const std::string& message) {
        static size_t lineNumber{};
        if (std::ofstream file("/home/bahaa/BFileX/logs.txt", std::ios::app); file.is_open()) {
            file << ++lineNumber << ". " << message << std::endl;
        }
    }

    class Timer {
    public:
        explicit Timer(std::string label, std::string message = {})
            : start(std::chrono::high_resolution_clock::now()), message(std::move(message)), label(std::move(label)) {}

        ~Timer() {
            const auto end = std::chrono::high_resolution_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            auto milliseconds = microseconds / 1000;
            microseconds %= 1000;

            const auto seconds = milliseconds / 1000;
            milliseconds %= 1000;

            log("[" + label + "]" + message + " elapsed: " +
                std::to_string(seconds) + "s, " +
                std::to_string(milliseconds) + "ms, " +
                std::to_string(microseconds) + "µs"
            );
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
        std::string message;
        std::string label;
    };

#define functionTimer Timer timer(__func__)
}
