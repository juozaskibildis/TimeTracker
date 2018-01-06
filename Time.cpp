#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>
#include <string>
#include <stdio.h>

using namespace std;

// TODO: folder
// TODO: consider month
// TODO: make help clearer
// TODO: create session command


// Global Variables
//-------------------------
string task = "";
string folder = "track/";
string logFileName = "log.txt"; 		// possible issue when task and journal filenames are the same
int remainingMinutes = 0; 				// remaining minutes after conversion to hours

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

int getMinutes 	 					// calculates difference in minutes
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
		if(hourStop < hourStart) 			// if past midnight
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
			sum += 24*60; 				// 24 hours in a day 60 minutes in an hour
		}
	}


	if(yearStart != yearStop)
	{
		for(int i = 0; i < yearStop - yearStart; i++)
		{
			sum += 365*24*60;
		}
	}

	return sum;	
}

int getHours(int minutes) 					// returns amount of hours in a given number of minutes
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

void help()
{
	// TODO: make help clearer
	cout << endl;
	cout << "Usage:" << endl;
	cout << "track [task]" << endl;
	cout << ">> This command starts tracking or displays status" << endl;
	cout << endl;
	cout << "track [task] stop" << endl;
	cout << ">> This command stops tracking and logs results" << endl;
	cout << endl;
}

void printTaskInfo()
{
	ifstream ifile(task);

	int year;
	int month;
	int day;
	int hour;
	int minute;

	// get contents
	ifile >> year >> day >> hour >> minute;
	ifile.close();

	printTime(year, day, hour, minute);
	printTime();

	cout/* << setw(2) << setfill('0')*/ << getHours(getMinutes(getTime(), year, day, hour, minute)) << " hours "
	<</* setw(2) << setfill('0') <<*/ remainingMinutes << " minutes spent" << endl;
}

// Files
//-------------------------

bool fileExists() 					// task file
{
	ifstream ifile(task.c_str());
	if(!ifile) 					
	{
		return false;
	}
	ifile.close();
	return true;
}

int getNumberOfLines() 					// in a log file
{
	ifstream ifile(logFileName.c_str());
	int lines = 0;
	string tmp;
	
	// there should be three variables per line
	while(ifile >> tmp >> tmp >> tmp) 		// this possibly loops every three lines
	{
		lines++;
	}
	ifile.close();
	return lines;
}

void append(int minutes)
{
	int lines = getNumberOfLines();

	string taskRe[lines]; 				// used for rewriting
	int hourRe[lines];
	int minuteRe[lines];

	// read contents
	ifstream readLog(logFileName.c_str());

	for(int i = 0; i < lines; i++)
	{
		readLog >> taskRe[i] >> hourRe[i] >> minuteRe[i];	
	}

	readLog.close();
	
	// write contents
	ofstream writeLog(logFileName.c_str());

	for(int i = 0; i < lines; i++)
	{
		writeLog << taskRe[i] << " " << hourRe[i] << " " << minuteRe[i] << '\n';
	}

	writeLog << task << " " << getHours(minutes) << " " << remainingMinutes << '\n';
	writeLog << endl;
	
	writeLog.close();
}

void log()
{
	// consider making list of every task run and total time spent on them
	// consider printing date every day, possible issue if task is ongoing after midnight
	// consider making command to add a separator

	int minutes;

	ifstream ifile(task);

	int year;
	int month;
	int day;
	int hour;
	int minute;

	ifile >> year >> day >> hour >> minute;
	ifile.close();

	minutes = getMinutes(getTime(), year, day, hour, minute);

	append(minutes);
}

void removeFile()
{
	if(remove(task.c_str()) != 0 )
	{
    		cout << "Couldn`t remove task file" << endl;
	}	
}

void createTaskFile()
{
	time_t currentTime = time(0);
	struct tm * timeStruct = localtime(&currentTime);

	ofstream ofile(task.c_str());
	ofile << timeStruct->tm_year + 1900 << " "
	<< timeStruct->tm_yday << " "
	<< timeStruct->tm_hour << " "
	<< timeStruct->tm_min << " "
	<< endl;
}

// Commands
//-------------------------

// consider making a new command to separate days or sessions

void start()
{
	if(fileExists() == true)
	{
		cout << "Ongoing" << endl;
		printTaskInfo();
	}
	else
	{
		cout << "Started" << endl;
		createTaskFile();
		printTime();
	}
}

void stop()
{
	if(fileExists() == true)
	{
		cout << "Task finished" << endl;
		printTaskInfo();

		log();
		removeFile();
	}
	else
	{
		cout << "This task wasn`t started" << endl;
	}
}


// Main
//-------------------------

int main
	(
 	int argc, 				// argc argv
       	char* argv[]
	)

{
	string command = "";

	switch(argc)
	{
		case(1): 				// if there is a single argument
		{
			help();
			break;
		}
		case(2):
		{
			task = argv[1]; 		// set the first argument as a task
			start();
			break;
		}
		default:
		{
			task = argv[1];
			command = argv[2]; 		// set the second argument as a command	
			if(command == "stop")
			{
				stop();
			}
			break;
		}
	}
	return 0;
}
