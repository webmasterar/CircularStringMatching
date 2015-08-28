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
#include <fstream>
#include "CircularStringMatching.h"

int main ( int argc, char** argv )
{
    char * info = (char *) "The valid options for Circular String Matching (csm) are:\n"
			   "-t\tRequired. The text to search through\n"
			   "-f\tOptional. File for -t if -t won't do\n"
			   "-p\tRequired. The pattern you are searching for\n"
			   "-l\tOptional. File for -p if -p won't do\n"
			   "-a\tRequired. The sequence type (DNA or PROT)\n"
			   "-k\tOptional. Maximum error, e.g. 2. Default := 0\n"
			   "-h\tOptional. This help message\n";
    string text;
    string pattern;
    char a = '\0';
    unsigned int k = 0;

    unsigned int i;
    for ( i = 1; i < argc; i++ ) {
        if ( '-' == (int)argv[i][0] ) {
            switch ( (int)argv[i][1] ) {
                case 't':
		    text = string(argv[i + 1]);
                    break;

                case 'f':
		    {
			ifstream ft (argv[i + 1], ios::in);
			if (ft) {
			    ft.seekg(0, ios::end);
			    text.resize(ft.tellg());
			    ft.seekg(0, ios::beg);
			    ft.read(&text[0], text.size());
			    ft.close();
			} else {
			    fprintf( stderr, "Error opening file passed via -f\n" );
			    return EXIT_FAILURE;
			}
		    }
                    break;

                case 'p':
		    pattern = string(argv[i + 1]);
                    break;

		case 'a':
		    if (strcmp("DNA", argv[i + 1]) == 0)
		    {
			a = ALPHABET_DNA;			
		    }
		    else if (strcmp("PROT", argv[i + 1]) == 0)
		    {
			a = ALPHABET_PROT;
		    }
		    else if (strcmp("IUPAC", argv[i + 1]) == 0)
		    {
			a = ALPHABET_IUPAC;
		    }
		    break;

                case 'l':
		    {
			ifstream fp (argv[i + 1], ios::in);
			if (fp) {
			    fp.seekg(0, ios::end);
			    pattern.resize(fp.tellg());
			    fp.seekg(0, ios::beg);
			    fp.read(&pattern[0], pattern.size());
			    fp.close();
			} else {
			    fprintf( stderr, "Error opening file passed via -l\n" );
			    return EXIT_FAILURE;
			}
		    }
                    break;

                case 'k':
                    k = (unsigned int) atoi( argv[i + 1] );
                    break;

                case 'h':
                    printf ( "%s", info );
                    return EXIT_SUCCESS;
                    break;

                default:
                    fprintf( stderr, "Invalid option '-%c' supplied!\n", argv[i][1] );
                    printf ( "%s", info );
                    return ( EXIT_FAILURE );
            }
        }
    }

    unsigned int n = text.size();
    unsigned int m = pattern.size();

    if (n == 0 || m == 0 || a == '\0')
    {
        fprintf( stderr, "Command line options missing/invalid!\n" );
        printf ( "%s", info );
        return ( EXIT_FAILURE );
    }
    else
    {
	CircularStringMatching csm(&pattern, m, &text, n, k, a);
	int run = csm.run();

	if (run == EXIT_FAILURE) {
	    fprintf ( stderr, "Circular String Matching process failed... exiting.\n" );
	    return EXIT_FAILURE;
	}
    }

    return EXIT_SUCCESS;
}
