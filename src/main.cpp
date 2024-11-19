#include "BFileX.hpp"
#include "CommandLineParser.hpp"

int main(const int argc, char** argv) {
    CommandLineParser::parse(argc, argv);

    BFileX BFileX;
    BFileX.run();
}
