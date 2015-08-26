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

void CircularStringMatching::initAlphabet()
{
    this->alphabet = (unsigned int *) calloc(127, sizeof(unsigned int));
    this->antiAlphabet = (char *) calloc(21, sizeof(char));

    char alphabet[22];
    alphabet[21] = '\0';

    if (this->a == ALPHABET_DNA)
    {
	strcpy(alphabet, DNA);
	alphabet[5] = '\0';
	this->sigma = 5u;
    }
    else if (this->a == ALPHABET_IUPAC)
    {
	strcpy(alphabet, IUPAC);
	alphabet[16] = '\0';
	this->sigma = 16u;
    }
    else
    {
	strcpy(alphabet, PROT);
	alphabet[21] = '\0';
	this->sigma = 21u;
    }

    unsigned int i;
    for (i = 0; i < strlen(alphabet); i++) {
	this->alphabet[(int)alphabet[i]] = i;
	this->alphabet[(int)tolower(alphabet[i])] = i;
	this->antiAlphabet[i] = alphabet[i];
    }
}

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
    int i;
    unsigned int value = 0;
    for (i = this->q - 1; i >= 0; i--) {
          value = value + ((pow(sigma,i)) * this->alphabet[(int)qgram[this->q - i - 1]]);
    }
    return value;
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
	Emin = this->EditDistance(u, this->q, s, this->q); //sets Emin equal to the minimum edit distance between s and any prefix of u //2 * q

	for (j = 1; j < this->m; j++) { //1..2m-2q times 
	    this->calculateU(patternDoubled, u, j);
	    EminNew = this->EditDistance(u, this->q, s, this->q); //sets EminNew equal to the minimum edit distance between s and any prefix of u //2 * q
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
    int i, j, k;
    int Emin = INT_MAX;

    int * D0 = new int[m + 1];
    D0[0] = 0;
    for (i = 1; i < m + 1; i++) {D0[i] = D0[i - 1] + PENALTY_DEL;}
    int * D1 = new int[m + 1];

    for (i = 1; i < n + 1; i++) {
	for (j = 0; j < m + 1; j++) {
	    switch (i % 2) {
	      case 1:
		  if (j == 0)
		  {
		      D1[0] = D0[0] + PENALTY_INS;
		  }
		  else
		  {
		      D1[j] = k = min(
			  D0[j - 1] + this->delta(pattern[j - 1], qgram[i - 1]),
			  min(D0[j] + PENALTY_DEL, D1[j - 1] + PENALTY_INS)
		      );
		  }
		  break;

	      case 0:
		  if (j == 0)
		  {
		      D0[0] = D1[0] + PENALTY_INS;
		  }
		  else
		  {
		      D0[j] = k = min(
			  D1[j - 1] + this->delta(pattern[j - 1], qgram[i - 1]),
			  min(D1[j] + PENALTY_DEL, D0[j - 1] + PENALTY_INS)
		      );
		  }
		  break;
	    }

	    if (i == n && k < Emin) {
		Emin = k;
	    }
	}
    }

    delete[] D0;
    delete[] D1;

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
    int i, j, k;

    int * D0 = new int[n + 1];
    for (j = 0; j <= n; j++) {D0[j] = 0;}
    int * D1 = new int[n + 1];

    for (i = 1; i <= m; i++) {
	for (j = 0; j <= n; j++) {
	    k = i % 2;
	    switch (k) {
	      case 1:
		  if (j == 0)
		  {
		      D1[0] = D0[0] + PENALTY_INS;
		  }
		  else
		  {
		      D1[j] = min(
			  D0[j - 1] + this->delta(pattern[i - 1], text[j - 1]),
			  min(D0[j] + PENALTY_DEL, D1[j - 1] + PENALTY_INS)
		      );
		  }
		  break;

	      case 0:
		  if (j == 0)
		  {
		      D0[0] = D1[0] + PENALTY_INS;
		  }
		  else
		  {
		      D0[j] = min(
			  D1[j - 1] + this->delta(pattern[i - 1], text[j - 1]),
			  min(D1[j] + PENALTY_DEL, D0[j - 1] + PENALTY_INS)
		      );
		  }
		  break;
	    }
	}
    }

    for (j = 0; j <= n; j++) {
	outputVector[j] = (k == 1) ? D1[j] : D0[j];
    }
    
    delete[] D0;
    delete[] D1;
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

    if (outputVector.size() >= 1) {
	cout << "Best Match: " << endl;
	cout << "position i" << "\trotation j" << "\tnum mismatches" << endl;
	for (i = 0; i < outputVector.size(); i++) {
	    if (outputVector[i][2] < Emin) {
		j = i;
		Emin = outputVector[i][2];
	    }
	}
	cout << outputVector[j][0] << "           \t" << outputVector[j][1] << "          \t" << outputVector[j][2] << endl;
    } else {
	cout << "<<<No matches found>>>" << endl;
    }
}

