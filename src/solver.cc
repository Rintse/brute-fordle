#include "solver.h"
#include "evaltree.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>


// Word guess: collection of letter guesses
WordleSolver::WordleSolver(std::string filename, size_t l) : wlen(l) {
    // Read in a dictionary file
    read_dict(filename, wlen);
    // Set the guess to nothing
    guessed = std::string(wlen, '.');
}


// Reads a sorted dictionary file to use for suggestion
void WordleSolver::read_dict(std::string filename, size_t wlen) {
    dict = std::make_unique<std::list<std::string>>();
    std::ifstream dict_file(filename);

    if(dict_file.is_open()){
        std::copy_if(
            std::istream_iterator<std::string>(dict_file),
            std::istream_iterator<std::string>(), // Empty itertor is OEF
            std::back_inserter(*dict),

            // Only copies words of length wlen
            [wlen](const std::string &s) { 
                return s.length() == wlen 
                && std::all_of(
                    s.begin(), s.end(), 
                    [](const char c){ return std::isalpha(c); }
                ); 
            }
        );
    }

    else { 
        throw std::runtime_error("Error reading file."); 
    }
    
    // Use only lower case letters in the rest of the program
    for(auto &word : *dict) {
        for(auto &i : word) {
            i = std::tolower(i);
        }
    }

    // We start with all words still in the running
    words_left = std::make_unique<std::list<std::string>>(*dict);
}



// Check if done and print something if so
bool WordleSolver::done() const {
    switch (words_left->size()) {
        case 0: 
            std::cout << "Word not in dictionary.\n";
            return true;

        case 1:
            std::cout << "The word is " << words_left->front() << "\n";
            return true;

        default: return false;
    }
}


// TODO: see if we can do something about the code 
// duplicaiton with evaluate(...) in evaltree.cc
void WordleSolver::update(
    const std::string& guess, 
    const std::string& marks
) {
    std::vector<std::unique_ptr<const LetterEval>> result;
    std::map<char,int> places;
    for(auto &c : guessed) { places[c]++; }

    if(guess.length() != marks.length()) {
        throw std::runtime_error("Word and marks dont have equal length");
    }

    // TODO: can this be done in a prettier way than 3 consecutive loops?
    for(size_t i = 0; i < guess.length(); i++) {
        if(marks[i] == 'v') {
            result.push_back(std::make_unique<EvalRight>(i, guess[i]));
            places[guess[i]]++;
            guessed[i] = guess[i];
        }
    }
    
    for(size_t i = 0; i < guess.length(); i++) {
        if(marks[i] == 'c') {
            result.push_back(std::make_unique<EvalPlace>(i, guess[i]));
            places[guess[i]]++;
        }
    }
    
    for(size_t i = 0; i < guess.length(); i++) {
        if(marks[i] == 'x') {
            result.push_back(std::make_unique<EvalWrong>(
                i, guess[i], places[guess[i]]
            ));
        }
    }

    if(result.size() != wlen)
        throw std::runtime_error("Invalid mark. Use [xcv]");

    for(auto &letter : result) {
        letter->filter(words_left.get());
    }
}


void WordleSolver::calculate_best_guess() const {
    EvalTree elim_solver(words_left.get(), dict.get(), guessed);

    elim_solver.best_guesses();
    elim_solver.show_remaining();
}

