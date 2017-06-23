#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <fstream>
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>

using namespace std;

util utils;

void makeSheet(string inputFile, string outputFile) {
	ofstream writeFile;
	ifstream readFile;
	string buffer_global;
	string buffer4FirstLine;
	string buffer4CurrentLine;

	writeFile.open(outputFile.c_str());
	readFile.open(inputFile.c_str());

	while (!readFile.eof()) {
		getline(readFile, buffer4CurrentLine);

#pragma region Rules
		//For the first line
		bool vlanMatchStatus = buffer4CurrentLine.find("vlan") != string::npos;
		bool interfaceVlanMatchStatus = buffer4CurrentLine.find("interface Vlanif") != string::npos;
		bool sharpMatchStatus = buffer4CurrentLine.find("#") != string::npos;
		

		//First Line Exclude List

		bool descriptionMatchStatus = buffer4CurrentLine.find("description") == string::npos;
		bool vlanBatchMatchStatus = buffer4CurrentLine.find("vlan batch") == string::npos;
		bool accessVlanMatchStatus = utils.containIgnoreCase(buffer4CurrentLine, "access-vlan") == 0;
		bool aggregateVlanMatchStatus = utils.containIgnoreCase(buffer4CurrentLine, "aggregate-vlan") == 0;
		
		//For the current line
		bool trafficPolicyMatchStatus = utils.containIgnoreCase(buffer4CurrentLine, "traffic-policy tsm");
		bool ipAddressMatchStatus = utils.containIgnoreCase(buffer4CurrentLine, "ip address");

#pragma endregion

#pragma region First Line Detection

		if ((vlanMatchStatus 
			&& descriptionMatchStatus 
			&& vlanBatchMatchStatus 
			&& accessVlanMatchStatus 
			&& aggregateVlanMatchStatus) 
			
			|| interfaceVlanMatchStatus) {
			buffer4FirstLine = buffer4CurrentLine;
		}
		
		if (sharpMatchStatus && buffer4FirstLine.find("vlan") == string::npos) {
			if (buffer4FirstLine != "") {
				writeFile << buffer4FirstLine << "\n";
			}
			buffer4FirstLine = ""; continue;
		}
#pragma endregion

#pragma region Final Line Detection
		if (trafficPolicyMatchStatus || ipAddressMatchStatus) {
#pragma endregion
			//write to file
			writeFile << buffer4FirstLine
				<< "	";

#pragma region Final Rule 1
			
			if (ipAddressMatchStatus) {
				for (int i = 0; i < (buffer4CurrentLine.size()-14); ++i) { 
					writeFile << buffer4CurrentLine[i]; 
				}
			}

#pragma endregion

#pragma region Final Rule 2
			if (trafficPolicyMatchStatus) { writeFile << buffer4CurrentLine; }
#pragma endregion
				writeFile<< "\n";
			//Clear Buffer
			buffer4FirstLine = "";
			buffer4CurrentLine = "";
			continue;
		}
		//Add the current line to the global buffer;
	}
	writeFile.close();
	readFile.close();
};

int main(int argc, char*argv[]) {
	string buf4Arguments;
	string outputFileName;
	string inputFileName;
	if (argc == 1) {

	}

	if (argc > 1 && argc <= 2) {
		buf4Arguments = argv[1];
		if (buf4Arguments == "--help") {
			printf("sort-vlan-info by LBYPatrick\n");
			printf("Here are parameters available for use:\n");
			printf("    -i or --input-file  : specify origin log file.\n");
			printf("    -o or --output-file : specify output file.\n");
			return 0;
			//Some READMEs
			return 0;
		}
		printf("Oops, something went wrong...Please double-check arguments you typed.\n");
		return 0;
	}
	if (argc > 2) {
		bool gotInputCommand = 0;
		bool gotOutputCommand = 0;
		for (int i = 1; i < argc; i++) {
			if (argv[i]) {
				buf4Arguments = argv[i];
				if (buf4Arguments == "--input-file" || buf4Arguments == "-i") { gotInputCommand = 1;   inputFileName = argv[i + 1]; }
				if (buf4Arguments == "--output-file" || buf4Arguments == "-o") { gotOutputCommand = 1; outputFileName = argv[i + 1]; }
			}
		}
		if (!gotInputCommand || !gotOutputCommand) {
			printf("ERROR:No enough vaild arguments received, please try again.");
			return 0;
		}
		if (gotInputCommand && gotOutputCommand) {
			printf("Start Converting...\n");
			makeSheet(inputFileName,outputFileName);
		}
		printf("Done.\n");
		
	}

};