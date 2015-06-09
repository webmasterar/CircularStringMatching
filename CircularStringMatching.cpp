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

unsigned int CircularStringMatching::rotate(char * x, int offset, char * rotation)
{
    unsigned int m = strlen ( ( char * ) x );
    memmove ( &rotation[0], &x[offset], m - offset );
    memmove ( &rotation[m - offset], &x[0], offset );
    rotation[m] = '\0';
    return 1;
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
    u[2 * this->q] = 0;
}

int CircularStringMatching::EditDistance(char *pattern, int m, char *text, int n)
{
    const int rows = 2;
    int i, j;
    int Emin = 100;

    //initialize dynamic matrix D
    int **D = new int*[rows];
    for (i = 0; i < rows; i++) {
	D[i] = new int[n + 1];
    }
    for (j = 0; j <= n; j++) {
	D[0][j] = 0;
    }

    //Dynamic Programming
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

	if (D[1][n] < Emin){
	    Emin = D[1][n];
	}

	//copies second vector to the first
	if (i < m) {
	    for (j = 0; j <= n; j++) {
		D[0][j] = D[1][j];
	    }
	}
    }

    //free memory
    for (i = 0; i < rows; i++) {
        delete[] D[i];
    }
    delete[] D;

    return Emin;
}

void CircularStringMatching::EditDistance(char *pattern, int m, char *text, int n, int *outputVector)
{
    const int rows = 2;
    int i, j;

    //initialize dynamic matrix D
    int **D = new int*[rows];
    for (i = 0; i < rows; i++) {
	D[i] = new int[n + 1];
    }
    for (j = 0; j <= n; j++) {
	D[0][j] = 0;
    }

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
    for (i = 0; i < rows; i++){
	for (j = 0; j <= n; j++){
	    cout << D[i][j] << " ";
	}
	cout << endl;
    }
    cout << endl;

    for (i = 0; i <= n; i++) {
	outputVector[i] = D[1][i];
    }
    //outputVector[n + 1] = 0;

    //free memory
    for (i = 0; i < rows; i++) {
        delete[] D[i];
    }
    delete[] D;
}

int CircularStringMatching::preprocessing(char *pattern, int windowSize)
{
    int i, j, Emin, EminNew;
    int sigmaPowerQ = (int) pow((double)this->sigma, (double)this->q); //sigma^q

    //creates M vector and initialize all its values to 0
    if ((this->M = (unsigned int *) calloc(sigmaPowerQ, sizeof(unsigned int))) == NULL) {
        cerr << "Error: Could not assign M for preprocessing" << endl;
        return EXIT_FAILURE;
    }

    char *s = new char[q + 1];
    char *u = new char[2 * q + 1];

    for (i = 0; i < sigmaPowerQ; i++) {
	this->calculateS(s, i);
	this->calculateU(pattern, u, 0);

	Emin = this->EditDistance(u, 2 * this->q , s, this->q); //sets Emin equal to the minimum edit distance between s and any prefix of u
	
	for (j = 1; j <= windowSize - (2 * this->q); j++) { //1..m-(2*q) times 
	    this->calculateU(pattern, u, j);
	    EminNew = this->EditDistance(u, 2 * this->q, s, this->q);
	    //sets EminNew equal to the minimum edit distance between s and any prefix of u
	    if (EminNew < Emin) {
		Emin = EminNew;
	    }
	}
	this->M[i] = Emin;
    }

    delete[] u;
    delete[] s;

    return EXIT_SUCCESS;
}

void CircularStringMatching::checkVector(int *editDistanceVector, int n, int j, vector<vector<int>> &outputVector)
{
    /*
     * Not sure what n should be @todo fix it
     */
    int i;
    for (i = 1; i < n + 1; i++) {
	if (editDistanceVector[i] <= this->k) {
	    vector<int> data;
	    data.push_back(j); //adds j
	    data.push_back(i); //adds i
	    data.push_back(editDistanceVector[i]); //adds k'
	    outputVector.push_back(data); //add j,i,vector[i] to the output vector
	}
    }
}

void CircularStringMatching::verification(char *pattern, int m, char * window2m, int n, vector<vector<int>> &outputVector)
{
    int i;
    int * editDistanceVector = new int[n + 2];

    this->EditDistance(pattern, m, window2m, n, editDistanceVector);
    this->checkVector(editDistanceVector, n, 0, outputVector);
    this->rotate(pattern, 1, pattern + m); //rotates pattern by one position //pattern + 

    for (i = 1; i < m; i++) {
	this->EditDistance(pattern, m, window2m, n, editDistanceVector);
	this->checkVector(editDistanceVector, n, i, outputVector);
	this->rotate(pattern, 1, pattern + m);
    }

    delete[] editDistanceVector;
}

void CircularStringMatching::printOutputVector(vector<vector<int>> &outputVector)
{
    int i;
    cout << "rotation j	" << "	position i	" << "	number of mismatches	" << endl;
    for (i = 0; i < outputVector.size(); i++) {
	cout << "	" << outputVector[i][0] << "		" << outputVector[i][1] << "		" << outputVector[i][2] << endl;
    }
}

