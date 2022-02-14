# brute-fordle
Beat your friends at [wordle](https://www.nytimes.com/games/wordle/index.html).
Or perhaps just witness how this thing solves wordles (it seems to make no sense at first, but then somehow it beats you.)

Literally just maintains [a list of words](include/solver.h#L39) that fit the guesses made so far.
Greedily suggests words to guess based on how many words, on average, are left in the list after that 
guess is made. Assumes all words are equally likely to be the sought after word.

## Compression
Uses a [tree structure](include/evaltree.h) to [compress](src/evaltree.cc#L157) this data, and to 
allow for the [re-use of certain intermediate results](src/evaltree.cc#L172). This gives the program a 
factor 50-100 speedup and a factor 25-30 decrease in memory use, making it feasible to run on most modern machines.

## Best first word
When no words have been guessed yet, the program goes through |dictionary|<sup>2</sup> possible (guess+word)
combinations. This is by far the most work-intensive step, taking about 80 seconds for dictionaries of about 
10.000 words (on my machine). After having calculated the best starter word one time, you can skip the first 
round using the `-s` argument.

## Dictionaries
The program expects a dictionary with all lowercase words.
If one specifies the `-l n` argument, the program automatically 
filters out all words that do not have length `n`.
