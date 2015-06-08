#include <iostream>
#include <fstream>	// I/O files
#include <time.h>	// clock library that includes clock_t, clock(), CLOCKS_PER_SEC
#include <string>
#include <stdlib.h>
using namespace std;


int main(){
	
	int i,j;//counters
	string text,pattern;
	clock_t t; //initializes t  which will hold the time
	
	
	ifstream textFile("text.txt"); //input file
	ifstream patternFile("pattern.txt"); //input file
	ofstream runningTimesFile; //output file
	ofstream patternLengthFile; //output file
	
	if(textFile.is_open()){ //checks if file is open
		getline (textFile, text); //gets first line of the file and sets it equal to the string text
	}
	textFile.close(); //close file
	
	if(patternFile.is_open()){ //check if file is open
		getline (patternFile, pattern); //gets first line of the file and sets it equal to the string text
	}
	patternFile.close(); //close file
	
	int m = pattern.length();
	int n = text.length();
	
	runningTimesFile.open("runningTimes.txt");  //open a file
	patternLengthFile.open("patternLengthFile.txt"); //open file
	char cmd[2 * n + 25];
	cmd[2 * n + 25 - 1] = '\0';
	
	do{
		cout<<m<<endl;
		
		sprintf(cmd, "./../../csm -t %s -p %s -k %d", text.c_str(), pattern.c_str(), 1);
		
		t=clock(); //sets t equal to the clock

		system(cmd);
		
		t=clock() - t; //sets t equal to the differnce of intial clock value and current clock value 
	
		runningTimesFile << (((float)t)/CLOCKS_PER_SEC)<<"\n";
		patternLengthFile << m << "\n";
		cout<<"Running time: "<<(((float)t)/CLOCKS_PER_SEC)<< " seconds"<<endl; //converts t into a float and outputs it
		
		pattern=pattern+pattern;
		m=pattern.length();
		
		
	}while(m<n);
	
	runningTimesFile.close(); //close file
	patternLengthFile.close(); //close file
	
	return 0;
}