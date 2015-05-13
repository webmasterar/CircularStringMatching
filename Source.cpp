#include<iostream>
#include<string>
#include<vector>
#include<math.h>

using namespace std;

const char * alphabet[] = { "a", "c","g","t" }; //Alphabet
const int sigma = 4; //length of alphabet
const double c = abs(1 - (exp(1) / sqrt(sigma))); // c = 0.359
const double d = 1.1;

int min(int x, int y, int z){
	int minimum = x;

	if (minimum > y){
		minimum = y;
	}

	if (minimum > z){
		minimum = z;
	}
	return minimum;

}

int del(char character){
	return 1;
}

int ins(char character){
	return 1;
}

int sub(char pCharacter, char tCharacter){
	int value;
	if (pCharacter == tCharacter){
		value = 0;
	}
	else{
		value = 1;
	}
	return value;
}

//checked
void calculateS(char *s, int &q, int i){	//calculates string s
	int j;
	int temp = i;

	for (j = q-1; j >=0; j--){
		s[j] = *alphabet[temp%sigma];
		temp = temp / sigma;
	}
	s[q] = 0;

	//prints s
	/*for (j = 0; j < q; j++){
		cout << s[j]<< " ";
	}
	cout << endl;*/
}

//checked
void calculateU(char *pattern, char *u, int &q, int i){
	int j;
	for (j = 0; j < 2 * q; j++){
		u[j] = pattern[j + i];
	}
	u[2 * q] = 0;

	//prints u
	/*for (j = 0; j < 2 * q; j++){
		cout << u[j] << " ";
	}
	cout << endl;*/
}

//checked
int EditDistance(char *pattern, int m, char *text, int n){ //Approximate string matching algorithm using standard dynamic algorithm

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

	for (i = 1; i <= m; i++){
		D[1][0] = D[0][0] + del(pattern[i - 1]);
		for (j = 1; j <= n; j++){
			D[1][j] = min(D[0][j - 1] + sub(pattern[i - 1], text[j - 1]),
				D[0][j] + del(pattern[i - 1]),
				D[1][j - 1] + ins(text[j - 1])
				);
		}

		if (Emin > D[1][n]){
			Emin = D[1][n];
		}

		//copies second vector to the first
		if (i < m){
			for (j = 0; j <= n; j++){
				D[0][j] = D[1][j];
			}
		}
	}
	//prints matrix D
	/*
	for (i = 0; i < rows; i++){
		for (j = 0; j <= n; j++){
			cout << D[i][j]<<" ";
		}
		cout << endl;
	}
	*/
	return Emin;
}

//checked
int preprocessing(char *pattern, int &m, int q){ 
	int i,j,Emin, EminNew;
	int sigmaPowerQ = _Pow_int(sigma, q); //sigma^q
	int total = 0;

	//creates M vector and assign all its values equal to 0
	//size of M is 0..sigma^q - 1
	int *M = new int[sigmaPowerQ + 1];
	for (i = 0; i < sigmaPowerQ; i++){
		M[i] = 0;
	}
	M[sigmaPowerQ + 1] = 0;

	char *s = new char[q + 1];
	char *u = new char[2 * q + 1];

	for (i = 0; i < sigmaPowerQ; i++){
		calculateS(s, q, i);
		calculateU(pattern, u, q, 0);

		Emin = EditDistance(u, 2 * q , s, q); //sets Emin equal to the minimum edit distance between s and any prefix of u
		
		for (j = 1; j <= m - (2*q); j++){  //1..m-(2*q) times 
			calculateU(pattern, u, q, j);
			EminNew = EditDistance(u, 2 * q, s, q);
			//sets EminNew equal to the minimum edit distance between s and any prefix of u
			if (Emin>EminNew){
				Emin = EminNew;
			}
		}
		M[i] = Emin;
	}
	for (i = 0; i < sigmaPowerQ; i++){
		total = total + M[i];
	}

	return total;

	//prints matrix M
	/*for (i = 0; i < sigmaPowerQ; i++){
		cout << M[i] << " ";
	}
	cout << endl;*/
}

