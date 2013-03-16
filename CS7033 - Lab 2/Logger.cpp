#include "Logger.h"

Logger::RunningFunctionsMap Logger::initRunningFunctionsMap(){
	RunningFunctionsMap m;
	return m;
}

Logger::FunctionRunTimesMap Logger::initFunctionRunTimesMap(){
	FunctionRunTimesMap m;
	return m;
}

Logger::RunningFunctionsMap Logger::runningFunctions(Logger::initRunningFunctionsMap());
Logger::FunctionRunTimesMap Logger::functionRunTimes(Logger::initFunctionRunTimesMap());

void Logger::initLogger(){
	srand(glutGet(GLUT_ELAPSED_TIME));
	std::cout << "Logger starting..." << std::endl;
}

int Logger::enterFunction(char* className, char* funcName){
	pthread_mutex_lock(&loggerMutex);
	unsigned int runningFunctionID = rand();
	while(runningFunctions.find(runningFunctionID) != runningFunctions.end())
		runningFunctionID = rand();
	
	LogRunningFunction functionDetails;
	functionDetails.className = className;
	functionDetails.funcName = funcName;
	functionDetails.startTime = glutGet(GLUT_ELAPSED_TIME);

	runningFunctions[runningFunctionID] = functionDetails;
	pthread_mutex_unlock(&loggerMutex);

	return runningFunctionID;
}

void Logger::exitFunction(int logID){
	unsigned int endTime = glutGet(GLUT_ELAPSED_TIME);

	pthread_mutex_unlock(&loggerMutex);
	if(runningFunctions.find(logID) == runningFunctions.end())
		return;

	LogRunningFunction funcDetails = runningFunctions[logID];
	runningFunctions.erase(runningFunctions.find(logID));

	unsigned int runTime = endTime - funcDetails.startTime;

	std::ostringstream ss;
	ss << funcDetails.className << "::" << funcDetails.funcName;
	std::string str(ss.str());

	if(functionRunTimes.find(str) == functionRunTimes.end()){
		LogFunctionDetails d;
		d.className = funcDetails.className;
		d.funcName = funcDetails.funcName;
		d.calls = 1;
		d.totalRunTime = runTime;
		functionRunTimes[str] = d;
	}else{
		++functionRunTimes[str].calls;
		functionRunTimes[str].totalRunTime += runTime;
	}

	pthread_mutex_unlock(&loggerMutex);
}

void Logger::printRunTimes(){
	pthread_mutex_lock(&loggerMutex);
	
	FunctionRunTimesMap::iterator itr = functionRunTimes.begin();
	while(itr != functionRunTimes.end()){
		std::cout << "[" << itr->first << "]" << itr->second.calls << " calls, " << itr->second.totalRunTime << "ms" << std::endl;
		itr++;
	}

	pthread_mutex_unlock(&loggerMutex);
}

void Logger::resetRunTimes(){
	pthread_mutex_lock(&loggerMutex);
	functionRunTimes.clear();
	pthread_mutex_unlock(&loggerMutex);
}

std::vector<std::string> Logger::runTimesVector(){
	pthread_mutex_lock(&loggerMutex);
	std::vector<std::string> resultVector;
	FunctionRunTimesMap::iterator itr = functionRunTimes.begin();
	
	while(itr != functionRunTimes.end()){
		std::ostringstream ss;
		ss << "[" << itr->first << "] " << itr->second.calls << " calls, " << itr->second.totalRunTime << "ms";
		resultVector.push_back(ss.str());
		itr++;
	}
	pthread_mutex_unlock(&loggerMutex);

	return resultVector;
}