/*
 * Circular String Matching
 * Copyright (C) 2015 Xenofon Kyriakidis, Solon Pissis and Ahmad Retha
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CSM__
#define __CSM__

#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <math.h>
#include <time.h>

#define ALPHABET_DNA   'D'
#define ALPHABET_PROT  'P'
#define ALPHABET_IUPAC 'I'
#define DNA   "ACGTN"                 //DNA alphabet
#define IUPAC "ACGTUWSMKRYBDHVN"      //IUPAC nucleotide alphabet
#define PROT  "ARNDCQEGHILKMFPSTWYVX" //Proteins alphabet
#define PENALTY_DEL 1
#define PENALTY_INS 1
#define PENALTY_SUB 1

using namespace std;

class CircularStringMatching
{
private:
    /**
     * @var alphabet The numerical representation of the DNA alphabet - a=0, c=1, g=2 and t=3
     */
    unsigned int * alphabet;
    /**
     * @var antiAlphabet The char representation of the DNA alphabet - 0=a, 1=c, 2=g and 3=t
     */
    char * antiAlphabet;
    /**
     * @var sigma The size of the alphabet
     */
    unsigned int sigma;
    /**
     * @var a Alphabet type
     */
    char a;
    /**
     * @var d See lemma 4
     */
    double d;
    /**
     * @var c See lemma 4
     */
    double c;
    /**
     * @var pattern The initial pattern being searched for
     */
    string * pattern;
    /**
     * @var m The length of the pattern
     */
    unsigned int m;
    /**
     * @var text The text to search
     */
    string * text;
    /**
     * @var n The length of the text
     */
    unsigned int n;
    /**
     * @var k The minimum number of errors allowed in a match
     */
    unsigned int k;
    /**
     * @var q The size of the q-gram
     */
    unsigned int q;
    /**
     * @var qGramBackwards How many q-grams to read backwards from a window
     */
    int qGramBackwards;
    /**
     * @var verifiedWindowSize The maximum positions to move the window if verified
     */
    int verifiedWindowShift;
    /**
     * @var unverifiedWindowSize The maximum positions to move the window if unverified
     */
    int unverifiedWindowShift;
    /**
     * @var M The M array which holds the edit distances of preprocessing the pattern
     */
    unsigned int * M = NULL;
    
    /**
     * Initializes the alphabet indices
     */
    void initAlphabet();

    /**
     * Compares characters a and b and returns 0 if they match or the substitution penalty
     *
     * @param a
     * @param b
     * @return 0 if a and b match, otherwise the substitution penalty
     */
    int delta(char a, char b)
    {
        return (a == b) ? 0 : PENALTY_SUB;
    }

    /**
     * Calculates string s
     *
     * @param s
     * @param i
     */
    void calculateS(char *s, int i);

    /**
     * Calculates string u
     *
     * @param pattern
     * @param u
     * @param i
     */
    void calculateU(char *pattern, char *u, int i);

    /**
     * Preprocessing - Creates the M array and populates it
     *
     * @param patternDoubled
     * @return
     */
    int preprocessing(char *patternDoubled);

    /**
     * Edit Distance for preprocessing
     *
     * @param pattern
     * @param m The length of the pattern
     * @param qgram
     * @param n The length of the qgram
     */
    int EditDistance(char *pattern, int m, char *qgram, int n);

    /**
     * Edit Distance for verification
     *
     * @param pattern
     * @param m The length of the pattern
     * @param text
     * @param n The length of the text
     * @param outputVector
     */
    void EditDistance(char *pattern, int m, char *text, int n, int *outputVector);

    /**
     * Takes the verification edit distance row and sticks it in the outputVector if < k
     *
     * @param editDistanceVector
     * @param n
     * @param rotation 
     * @param outputVector
     * @param window2mStart
     */
    void checkVector(int *editDistanceVector, int n, int rotation, vector<vector<unsigned int>> &outputVector, unsigned int window2mStart);

    /**
     * Verify
     *
     * @param pattern
     * @param m Length of pattern
     * @param text
     * @param n Length of text
     * @param outputVector
     * @param window2mStart
     */
    void verification(char *pattern, int m, char *text, int n, vector<vector<unsigned int>> &outputVector, unsigned int window2mStart);

    /**
     * Prints best match found
     *
     * @param outputVector
     */
    void printOutputVector(vector<vector<unsigned int>> &outputVector);

    /**
     * Rotates a given cstring
     * 
     * @param x a cstring
     * @param offset where to rotate it at
     * @param rotation resulting rotated string
     */
    void rotate(char * x, int offset, char * rotation);
    
    /**
     * Returns the numerical representation of a string qgram
     * 
     * @param qgram A qgram char array
     * @return The numerical qgram
     */
    unsigned int getQIndex(char * qgram);

public:

    /**
     * The Circular (DNA) String Matching class
     *
     * @param pattern
     * @param m The length of the pattern
     * @param text
     * @param n The length of the text
     * @param k The maximum number of errors permitted before skipping a window (inclusive)
     * @param a The alphabet of the sequence (DNA/Protein)
     */
    CircularStringMatching(string * pattern, unsigned int m, string * text, unsigned int n, unsigned int k, char a);
    
    /**
     * Free memory
     */
    ~CircularStringMatching()
    {
	if (this->M != NULL) {
	    free(this->M);
	}
	free(this->alphabet);
	free(this->antiAlphabet);
    }

    /**
     * Run the algorithm
     */
    int run();
};

#endif
