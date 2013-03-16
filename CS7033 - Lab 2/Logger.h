#pragma once

#include <iostream>
#include <random>
#include <time.h>
#include <pthread.h>
#include <GL\glut.h>
#include <map>
#include <iterator>
#include <sstream>

#define LOGGER_ENTER(className, funcName) unsigned int  __loggerLogID = Logger::enterFunction(className, funcName)
#define LOGGER_EXIT Logger::exitFunction(__loggerLogID)

struct LogRunningFunction{
		char* className;
		char* funcName;
		unsigned int startTime;
};

struct LogFunctionDetails{
	char* className;
	char* funcName;
	unsigned int calls;
	unsigned int totalRunTime;
};

static pthread_mutex_t loggerMutex = PTHREAD_MUTEX_INITIALIZER;

class Logger{
private:
	typedef std::map<unsigned int, LogRunningFunction> RunningFunctionsMap;
	typedef std::map<std::string,LogFunctionDetails> FunctionRunTimesMap;
	static RunningFunctionsMap runningFunctions;
	static FunctionRunTimesMap functionRunTimes;

	static RunningFunctionsMap initRunningFunctionsMap();
	static FunctionRunTimesMap initFunctionRunTimesMap();
public:
	static void initLogger();
	static int enterFunction(char* className, char* funcName);
	static void exitFunction(int logID);
	static void printRunTimes();
	static void resetRunTimes();
	static std::vector<std::string> runTimesVector();
};