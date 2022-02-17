#include "solver.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <argparse.h>

#define DEFAULT_LENGTH 5

// Specifies the arguments using the include/argparse.h header
// on github @ https://github.com/p-ranav/argparse
argparse::ArgumentParser arg_spec() {
    argparse::ArgumentParser program(
        "brutefordle", "dev", 
        argparse::default_arguments::help
    );

    program.add_argument("dictionary")
        .help("The dictionary to use when guessing");

    program.add_argument("-l", "--wordlength")
        .help("Only words of this length will be used")
        .default_value(DEFAULT_LENGTH)
        .scan<'d', int>();

    program.add_argument("-s", "--skipfirst")
        .help("Skip the first round (use if known).")
        .default_value(false)
        .implicit_value(true);

    return program;
}


int main(int argc, char** argv) {
    auto argparser = arg_spec();

    try{ argparser.parse_args(argc, argv); }

    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl << argparser;
        std::exit(1);
    } 

    // Argument parsing successful, grab arguments
    std::string dict_name = argparser.get<std::string>("dictionary");
    int wlen = argparser.get<int>("-l");
    bool suggest = !argparser.get<bool>("-s");
    
    // The main solver
    WordleSolver solver(dict_name, wlen);

    // Hold input strings
    std::string word;
    std::string marks;
    
    while(!solver.done()) {
        if(suggest) solver.calculate_best_guess();

        // Await user input
        std::cout 
            << "\nEnter the results for a word:\n"
            << "FORMAT: word {space or newline} marks\n"
            << "Where marks is a sequence of:\n"
            << "    v if in the correct spot\n"
            << "    c if in the word\n"
            << "    x if not in the word\n\n"
            << "Or use '!q' to quit the program.\n";
        
        // Get guess
        std::cin >> word;
        if(word == "!q") { break; }
        // And its result
        std::cin >> marks;
        
        try { solver.update(word, marks); } 

        catch (std::runtime_error& e) {
            std::cout 
                << "Something went wrong processing guess: " 
                << e.what() << "\n";

            suggest = false;
            continue;
        }

        suggest = true;
    }

    return 0;
}
