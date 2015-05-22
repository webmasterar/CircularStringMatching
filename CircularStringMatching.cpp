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

void CircularStringMatching::calculateS(char *s, int i) {
    int j, temp = i;

    for (j = this->q - 1; j >= 0; j--) {
	    s[j] = this->alphabet[temp % this->sigma];
	    temp = temp / this->sigma;
    }
    s[this->q] = 0;
}

void CircularStringMatching::calculateU(char *pattern, char *u, int i){
    int j;
    for (j = 0; j < 2 * this->q; j++) {
	    u[j] = pattern[j + i];
    }
    u[2 * this->q] = 0;
}

int CircularStringMatching::EditDistance(char *pattern, int m, char *text, int n) {

	const int rows = 2;
	int i, j;
	int Emin = 100;

	//initialize dynamic matrix D
	int **D = new int*[rows];
	for (i = 0; i < rows; i++){
		D[i] = new int[n + 1];
	}
	for (j = 0; j <= n; j++){
		D[0][j] = 0;
	}

    //Dynamic Programming
	for (i = 1; i <= m; i++) {
		D[1][0] = D[0][0] + PENALTY_DEL;
		for (j = 1; j <= n; j++) {
			D[1][j] = min(D[0][j - 1] + this->delta(pattern[i - 1], text[j - 1]),
                min(
				    D[0][j] + PENALTY_DEL,
				    D[1][j - 1] + PENALTY_INS
				));
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

void CircularStringMatching::EditDistance(char *pattern, int m, char *text, int n, int *outputVector) {
	
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
			D[1][j] = min(D[0][j - 1] + this->delta(pattern[i - 1], text[j - 1]),
                min(
				    D[0][j] + PENALTY_DEL,
				    D[1][j - 1] + PENALTY_INS
				));
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
		cout<<endl;
	}
	cout << endl;
	

	for (i = 0; i <= n; i++) {
		outputVector[i] = D[1][i];
	}
	outputVector[n + 1] = 0;

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
	//size of M is 0..sigma^q - 1 //@todo double check this
    unsigned int * M;
    if ((M = (unsigned int *) calloc(sigmaPowerQ + 1, sizeof(unsigned int))) == NULL) {
        cerr << "Error: Could not assign M for preprocessing" << endl;
        return EXIT_FAILURE;
    }

	char *s = new char[q + 1];
	char *u = new char[2 * q + 1];

	for (i = 0; i < sigmaPowerQ; i++) {
		this->calculateS(s, i);
		this->calculateU(pattern, u, 0);

		Emin = this->EditDistance(u, 2 * this->q , s, this->q); //sets Emin equal to the minimum edit distance between s and any prefix of u
		
		for (j = 1; j <= windowSize - (2 * this->q); j++) {  //1..m-(2*q) times 
			this->calculateU(pattern, u, j);
			EminNew = this->EditDistance(u, 2 * this->q, s, this->q);
			//sets EminNew equal to the minimum edit distance between s and any prefix of u
			if (EminNew < Emin) {
				Emin = EminNew;
			}
		}
		M[i] = Emin;
	}

	int total = 0;
	for (i = 0; i < sigmaPowerQ; i++){
		total = total + M[i];
	}

    delete[] u;
    delete[] s;
    free(M);

	return total;
}

void CircularStringMatching::checkVector(int *editDistanceVector, int j, vector<vector<int>> &outputVector) {
	int i;
	for (i = 1; i < this->n + 1; i++){
		if (editDistanceVector[i] <= this->k){
			vector<int> data;
			data.push_back(j); //adds j
			data.push_back(i); //adds i
			data.push_back(editDistanceVector[i]); //adds k'
			outputVector.push_back(data); //add j,i,vector[i] to the output vector
		}
	}
}

void CircularStringMatching::verification(char *pattern, int m, char *text, int n, vector<vector<int>> &outputVector) {
	
	int i;
	int *editDistanceVector = new int[n + 2];

	EditDistance(pattern, m, text, n, editDistanceVector);
	checkVector(editDistanceVector, 0, outputVector);
	rotate(pattern, pattern + 1, pattern + m); //rotates pattern by one position //@todo find out where rotate is

	for (i = 1; i < m; i++) {
		EditDistance(pattern, m, text, n, editDistanceVector);
		checkVector(editDistanceVector, i, outputVector);
		rotate(pattern, pattern + 1, pattern + m);
	}

    delete[] editDistanceVector;
}

void CircularStringMatching::printOutputVector(vector<vector<int>> &outputVector) {
	int i;
	cout << "rotation j	" << "	position i	" << "	number of mismatches	" << endl;
	for (i = 0; i < outputVector.size(); i++) {
		cout << "	" << outputVector[i][0] << "		" << outputVector[i][1] << "		" << outputVector[i][2] << endl;
	}
}

void CircularStringMatching::calculateWindowBackwards(char *windowBackwards, int &windowBackwardsSize, char *window, int &windowSize, int &qGramBackwards) {
	int i;
	for (i = 0; i < this->q + qGramBackwards - 1; i++) {
		windowBackwards[i] = window[windowSize - (this->q + qGramBackwards - 1) + i];
	}
	for (i = this->q + qGramBackwards - 1; i < windowBackwardsSize; i++) {
		windowBackwards[i] = windowBackwards[i - ((this->q + qGramBackwards) - 1)];
	}
	windowBackwards[2 * (this->q + qGramBackwards) - 1] = 0;
}

void CircularStringMatching::run() {

}

