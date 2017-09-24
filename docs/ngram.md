# Ngram

## Introduction
Ngram is a command line based program which takes a list of conversations and a word as input, and outputs
the word that could possible follow the provided one. The current release of the this tool builds an Ngram 
table where N = 2, also called Bigram.

## Build the tool
* Clone repository
* Clone submodules `git clone submodule update --init --recursive`
* Go the tool directory `cd tools/ngram/`
* Build the tool `cmake . && make`

## How to use Ngram tool?
Ngram provides output on request. The information displayed on the console can be controlled using the flags
and arguments that are passed to the program. In the output, the strings `<s>` and `</s>` denote the 
beginning and the end of a sentence.

*Note: Use the `-h` or `--help` flags to learn more about the latest functionalities of the tool.*

## How does it work?
The Ngram tool stores a Bigram table and a Word occurence vector. The two latter are used to guess what are
the possible words that could follow the provided one.

### Bigram table
The Bigram table is a lookup table that keeps track of the number of times the word W<sub>i</sub> occurres 
before W<sub>i+1</sub>.

### Word occurence
The Word occurence vector keeps track of how many times a word occurred in the provided conversations.

### Guess the next word
Using the Bigram table and the word occurence vector, W<sub>i+1</sub> can be guessed:  
P(W<sub>i+1</sub> | W<sub>i</sub>) = Count(W<sub>i</sub>, W<sub>i+1</sub>) / Count(W<sub>i</sub>)

*Note: The program will print all the possible words sorted by their probabilities.*

## Examples
### Example 1
```
$ cat dataset.xml | ./ngram -i -- -c -w "<s>" -T 0.2
Ngram table
-----------
count(<s>, Hey) = 1
count(<s>, I’m) = 1
count(<s>, Me) = 2
count(<s>, Nice) = 1
count(<s>, Who’s) = 1
count(Hey, how) = 1
count(I’m, fine) = 1
count(Me, </s>) = 1
count(Me, too) = 1
count(Nice, </s>) = 1
count(Who’s, around) = 1
count(are, you) = 1
count(around, for) = 1
count(fine, thank) = 1
count(for, lunch) = 1
count(how, are) = 1
count(lunch, </s>) = 1
count(thank, you) = 1
count(too, </s>) = 1
count(you, </s>) = 2
---

Ngram word occurrence
---------------------
count(</s>) = 6
count(<s>) = 6
count(Hey) = 1
count(I’m) = 1
count(Me) = 2
count(Nice) = 1
count(Who’s) = 1
count(are) = 1
count(around) = 1
count(fine) = 1
count(for) = 1
count(how) = 1
count(lunch) = 1
count(thank) = 1
count(too) = 1
count(you) = 2
---

Possible word(s) after <s>
---------------------
P(Me | <s>) = 0.333333
```

### Example 2
```
$ cat dataset.xml | ./ngram -i -- -c -w "hey" -I
Ngram table
-----------
count(<s>, hey) = 1
count(<s>, i’m) = 1
count(<s>, me) = 2
count(<s>, nice) = 1
count(<s>, who’s) = 1
count(are, you) = 1
count(around, for) = 1
count(fine, thank) = 1
count(for, lunch) = 1
count(hey, how) = 1
count(how, are) = 1
count(i’m, fine) = 1
count(lunch, </s>) = 1
count(me, </s>) = 1
count(me, too) = 1
count(nice, </s>) = 1
count(thank, you) = 1
count(too, </s>) = 1
count(who’s, around) = 1
count(you, </s>) = 2
---

Ngram word occurrence
---------------------
count(</s>) = 6
count(<s>) = 6
count(are) = 1
count(around) = 1
count(fine) = 1
count(for) = 1
count(hey) = 1
count(how) = 1
count(i’m) = 1
count(lunch) = 1
count(me) = 2
count(nice) = 1
count(thank) = 1
count(too) = 1
count(who’s) = 1
count(you) = 2
---

Possible word(s) after hey
---------------------
P(how | hey) = 1
```
