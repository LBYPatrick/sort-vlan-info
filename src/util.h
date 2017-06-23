#pragma once
#include <string>
#include <vector>


class util {
public:
	std::string getVendor();
    unsigned int getMemoryMB();
	bool containIgnoreCase(std::string str, std::string key);
    bool equalIgnoreCase(std::string str, std::string key);
	std::string toUpperString(std::string str);
	int scriptInit();
    int writeToScript(std::string command, bool output = false);
    int writePSScript(std::string command);
    int runAndDelete();
    void restart();
    void makeRestorePoint();
    unsigned int getDiskFree();
	void visualPercentage(unsigned int percent = 0, std::string message = "");
	int sysExecute(std::string command);
    void killEXE(std::string processName);
    std::vector<std::string> getDrives();
	std::string getCurrentTime();
	bool isUserAdmin();
	std::string getWinVersion();
};
