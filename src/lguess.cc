#include "lguess.h"
#include <iostream>

bool LetterEval::operator <(const LetterEval &g) const { 
    if(type == g.type) {
        if(idx == g.idx) {
            return letter < g.letter;
        }
        return idx < g.idx;
    }
    return type < g.type;
}

std::ostream& operator << (std::ostream& s, const LetterEval& l) {
    l.print(s);
    return s;
}


void EvalRight::filter(std::list<std::string>* const dict) const {
    std::erase_if(*dict, 
        [this](const std::string &s){ return s[idx] != letter; }
    );
} 
        
std::ostream& EvalRight::print(std::ostream& s) const {
    s << "Right, " << idx << " , " << letter;
    return s;
}


void EvalPlace::filter(std::list<std::string>* const dict) const {
    std::erase_if(*dict, 
        [this](const std::string &s) { return 
            s.find(letter) == std::string::npos
            || s[idx] == letter; 
        }
    );
} 

std::ostream& EvalPlace::print(std::ostream& s) const {
    s << "Place, " << idx << " , " << letter;
    return s;
}


void EvalWrong::filter(std::list<std::string>* const dict) const {
    std::erase_if(*dict,
        [this](const std::string &s){ 
            int found = 0;

            for(size_t i = 0; i < s.length(); i++) {
                if(s[i] == letter) found++;
            }

            if (found > allow) return true;
            else return false;
        }
    );
}

std::ostream& EvalWrong::print(std::ostream& s) const {
    s << "Wrong, " << idx << " , " << letter;
    if(allow != 0) { s << " (allow " << allow << ")"; }
    return s;
        }
