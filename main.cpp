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
    char * t = (char *) "";
    char * p = (char *) "";
    char a = '\0';
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
			size_t read_size = 0;
                        fseek ( ft, 0, SEEK_END );
                        unsigned long length = ftell ( ft );
                        fseek ( ft, 0, SEEK_SET );
                        t = (char *) malloc ( length + 1 );
                        if ( t ) {
                            read_size = fread ( t, 1, length, ft );
                        }
                        fclose ( ft );
			t[length] = '\0';
                    }
                    break;

                case 'p':
                    p = argv[i + 1];
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
                    if ( ( fp = fopen ( argv[i + 1], "r" ) ) == NULL ) {
                        fprintf( stderr, "Input file l could not be opened!\n");
                        return ( EXIT_FAILURE );
                    } else {
			size_t read_size = 0;
                        fseek ( fp, 0, SEEK_END );
                        unsigned long length = ftell ( fp );
                        fseek ( fp, 0, SEEK_SET );
                        p = (char *) malloc ( length + 1 );
                        if ( p ) {
                            read_size = fread ( p, 1, length, fp );
                        }
                        fclose ( fp );
			p[length] = '\0';
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

    unsigned int n = strlen(t);
    unsigned int m = strlen(p);

    if (n == 0 || m == 0 || a == '\0')
    {
        fprintf( stderr, "Command line options missing/invalid!\n" );
        printf ( "%s", info );
        return ( EXIT_FAILURE );
    }
    else
    {
	string pattern = string(p);
	string text = string(t);

	CircularStringMatching csm(pattern, m, text, n, k, a);
	int run = csm.run();

	if (run == EXIT_FAILURE) {
	    fprintf ( stderr, "Circular String Matching process failed... exiting.\n" );
	    free ( t );
	    return EXIT_FAILURE;
	}
    }

    return EXIT_SUCCESS;
}
