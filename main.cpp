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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "CircularStringMatching.h"

int main ( int argc, char** argv ) {

    unsigned char* info = "The valid options for Circular String Matching (csm) are:\n"
                          "-t\tRequired. The text to search through\n"
                          "-f\tOptional. File for -t if -t won't do\n"
                          "-p\tRequired. The pattern you are searching for\n"
                          "-l\tOptional. File for -p if -p won't do\n"
                          "-k\tOptional. Maximum error, e.g. 2. Default := 0\n";
    unsigned char * t = "";
    unsigned char * p = "";
    unsigned int k = 0;
    FILE * ft, * fp;
    
    unsigned int i;
    for ( i = 1; i < argc; i++ ) {
        if ( '-' == (int)argv[i][0] ) {
            switch ( (int)argv[i][1] ) {
                case 't':
                    t = argv[i + 1];
                    break;
                    
                case 'f':
                    if ( ( ft = fopen ( argv[i + 1], "r" ) ) == NULL ) {
                        fprintf( stderr, "Input file f could not be opened!\n");
                        return ( EXIT_FAILURE );
                    } else {
                        fseek ( ft, 0, SEEK_END );
                        unsigned long length = ftell ( ft );
                        fseek ( ft, 0, SEEK_SET );
                        t = malloc ( length );
                        if ( t ) {
                            fread ( t, 1, length, ft );
                        }
                        close ( ft );
                    }
                    break;
                    
                case 'p':
                    p = argv[i + 1];
                    break;
                    
                case 'l':
                    if ( ( fp = fopen ( argv[i + 1], "r" ) ) == NULL ) {
                        fprintf( stderr, "Input file l could not be opened!\n");
                        return ( EXIT_FAILURE );
                    } else {
                        fseek ( fp, 0, SEEK_END );
                        unsigned long length = ftell ( fp );
                        fseek ( fp, 0, SEEK_SET );
                        p = malloc ( length );
                        if ( p ) {
                            fread ( p, 1, length, fp );
                        }
                        close ( fp );
                    }
                    break;
                    
                case 'k':
                    k = (unsigned int) atoi( argv[i + 1] );
                    break;
                    
                default:
                    fprintf( stderr, "Invalid option '-%c' supplied!\n", argv[i][1] );
                    printf ( "%s", info );
                    return ( EXIT_FAILURE );
            }
        }
    }
    
    unsigned int n = strlen ( t );
    unsigned int m = strlen ( p );
    
    if ( n == 0 || m == 0 ) {
        fprintf( stderr, "Command line options missing!\n");
        printf ( "%s", info );
        return ( EXIT_FAILURE );
    }
    
	string pattern = string(p);
	string text = string(t);

    //initialize the object
    CircularStringMatching csm(pattern, m, text, n, k, 0);

    /*
     *
     * The code below here would go into the run() method
     * @todo copy it accross and finish up
     *
     */

	int j,l; //counters
	int windowSize, shift, qGramBackwards, q, windowBackwardsSize;

	vector<vector<int>> outputVector; //it will hold information of rotation of patern, position of occurence and number of mismatches

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

		calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, windowSize, qGramBackwards);
		cout << preprocessing(windowBackwards, windowBackwardsSize) << endl;

		if (preprocessing(windowBackwards, windowBackwardsSize) > k){
			shift = ceil(m - k - (q + k / c));
		}
		else{
			verification(dynPattern, m, window, windowSize, outputVector); 
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
		calculateWindowBackwards(windowBackwards, windowBackwardsSize, window, windowSize, qGramBackwards);
		
		cout << preprocessing(windowBackwards, windowBackwardsSize, q) << endl;
		if (preprocessing(windowBackwards, windowBackwardsSize, q) <= k){
			verification(dynPattern, m, window, windowSize, outputVector);
		}
		//print window
		for (l = 0; l < windowSize; l++){
			cout << window[l];
		}
		cout << endl;

		for (l = 0; l < windowBackwardsSize; l++){
			cout << windowBackwards[l];
		}
		cout << endl;
	}

	return EXIT_SUCCESS;
}

