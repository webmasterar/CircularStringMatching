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

#include "CircularStringMatching.h"

void CircularStringMatching::calculateS(char *s, int i)
{
    int j, temp = i;
    for (j = this->q - 1; j >= 0; j--) {
	s[j] = this->antiAlphabet[temp % this->sigma];
	temp = temp / this->sigma;
    }
    s[this->q] = '\0';
}

void CircularStringMatching::rotate(char * x, int offset, char * rotation)
{
    unsigned int m = strlen ( x );
    memmove ( &rotation[0], &x[offset], m - offset );
    memmove ( &rotation[m - offset], &x[0], offset );
    rotation[m] = '\0';
}

unsigned int CircularStringMatching::getQIndex(char * qgram)
{
    unsigned int i = 0, len = strlen(qgram);
    unsigned int word = 0;
    
    while (i < len) {
	word = word << 2;
	word = word | this->alphabet[(int)qgram[i]];
	i++;
    }
    
    return word;
}

void CircularStringMatching::calculateU(char *pattern, char *u, int i)
{
    int j;
    for (j = 0; j < 2 * this->q; j++) {
	u[j] = pattern[j + i];
    }
    u[2 * this->q] = '\0';
}

int CircularStringMatching::preprocessing(char *patternDoubled)
{
    int i, j, Emin, EminNew;
    int sigmaPowerQ = (int) pow((double)this->sigma, (double)this->q); //sigma^q

    //creates M vector and initialize all its values to 0
    if ((this->M = (unsigned int *) malloc(sigmaPowerQ * sizeof(unsigned int))) == NULL) {
        cerr << "Error: Could not assign M for preprocessing" << endl;
        return EXIT_FAILURE;
    }
    memset(this->M, this->k + 1, sigmaPowerQ * sizeof(unsigned int));

    char *s = new char[this->q + 1];
    char *u = new char[2 * this->q + 1];

    for (i = 0; i < sigmaPowerQ; i++) {
	this->calculateS(s, i);
	this->calculateU(patternDoubled, u, 0);

	Emin = this->EditDistance(u, 2 * this->q, s, this->q); //sets Emin equal to the minimum edit distance between s and any prefix of u

	for (j = 1; j < 2 * this->m - 2 * this->q; j++) { //1..2m-2q times 
	    this->calculateU(patternDoubled, u, j);
	    EminNew = this->EditDistance(u, 2 * this->q, s, this->q);
	    //sets EminNew equal to the minimum edit distance between s and any prefix of u
	    if (EminNew < Emin) {
		Emin = EminNew;
	    }
	}
	
	if (Emin < this->M[i]) {
	    this->M[i] = Emin;
	}
    }

    delete[] u;
    delete[] s;

    return EXIT_SUCCESS;
}

int CircularStringMatching::EditDistance(char *pattern, int m, char *qgram, int n)
{
    int i, j;
    int Emin = INT_MAX;
    
    int ** D = new int*[2];
    D[0] = new int[m + 1];
    D[1] = new int[m + 1];
    D[0][i] = 0;
    for (i = 1; i < m + 1; i++){D[0][i] = D[0][i - 1] + PENALTY_DEL;}
    
    for (i = 1; i < n + 1; i++) {
	D[1][0] = D[0][0] + PENALTY_INS;
	for (j = 1; j < m + 1; j++) {
	    D[1][j] = min(
		D[0][j - 1] + this->delta(pattern[j - 1], qgram[i - 1]), 
		min(
		    D[0][j] + PENALTY_DEL,
		    D[1][j - 1] + PENALTY_INS
		)
	    );
	    if (i == n && D[1][j] < Emin) {
		Emin = D[1][j];
	    }
	}
	
	if (i < n) {
	    for (j = 0; j < m + 1; j++) {
		D[0][j] = D[1][j];
	    }
	}
    }
    
    /*cout << qgram << " Row 0: ";
    for (i = 0; i < m + 1; i++) {
	cout << D[0][i] << " ";
    }
    cout << " Row 1: ";
    for (i = 0; i < m + 1; i++) {
	cout << D[1][i] << " ";
    }
    cout << Emin << endl;*/
    
    delete[] D[1];
    delete[] D[0];
    delete[] D;

    return Emin;
}

void CircularStringMatching::checkVector(int *editDistanceVector, int n, int rotation, vector<vector<int>> &outputVector, int window2mStart)
{
    int i;
    for (i = 1; i < n + 1; i++) {
	if (editDistanceVector[i] <= this->k) {
	    vector<int> data;
	    data.push_back(window2mStart + i - 1); //end position of x in t
	    data.push_back(rotation); //adds rotation
	    data.push_back(editDistanceVector[i]); //adds k'
	    outputVector.push_back(data); //adds rotation, end position of x in t,vector[i] to the output vector
	}
    }
}

void CircularStringMatching::EditDistance(char *pattern, int m, char *text, int n, int *outputVector)
{
    int i, j;

    //initialize dynamic matrix D
    int **D = new int*[2];
    D[0] = new int[n + 1];
    D[1] = new int[n + 1];
    for (j = 0; j <= n; j++){D[0][j] = 0;}

    for (i = 1; i <= m; i++) {
	D[1][0] = D[0][0] + PENALTY_DEL;
	for (j = 1; j <= n; j++) {
	    D[1][j] = min(
		D[0][j - 1] + this->delta(pattern[i - 1], text[j - 1]),
		min(
		    D[0][j] + PENALTY_DEL,
		    D[1][j - 1] + PENALTY_INS
		)
	    );
	}

	//copies second vector to the first
	if (i < m) {
	    for (j = 0; j <= n; j++) {
		D[0][j] = D[1][j];
	    }
	}
    }

    //Print Matrix D
    /*for (i = 0; i < 2; i++){
	for (j = 0; j <= n; j++){
	    cout << D[i][j] << " ";
	}
	cout << endl;
    }
    cout << endl;*/

    for (i = 0; i <= n; i++) {
	outputVector[i] = D[1][i];
    }

    //free memory
    delete[] D[1];
    delete[] D[0];
    delete[] D;
}