int CircularStringMatching::run()
{
    //holds rotation of pattern, position of occurence and number of mismatches
    vector<vector<int>> outputVector;
    clock_t preprocessingTime, runningTime; //initializes variables which will hold time
    runningTime = clock(); //sets it equal to clock 
    int numberVerifications = 0;
    bool boolVerify = false;
          
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
    cout << "Building q-gram index..." << endl;
    preprocessingTime = clock(); //sets it equal to the clock
    this->preprocessing((char *) xx.c_str());
    preprocessingTime = clock() - preprocessingTime; //sets it equal to the differnce of intial clock value and current clock value
    cout << "Preprocessing time: " << (((float) preprocessingTime) / CLOCKS_PER_SEC) << " seconds" << endl; //converts into a float and outputs time in seconds
          
    /*
     * 
     * Step 2: loop through the text in a moving window
     * 
     */
    cout << "Starting searching process..." << endl;
    int qpos, qIndex, total, pos = this->verifiedWindowShift;
    while (pos < this->n) {

	//figure out if we need to verify
	unsigned int i;
	total = 0;
	for (qpos = pos - this->q, i = 0; qpos >= 0 && i < this->qGramBackwards; qpos--, i++) { //VII
	    qIndex = this->getQIndex( (char *) this->text.substr(qpos, this->q).c_str());
	    if ((this->M[qIndex]) < (this->c * this->q)){
		boolVerify = true;
		break;
	    }
	    total = total + this->M[qIndex];
	}
	
	if ((boolVerify == false) && (total <= this->k)){
	    boolVerify = true;
	}

	//verify
        if (boolVerify){
	    int window2mStart = pos - ((int)this->m - (int)this->k);
	    string w = this->text.substr(window2mStart, 2 * this->m);
	    this->verification((char *) this->pattern.c_str(), this->m, (char *) w.c_str(), w.length(), outputVector, window2mStart);
	    numberVerifications += 1;
	}

	if (pos == this->n -1) {
	    break;
	}

        if (boolVerify){
	    pos = pos + this->verifiedWindowShift;
	} else {
	    pos = pos + this->unverifiedWindowShift;
	}
	
	
	if (pos >= this->n)
	{
	    pos = this->n - 1;
	}

	boolVerify = false;
    }

    this->printOutputVector(outputVector);    
    cout << "Number of Verifications: " << numberVerifications << endl;
    runningTime = clock() - runningTime;

    cout << "Searching/Verification time: " << (((float) (runningTime - preprocessingTime)) / CLOCKS_PER_SEC) << " seconds" << endl;
    cout << "Total running time: " << (((float) runningTime) / CLOCKS_PER_SEC) << " seconds" << endl;

    return EXIT_SUCCESS;
}

CircularStringMatching::CircularStringMatching(string pattern, unsigned int m, string text, unsigned int n, unsigned int k, char a)
{
    this->a = a;
    this->initAlphabet();

    this->pattern = pattern;
    this->m = m;
    cout << "m: " << this->m << endl;

    this->text = text;
    this->n = n;
    cout << "n: " << this->n << endl;

    this->k = k;
    cout << "k: " << this->k << endl;

    //this->c = abs(1 - (exp(1) / sqrt(this->sigma))); //@todo check: lemma 4 "The probability decreases exponentionally for d > 1, which holds if c < 1 - e/sqrt(sigma)
    //c value dependent on sigma
    if (this->a == ALPHABET_DNA) {
	this->c = 0.1111;
    } else {
	this->c = 0.2498;
    }
    cout << "c: " << c << endl;

    //1 - this->c + (2 * this->c * (log(this->c) / log(this->sigma))) + (2 * (1 - this->c) * (log(1 - this->c) / log(this->sigma))); //lemma 4
    //d value somewhat dependent on sigma
    if (this->a == ALPHABET_DNA) {
	this->d = 1.0;
    } else {
	this->d = 1.1;
    }
    cout << "d: " << d << endl;

    //q-gram size restricted otherwise preprocessing would take exponentionally longer and use up too much memory
    double logK = (this->k == 0) ? 0 : log(this->k); //can't log k=0
    this->q = min(9u, (unsigned int) ceil((3 * (log(this->m) / log(this->sigma)) + (logK / log(this->sigma))) / this->d )); //IX
    cout << "q: " << q << endl;

    this->qGramBackwards = ceil(1 + (this->k / (this->c * this->q))); //VII
    //this->qGramBackwards = (unsigned int) ((0.6 * this->m) + (this->k / (this->c * this->q))); //was 0.5
    cout << "qb: " << this->qGramBackwards << endl;

    this->verifiedWindowShift = this->m - this->k; //VIII
    cout << "vw: " << this->verifiedWindowShift << endl;

    this->unverifiedWindowShift = (int) (this->verifiedWindowShift - (this->q + (this->k / this->c))); //VIII
    //this->unverifiedWindowShift = (int) (this->verifiedWindowShift - (this->qGramBackwards + this->q - 1));
    cout << "uvw: " << this->unverifiedWindowShift << endl;
}
