#ifndef guess_h
#define guess_h

#include <algorithm>
#include<lguess.h>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <queue>
#include <functional>


// Collection of letter guesses
class WordleSolver {
    public:
        WordleSolver(std::string filename, size_t wlen, int n);

        // Update the words_left with the new information
        void update(
            const std::string& word, 
            const std::vector<std::string> marks
        );

        // Returns true when:
        // - There are no words left in words_left (word not in dict)
        // - There is one word left in words_left (that must be it)
        // And prints some info about this to the console
        bool done() const;

        // Does what it says on the tin
        void calculate_best_guess() const;

    private:
        // Reads in a dictionary file
        void read_dict(std::string filename, size_t wlen);
        // Stores the original full dictionaries
        std::unique_ptr<std::list<std::string>> dict;
        // Stores all the words that are compatible with previous guesses
        std::vector<std::unique_ptr<std::list<std::string>>> words_lefts;
        // Stores all the characters already guessed correctly
        std::vector<std::string> guessed;
        // The length of the words
        const size_t wlen;
        // The amount of simultanious instances
        const int n_instances;
};

#endif
