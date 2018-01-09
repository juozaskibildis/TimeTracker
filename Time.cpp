#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string>
#include <stdio.h>

using namespace std;

// Global Variables
//-------------------------

string task = "";
string taskFile = "";
string logFileName = "log.txt"; 	 			
int remainingMinutes = 0; 					// remaining minutes after conversion to hours

//-------------------------
void start();
void stop();
void printTaskInfo();

// Time
//-------------------------

time_t getTime()
{
	time_t currentTime = time(0);
	return currentTime;
}

void printTime(int year, int day, int hour, int minute)
{
	cout << year << " day " << day << " "
	<< setw(2) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << endl;
}

void printTime()
{
	time_t currentTime = getTime();
	struct tm * timeStruct = localtime(&currentTime);
	printTime
		(
	 	timeStruct->tm_year + 1900,
		timeStruct->tm_yday,
		timeStruct->tm_hour,
		timeStruct->tm_min
		);
}

// Calculations
//-------------------------

int getMinutes 	 						// calculates difference in minutes
	( 	
 	time_t time,
	int yearStart,
	int dayStart,
	int hourStart,
	int minuteStart
	)
{
	int sum = 0;

	struct tm * timeStruct = localtime(&time);

	int yearStop = timeStruct->tm_year + 1900;
	int dayStop = timeStruct->tm_yday;
	int hourStop = timeStruct->tm_hour;
	int minuteStop = timeStruct->tm_min;

	if(minuteStop < minuteStart) 				// avoid negatives
	{
		minuteStop += 60;
		hourStop--;
	}
	for(int i = 0; i < minuteStop - minuteStart; i++)
	{
		sum++;
	}

	if(hourStop != hourStart)
	{
		if(hourStop < hourStart) 
		{
			hourStop += 24;
			dayStop--;
		}
		for(int i = 0; i < hourStop - hourStart; i++) 	// convert hours into minutes
		{
			sum += 60;
		}
	}

	if(dayStop != dayStart)
	{
		if(dayStop < dayStart)
		{
			dayStop += 365; 			// not accurate since there are leap years
			yearStop--;
		}
		for(int i = 0; i < dayStop - dayStart; i++)
		{
			sum += 24 * 60; 			// 24 hours in a day 60 minutes in an hour
		}
	}


	if(yearStart != yearStop)
	{
		for(int i = 0; i < yearStop - yearStart; i++)
		{
			sum += 365 * 24 * 60;
		}
	}

	return sum;	
}

int getHours
	(
	int minutes
 	) 							// returns amount of hours in a given number of minutes
{
	int hours = 0;

	while(minutes >= 60)
	{
		minutes -= 60;
		hours++;
	}
	remainingMinutes = minutes; 				// returns remaining minutes into global variable	
	return hours;
}


// Text
//-------------------------

void help(string programName)
{
	cout << "Usage:" << endl;
	cout << programName << " [task]" << endl; 
	cout << "Starts tracking." << endl;
	cout << "Run again to stop." << endl;
	cout << "If given more arguments prints info instead of stopping." << endl;
}

// Files
//-------------------------

bool fileExists() 						// task file
{
	ifstream ifile(taskFile.c_str());
	if(!ifile) 					
	{
		return false;
	}
	ifile.close();
	return true;
}

int getNumberOfLines() 						// in a log file
{
	ifstream ifile(logFileName.c_str());
	int lines = 0;
	string tmp;
	
	while(ifile >> tmp >> tmp >> tmp) 			// there should be three variables per line 
	{
		lines++;
	}
	ifile.close();
	return lines;
}

void append
	(
 	int minutes
 	)
{
	int lines = getNumberOfLines();

	string taskRe[lines]; 					// used for rewriting
	int hourRe[lines];
	int minuteRe[lines];

	ifstream readLog(logFileName.c_str()); 

	for(int i = 0; i < lines; i++) 				// read file content
	{
		readLog >> taskRe[i] >> hourRe[i] >> minuteRe[i];	
	}

	readLog.close();
	
	ofstream writeLog(logFileName.c_str());

	for(int i = 0; i < lines; i++) 				// rewrite file content
	{
		writeLog << taskRe[i] << " " << hourRe[i] << " " << minuteRe[i] << '\n';
	}

	writeLog << task << " " << getHours(minutes) << " " << remainingMinutes << '\n';
	writeLog << endl; 					// append current task info
	
	writeLog.close();
}

void log()
{
	int minutes; 						// time spent in minutes

	ifstream ifile(taskFile.c_str());

	int year;
	int month;
	int day;
	int hour;
	int minute; 						// for reading file content

	ifile >> year >> day >> hour >> minute;
	ifile.close();

	minutes = getMinutes(getTime(), year, day, hour, minute);

	append(minutes);
}

void removeFile()
{
	if(remove(taskFile.c_str()) != 0 )
	{
    		cout << "Couldn`t remove task file." << endl;
	}	
}

void createTaskFile()
{
	time_t currentTime = time(0);
	struct tm * timeStruct = localtime(&currentTime);

	ofstream ofile(taskFile.c_str());
	ofile << timeStruct->tm_year + 1900 << " "
	<< timeStruct->tm_yday << " "
	<< timeStruct->tm_hour << " "
	<< timeStruct->tm_min << " "
	<< endl;
}

// Commands
//-------------------------

void start()
{
	if(fileExists() == true)
	{
		stop();
	}
	else
	{
		cout << "Started." << endl;
		createTaskFile();
		printTime();
	}
}

void printTaskInfo()
{
	if(fileExists())
	{
		ifstream ifile(taskFile.c_str());

		int year;
		int month;
		int day;
		int hour;
		int minute;

		ifile >> year >> day >> hour >> minute; 		// get content
		ifile.close();

		printTime(year, day, hour, minute);
		printTime();

		cout << getHours(getMinutes(getTime(), year, day, hour, minute)) << " hours "
		<< remainingMinutes << " minutes spent." << endl;
	}
	else
	{
		start();
	}
}

void stop()
{
	cout << "Finished." << endl;
	printTaskInfo();
	log();
	removeFile();
}

// Main
//-------------------------

int main
	(
 	int argc, 
       	char* argv[]
	)

{
	string command = "";

	switch(argc)
	{
		case(1): 
		{
			help(argv[0]);
			break;
		}
		case(2):
		{
			task = argv[1]; 			// set the first argument as a task
			taskFile = task + ".txt";
			if(task != "log")
			{
				start();
			}
			else
			{
				cout << "Use different name to not overwrite the log file." << endl;
			}
			break;
		}
		default:
		{
			task = argv[1];
			taskFile = task + ".txt";	
			printTaskInfo();
			break;
		}
	}
	return 0;
}