/*void CircularStringMatching::calculateWindowBackwards(char *windowBackwards, int &windowBackwardsSize, char *window, int windowSize, int &qGramBackwards)
{
    int i;
    for (i = 0; i < this->q + qGramBackwards - 1; i++) {
	windowBackwards[i] = window[windowSize - (this->q + qGramBackwards - 1) + i];
    }
    for (i = this->q + qGramBackwards - 1; i < windowBackwardsSize; i++) {
	windowBackwards[i] = windowBackwards[i - ((this->q + qGramBackwards) - 1)];
    }
    windowBackwards[2 * (this->q + qGramBackwards) - 1] = 0;
}*/

int CircularStringMatching::run()
{
    int i, j, k;
    int shift, qGramBackwards, windowBackwardsSize;

    //holds rotation of pattern, position of occurence and number of mismatches
    vector<vector<int>> outputVector;

    qGramBackwards = ceil(1 + (this->k / (this->c * this->q)));

    windowBackwardsSize = 2 * ((this->q + qGramBackwards) - 1) - 1; //VII

    cout << this->q << " " << qGramBackwards << endl;
    cout << ceil(this->m - this->k - (this->q + this->k / this->c)) << endl;

    char * window = new char[this->verifiedWindowShift + 1];
    char * windowBackwards = new char[2 * (this->q + qGramBackwards)];
    
    /*
     * 
     * Step 1: Preprocess the pattern to fill out the M array
     * 
     */
    this->preprocessing(windowBackwards, windowBackwardsSize);
    
    
    /*
     * 
     * Step 2: loop through the text in a moving window
     * 
     */
    int qpos, qIndex, total, pos = this->verifiedWindowShift;
    while (pos < this->n) {

	//figure out if we need to verify
	total = 0;
	for (qpos = pos - this->q; qpos > pos - qGramBackwards && qpos >= 0; qpos--) {
	    qIndex = this->getQIndex( (char *) this->text.substr(qpos, this->q).c_str());
	    total = total + this->M[qIndex];
	    if (total > this->k) {
		break;
	    }
	}
	
	//verify
	if (total <= this->k) {
	    int window2mStart = pos - ((int)this->m - (int)this->k);
	    cout << "pos: " << pos << " start: " << window2mStart << endl;
	    string w = this->text.substr(window2mStart, 2 * this->m);
	    this->verification((char *) this->pattern.c_str(), this->m, (char *) w.c_str(), w.length(), outputVector);
	}

	//move window
	if (total > this->k) {
	    pos = pos + this->unverifiedWindowShift;
	} else {
	    pos = pos + this->verifiedWindowShift;
	}
    }
    
    this->printOutputVector(outputVector);
    
    /*
    j = 0;
    while ((j + this->verifiedWindowShift) <= this->n) {
	for (i = 0; i < this->verifiedWindowShift; i++) {
	    window[i] = this->text[j+i];
	}
	j = j + this->verifiedWindowShift;
	
	this->calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, (int)this->verifiedWindowShift, qGramBackwards);

	if ( > this->k) {
	    shift = this->unverifiedWindowShift;
	} else {
	    this->verification((char *) this->pattern.c_str(), this->m, window, this->verifiedWindowShift, outputVector); 
	    shift = this->verifiedWindowShift;
	}

	//print window
	for (k = 0; k < this->verifiedWindowShift; k++) {
	    cout << window[k];
	}
	cout << endl;

	for (k = 0; k < windowBackwardsSize; k++) {
	    cout << windowBackwards[k];
	}
	cout << endl;

	j = (j + shift) - this->verifiedWindowShift;
    }
    */
    //final window in case it does not reach the end
    /*if (j < this->n) {
	for (i = 0; i < this->verifiedWindowShift; i++) {
	    window[i] = this->text[this->n - this->verifiedWindowShift + i];
	}
	this->calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, (int)this->verifiedWindowShift, qGramBackwards);

	cout << this->preprocessing(windowBackwards, windowBackwardsSize) << endl;
	if (this->preprocessing(windowBackwards, windowBackwardsSize) <= this->k) {
	    this->verification(dynPattern, this->m, window, this->verifiedWindowShift, outputVector);
	}
	//print window
	for (k = 0; k < this->verifiedWindowShift; k++) {
	    cout << window[k];
	}
	cout << endl;

	for (k = 0; k < windowBackwardsSize; k++) {
	    cout << windowBackwards[k];
	}
	cout << endl;
    }*/

    //clear memory
    delete[] window;
    delete[] windowBackwards;

    return EXIT_SUCCESS;
}

CircularStringMatching::CircularStringMatching(string pattern, unsigned int m, string text, unsigned int n, unsigned int k)
{
    this->alphabet[(int)'a'] = 0u;
    this->alphabet[(int)'c'] = 1u;
    this->alphabet[(int)'g'] = 2u;
    this->alphabet[(int)'t'] = 3u;
    this->antiAlphabet[0] = 'a';
    this->antiAlphabet[1] = 'c';
    this->antiAlphabet[2] = 'g';
    this->antiAlphabet[3] = 't';
    
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
