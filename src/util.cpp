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

#define DEBUG 1

using std::string;
using std::endl;

std::ofstream writer;
std::ifstream fileReader;

void util::restart(){
#if DEBUG
    system("echo There's a reboot & pause");
#else
    system("shutdown -r -t 0");
	system("exit");
#endif
}
 
int util::scriptInit() {
    writer.open("execute.bat", std::fstream::app | std::fstream::out);
    writer << "@echo off" << endl;
    return 0;
}

int util::writeToScript(string command, bool output) {
    writer << command << (output ? "" : " > nul") << endl;
    return 0;
}

int util::sysExecute(string command) {
	string executeCommand = command + " > nul";
	system(executeCommand.c_str());
	return 0;
}
int util::writePSScript(string command) {
    writer << "powershell -command "<< command << endl;
    return 0;
}

int util::runAndDelete() {
    writer.close();
    writer.clear();
    system("execute.bat");
    system("del execute.bat");
    return 0;
}

std::string util::getVendor() {
    char buf[128];
    string result;
    std::shared_ptr<FILE> pipe(_popen("wmic csproduct get vendor", "r"), _pclose);
    if (!pipe) throw std::runtime_error("unable to get computer information");
    while (fgetc(pipe.get()) != '\n'); // skip the first line
    fgets(buf, 128, pipe.get());
    result = buf;
    return result.substr(0, result.find('\n'));
 }

unsigned int util::getMemoryMB() {
    long long read, sum = 0;
    std::shared_ptr<FILE> pipe(_popen("wmic MEMORYCHIP get Capacity", "r"), _pclose);
    if (!pipe) throw std::runtime_error("unable to get computer information");
    while (fgetc(pipe.get()) != '\n'); // skip the first line
    while (fscanf(pipe.get(), "%llu", &read)!= EOF) sum += read;
    return (uint32_t)(sum >> 20);
}

std::string util::getWinVersion() {
    char buf[256] = "\n";
    std::shared_ptr<FILE> pipe(_popen("ver", "r"), _pclose);
    if (!pipe) throw std::runtime_error("unable to get computer information");
    while (buf[0] == '\n') fgets(buf, 256, pipe.get());
    unsigned short major, minor, build;
    sscanf(strrchr(buf, ' ') + 1, "%hu.%hu.%hu", &major, &minor, &build);
    std::stringstream fmt;
    if (major == 10) fmt << "Windows 10";
    else if (major == 6)
        switch (minor){
            case 0:
                fmt << "Windows Vista";
                break;
            case 1:
                fmt << "Windows 7";
            case 2:
                fmt << "Windows 8";
            case 3:
                fmt << "Windows 8.1";
            default:
                fmt << "Weird OS";
        }
    else if (major == 5) fmt << "Windows XP/2000";
    else fmt << "Old and weird OS";
    fmt << " Build " << build;
    printf("%s\n", fmt.str().c_str());
    return fmt.str();
}

bool util::containIgnoreCase(string str, string key){
    return toUpperString(str).find(toUpperString(key)) != string::npos;
}

bool util::equalIgnoreCase(string str, string key){
    return !toUpperString(str).compare(toUpperString(key));
}

string util::toUpperString(string str) {
    for (std::string::iterator it = str.begin(); it != str.end(); ++ it)
        *it = (char) toupper(*it);
    return str;
}

void util::makeRestorePoint(){
    util::scriptInit();
    writeToScript("Wmic.exe /Namespace:\\\\root\\default Path SystemRestore Call CreateRestorePoint \"LSHelper%DATE%\", 100, 1");
    util::runAndDelete();
}

unsigned int util::getDiskFree() {
    char buf[128];
    long long byte_free;
    std::shared_ptr<FILE> pipe(_popen("fsutil volume diskfree %SystemRoot%", "r"), _pclose);
    if (!pipe) throw std::runtime_error("unable to get computer information");
    fgets(buf, 128, pipe.get());
    sscanf(strstr(buf, ": ") + 2,"%llu", &byte_free);
    return (uint32_t)(byte_free >> 20);
}

void util::killEXE(std::string processName){
    char buffer[128];
    sprintf(buffer,"taskkill /f /im %s > nul",processName.c_str());
    system(buffer);
    }

std::vector<std::string> util::getDrives(){
    std::vector<std::string> drives;
    char read[10];
    std::shared_ptr<FILE> pipe(_popen("wmic logicaldisk get caption\n", "r"), _pclose);
    if (!pipe) throw std::runtime_error("unable to get computer information");
    while (fgetc(pipe.get()) != '\n'); // skip the first line
    while (fgets(read, 10, pipe.get())!= NULL) drives.push_back(string(read));
    return drives;
}

string util::getCurrentTime() {

	string result;

	system("echo %date% %time% >> time.info");
	fileReader.open("time.info");
	getline(fileReader, result);
	fileReader.close();
	system("del time.info");
	return result;
}

bool util::isUserAdmin() {
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	b = AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);
	if (b) {
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &b)) { b = false; }
		FreeSid(AdministratorsGroup);
	}
	return (bool)b;
}

void util::visualPercentage(unsigned int percent, std::string message) {
    system("cls");
	printf("%s\n[",message.c_str());

    for (unsigned int i = 0; i < (percent/2); i++) {
        printf(">");
    } for (unsigned int i = 0; i < ((100 - percent)/2); i++) {
        printf(" ");
    }

	printf("]%d%s", percent,R"(%)");
    if (percent == 100) {
        Sleep(300);
		printf("\n\n\n");
    }
}