void CircularStringMatching::verification(char *pattern, int m, char * window2m, int n, vector<vector<int>> &outputVector, int window2mStart)
{
    int rotation;
    int * editDistanceVector = new int[n + 2];
    char tempPattern[m + 1];

    this->EditDistance(pattern, m, window2m, n, editDistanceVector);
    this->checkVector(editDistanceVector, n, 0, outputVector, window2mStart);
    this->rotate(pattern, 1, tempPattern); //rotates pattern by one position
    memcpy(pattern, tempPattern, m + 1 * sizeof(char));

    for (rotation = 1; rotation < m; rotation++) {
	this->EditDistance(pattern, m, window2m, n, editDistanceVector);
	this->checkVector(editDistanceVector, n, rotation, outputVector, window2mStart);
	this->rotate(pattern, 1, tempPattern);
	memcpy(pattern, tempPattern, m + 1 * sizeof(char));
    }

    delete[] editDistanceVector;
}

void CircularStringMatching::printOutputVector(vector<vector<int>> &outputVector)
{
    int i, j, Emin = INT_MAX;

    cout << "Best Match: " << endl;
    cout << "position i" << "\trotation j" << "\tnum mismatches" << endl;
    for (i = 0; i < outputVector.size(); i++) {
	//cout << "	" << outputVector[i][0] << "		" << outputVector[i][1] << "		" << outputVector[i][2] << endl;
	if (outputVector[i][2] < Emin) {
	    j = i;
	    Emin = outputVector[i][2];
	}
    }

    cout << outputVector[j][0] << "           \t" << outputVector[j][1] << "          \t" << outputVector[j][2] << endl;
}

int CircularStringMatching::run()
{  
    int i, j, k, shift;

    //holds rotation of pattern, position of occurence and number of mismatches
    vector<vector<int>> outputVector;
    
    if (this->unverifiedWindowShift < 1) {
	cerr << "The pattern you are using is too short" << endl;
	return EXIT_FAILURE;
    }

    string xx = this->pattern + this->pattern;
    
    /*
     * 
     * Step 1: Preprocess the pattern to fill out the M array
     * 
     */
    this->preprocessing((char *) xx.c_str());
    
    /*
     * 
     * Step 2: loop through the text in a moving window
     * 
     */
    int qpos, qIndex, total, pos = this->verifiedWindowShift;
    while (pos < this->n) {

	//figure out if we need to verify
	total = 0;
	for (qpos = pos - this->q, i = 0; qpos >= 0 && i < this->qGramBackwards; qpos--, i++) { //VII
	    qIndex = this->getQIndex( (char *) this->text.substr(qpos, this->q).c_str());
	    total = total + this->M[qIndex];
	    if (total > this->k) {
		break;
	    }
	}
	
	//cout << "total: " << total << endl;
	
	//verify
	if (total <= this->k) {
	    int window2mStart = pos - ((int)this->m - (int)this->k);
	    //cout << "pos: " << pos - 1 << " start: " << window2mStart << endl;
	    string w = this->text.substr(window2mStart, 2 * this->m);
	    this->verification((char *) this->pattern.c_str(), this->m, (char *) w.c_str(), w.length(), outputVector, window2mStart);
	}

	//move window
	if (total > this->k) {
	    pos = pos + this->unverifiedWindowShift;
	} else {
	    pos = pos + this->verifiedWindowShift;
	}
    }
    
    this->printOutputVector(outputVector);

    return EXIT_SUCCESS;
}

CircularStringMatching::CircularStringMatching(string pattern, unsigned int m, string text, unsigned int n, unsigned int k)
{
    if ((int)text.at(0) > 96) {
	this->alphabet[(int)'a'] = 0u;
	this->alphabet[(int)'c'] = 1u;
	this->alphabet[(int)'g'] = 2u;
	this->alphabet[(int)'t'] = 3u;
	this->antiAlphabet[0] = 'a';
	this->antiAlphabet[1] = 'c';
	this->antiAlphabet[2] = 'g';
	this->antiAlphabet[3] = 't';
    } else {
	this->alphabet[(int)'A'] = 0u;
	this->alphabet[(int)'C'] = 1u;
	this->alphabet[(int)'G'] = 2u;
	this->alphabet[(int)'T'] = 3u;
	this->antiAlphabet[0] = 'A';
	this->antiAlphabet[1] = 'C';
	this->antiAlphabet[2] = 'G';
	this->antiAlphabet[3] = 'T';
    }
    
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
    this->q = min(6u, (unsigned int) ceil( ((log(this->m) / log(this->sigma)) + (logK / log(this->sigma))) / this->d )); //IX - decided to replace (3 * (log(this->m) / log(this->sigma))) with 1 * .
    //this->q = 3u;
    cout << "q: " << q << endl;
    
    this->qGramBackwards = ceil(1 + (this->k / (this->c * this->q))); //VII
    cout << "qb: " << this->qGramBackwards << endl;

    this->verifiedWindowShift = this->m - this->k; //VIII
    cout << "vw: " << this->verifiedWindowShift << endl;
    this->unverifiedWindowShift = (int) (this->verifiedWindowShift - (this->q + (this->k / this->c))); //VIII
    cout << "uvw: " << this->unverifiedWindowShift << endl;
}
