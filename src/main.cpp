#include "BFileX.hpp"
#include "CommandLineParser.hpp"

int main(const int argc, char** argv) {
    // handling command line options
    CommandLineParser::parse(argc, argv);

    BFileX BFileX;
    BFileX.run();
}
