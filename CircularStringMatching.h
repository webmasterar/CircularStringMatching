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
#include <math.h>

#define PENALTY_DEL 1
#define PENALTY_INS 1
#define PENALTY_SUB 1

using namespace std;

class CircularStringMatching
{
private:
    /**
     * @var alphabet The DNA alphabet - a, c, g and t
     */
    char alphabet[4];
    /**
     * @var sigma The size of the alphabet
     */
    const int sigma = 4;
    /**
     * @var d
     */
    double d;
    /**
     * @var c
     */
    double c;
    /**
     * @var pattern The initial pattern being searched for
     */
    string pattern;
    /**
     * @var m The length of the pattern
     */
    unsigned int m;
    /**
     * @var text The text to search
     */
    string text;
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
     * @var verifiedWindowSize The maximum positions to move the window if verified
     */
    unsigned int verifiedWindowShift;
    /**
     * @var unverifiedWindowSize The maximum positions to move the window if unverified
     */
    unsigned int unverifiedWindowShift;

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
     * Preprocessing
     *
     * @param pattern
     * @param windowSize
     * @return The total minimum edit distance between s and any prefix of u
     */
    int preprocessing(char *pattern, int windowSize);

    /**
     * Edit Distance
     *
     * @param pattern
     * @param m The length of the pattern
     * @param text
     * @param n The length of the text
     */
    int EditDistance(char *pattern, int m, char *text, int n);

    /**
     * Edit Distance
     *
     * @param pattern
     * @param m The length of the pattern
     * @param text
     * @param n The length of the text
     * @param outputVector
     */
    void EditDistance(char *pattern, int m, char *text, int n, int *outputVector);

    /**
     * @todo Describe
     *
     * @param editDistanceVector
     * @param j
     * @param outputVector
     */
    void checkVector(int *editDistanceVector, int j, vector<vector<int>> &outputVector);

    /**
     * @todo Describe
     *
     * @param pattern
     * @param m Length of pattern
     * @param text
     * @param n Length of text
     * @param outputVector
     */
    void verification(char *pattern, int m, char *text, int n, vector<vector<int>> &outputVector);

    /**
     * Prints output vector
     *
     * @deprecated Not in current use
     * @param outputVector
     */
    void printOutputVector(vector<vector<int>> &outputVector);

    /**
     * Calculates window backwards
     *
     * @param windowBackwards
     * @param windowBackwardsSize
     * @param window
     * @param windowSize
     * @param qGramBackwards
     */
    void calculateWindowBackwards(char *windowBackwards, int &windowBackwardsSize, char *window, int windowSize, int &qGramBackwards);

    /**
     * Rotates a given cstring
     * @param x a cstring
     * @param offset
     * @param rotation returned rotated string
     */
    unsigned int rotate(char * x, int offset, char * rotation)
    {
	unsigned int m = strlen ( ( char * ) x );
	memmove ( &rotation[0], &x[offset], m - offset );
	memmove ( &rotation[m - offset], &x[0], offset );
	rotation[m] = '\0';
	return 1;
    }

public:

    /**
     * The Circular (DNA) String Matching class
     *
     * @param pattern
     * @param m The length of the pattern
     * @param text Assumes it is passed in doubled
     * @param n The length of the text (assumes it is doubled - actual length of text)
     * @param k The maximum number of errors permitted before skipping a window (inclusive)
     */
    CircularStringMatching(string pattern, unsigned int m, string text, unsigned int n, unsigned int k)
    {
	this->alphabet[0] = 'a';
	this->alphabet[1] = 'c';
	this->alphabet[2] = 'g';
	this->alphabet[3] = 't';
        this->pattern = pattern;
        this->m = m;
        this->text = text;
        this->n = n;
        this->k = k;

	this->c = abs(1 - (exp(1) / sqrt(this->sigma))); //@todo check: lemma 4 "The probability decreases exponentionally for d > 1, which holds if c < 1 - e/sqrt(sigma)
	cout << "c: " << c << endl;

	this->d = 1.1; //1 - this->c + (2 * this->c * (log(this->c) / log(this->sigma))) + (2 * (1 - this->c) * (log(1 - this->c) / log(this->sigma))); //lemma 4
	cout << "d: " << d << endl;

	double logK = (this->k == 0) ? 0 : log(this->k); //can't log k=0
        this->q = (unsigned int) ceil( ((3 * (log(this->m) / log(this->sigma))) + (logK / log(this->sigma))) / this->d ); //IX
	cout << "q: " << q << endl;

        this->verifiedWindowShift = this->m - this->k; //VIII
        cout << "vw: " << this->verifiedWindowShift << endl;
	this->unverifiedWindowShift = (unsigned int) (this->verifiedWindowShift - (this->q + (this->k / this->c))); //VIII
	cout << "uvw: " << this->unverifiedWindowShift << endl;
    }

    /**
     * Run the algorithm
     */
    int run();
};

#endif