//checked
void EditDistance(char *pattern, int m, char *text, int n, int *outputVector){ //Approximate string matching algorithm using standard dynamic algorithm
	
	const int rows = 2;
	int i, j;

	//initialize dynamic matrix D
	int **D = new int*[rows];
	for (i = 0; i < rows; i++){
		D[i] = new int[n + 1];
	}

	for (j = 0; j <= n; j++){
		D[0][j] = 0;
	}

	for (i = 1; i <= m; i++){
		D[1][0] = D[0][0] + del(pattern[i - 1]);
		for (j = 1; j <= n; j++){
			D[1][j] = min(D[0][j - 1] + sub(pattern[i - 1], text[j - 1]),
				D[0][j] + del(pattern[i - 1]),
				D[1][j - 1] + ins(text[j - 1])
				);
		}

		//copies second vector to the first
		if (i < m){
			for (j = 0; j <= n; j++){
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
	

	for (i = 0; i <= n; i++){
		outputVector[i] = D[1][i];
	}
	outputVector[n + 1] = 0;
}

//checked
void checkVector(int *editDistanceVector, int n, int &k, int j, vector<vector<int>> & outputVector){
	int i;
	vector<int> data;

	for (i = 1; i < n ; i++){
		if (editDistanceVector[i] <= k){
			vector<int> data;
			data.push_back(j); //adds j
			data.push_back(i); //adds i
			data.push_back(editDistanceVector[i]); //adds k'
			outputVector.push_back(data); //add j,i,vector[i] to the output vector
		}
	}
	
}

//checked
void verification(char *pattern, int m, char *text, int n, int k, vector<vector<int>> & outputVector){
	
	int i;
	int *editDistanceVector = new int[n + 2];

	//cout << pattern << endl;
	EditDistance(pattern, m, text, n, editDistanceVector);
	checkVector(editDistanceVector, n + 1, k, 0, outputVector);
	rotate(pattern, pattern + 1, pattern + m); //rotates pattern by one position

	for (i = 1; i < m; i++){
		//cout << pattern << endl;
		EditDistance(pattern, m, text, n, editDistanceVector);
		checkVector(editDistanceVector, n + 1, k, i, outputVector);
		rotate(pattern, pattern + 1, pattern + m );
	}
}

//checked
void printOutputVector(vector<vector<int>> & outputVector){
	int i;
	cout << "rotation j	" << "	position i	" << "	number of mismatches	" << endl;
	for (i = 0; i < outputVector.size(); i++){
		cout << "	" << outputVector[i][0] << "		" << outputVector[i][1] << "		" << outputVector[i][2];
		cout << endl;
	}
}

//checked
void calculateWindowBackwards(char *windowBackwards, int &windowBackwardsSize, char *window, int &windowSize, int &q, int &qGramBackwards){
	int i;
	for (i = 0; i < q + qGramBackwards - 1; i++){
		windowBackwards[i] = window[windowSize - (q + qGramBackwards - 1) + i];
	}
	for (i = q + qGramBackwards - 1; i < windowBackwardsSize; i++){
		windowBackwards[i] = windowBackwards[i - ((q + qGramBackwards) - 1)];
	}
	windowBackwards[2 * (q + qGramBackwards) - 1] = 0;
}

int main(){
	string pattern;
	string text;
	int i,j,t; //counters
	int k, windowSize, shift, qGramBackwards, q, windowBackwardsSize;

	vector<vector<int>> outputVector; //it will hold information of rotation of patern, position of occurence and number of mismatches

	cout << "Give pattern:";
	getline(cin, pattern);
	cout << endl;
	cout << "Give text:";
	getline(cin, text);
	cout << endl;
	cout <<"Give number of errors:";
	cin >> k;
	cout << endl;

	int m = pattern.length();
	int n = text.length();

	windowSize = m - k;

	q = ceil(((3 * (log(m) / log(sigma))) + (log(k) / log(sigma))) / d);
	qGramBackwards = ceil(1 + (k / (c*q)));

	windowBackwardsSize = 2 * ((q + qGramBackwards) - 1) - 1;

	cout << q << " " << qGramBackwards << endl;
	cout << ceil(m - k - (q + k / c)) << endl;

	char *dynPattern = new char[m + 1];
	char *window = new char[windowSize + 1];
	char *windowBackwards = new char[2 * (q + qGramBackwards)]; //q + qGramBackwards - 1
	//int *editDistanceVector = new int[n + 2];
	
	//store pattern in array
	for (i = 0; i < m; i++){
		dynPattern[i] = pattern[i];
	}
	dynPattern[m] = 0;
	
	j = 0;
	while ((j + windowSize) <= n){

		for (i = 0; i < windowSize; i++){
			window[i] = text[j+i];
		}
		j = j + windowSize;

		calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, windowSize, q, qGramBackwards);
		cout << preprocessing(windowBackwards, windowBackwardsSize, q) << endl;

		if (preprocessing(windowBackwards, windowBackwardsSize, q) > k){
			shift = ceil(m - k - (q + k / c));
		}
		else{
			verification(dynPattern, m, window, windowSize, k, outputVector); 
			shift = m - k;
		}

		//print window
		for (t = 0; t < windowSize; t++){
			cout << window[t];
		}
		cout << endl;

		for (t = 0; t < windowBackwardsSize; t++){
			cout << windowBackwards[t];
		}
		cout << endl;

		j = (j + shift) - windowSize;
	}

	//final window in case it does not reach the end
	if (j < n){
		for (i = 0; i < windowSize; i++){
			window[i] = text[n - windowSize + i];
		}
		calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, windowSize, q, qGramBackwards);
		
		cout << preprocessing(windowBackwards, windowBackwardsSize, q) << endl;
		if (preprocessing(windowBackwards, windowBackwardsSize, q) <= k){
			verification(dynPattern, m, window, windowSize, k, outputVector);
		}
		//print window
		for (t = 0; t < windowSize; t++){
			cout << window[t];
		}
		cout << endl;

		for (t = 0; t < windowBackwardsSize; t++){
			cout << windowBackwards[t];
		}
		cout << endl;
	}
	
	cin.get();
	cin.get();
	return 0;
}