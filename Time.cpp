#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>
#include <string>

using namespace std;

// TODO: solve segmentation fault which is caused in main
// TODO: consider folder

// Global Variables
//-------------------------
string task;
string logFileName = "log"; 		// possible issue when task and journal filenames are the same

// Time
//-------------------------

time_t getTime()
{
	time_t currentTime = time(0);
	return currentTime;
}

void printTime(int year, int day, int hour, int minute)
{
	cout << year << " day " << day << " " << hour << ":" << minute << endl;
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

int getMinutes 					// calculates difference in minutes
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
		for(int i = 0; i < hourStart - hourStop; i++) 	// convert hours into minutes
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

// Text
//-------------------------

void help()
{
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
	ifile >> year >> month >> day >> hour >> minute;
	ifile.close();

	// TODO
	// print them
	// printTime(year, month, day, hour, minute);
	// printTime();
	// 
	// time_t currentTime = getTime();
	// struct tm * timeStruct = localtime(&currentTime);
	//
	//
	// cout << getHours(getTime(), year, month, day, hour) << " hours " << "" << " minutes spent" << endl;
	// consider doing all the calculations only with minutes and then converting them into hours
	//
}

// Files
//-------------------------

bool fileExists() 					// task file
{
	ifstream ifile(task);
	if(!ifile) 					// not sure about this
	{
		return false;
	}
	ifile.close();
	return true;
}

int getNumberOfLines() 					// in log file
{
	ifstream ifile(logFileName);
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

void log
 	(
	int year,
	int day,
	int hour,
	int minute
	)
{
	ofstream ofile(logFileName);
	int lines = getNumberOfLines();
	int minutes;

	ofile.seekp(0, ios_base::end);  		// set cursor at the end of the file
	ofile << endl;	

	minutes = getMinutes(getTime(), year, day, hour, minute);

	// ofile << task << " " << getHours() << " " << minutes << endl;
	// there is a possibility of too many hours 
	
	ofile.close();
}

void createTaskFile()
{
	time_t currentTime = time(0);
	struct tm * timeStruct = localtime(&currentTime);

	ofstream ofile(task);
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
		// log();
		// delete file
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
	task = ""; 					// initialize variables
	string command = "";

	switch(argc)
	{
		case(1): 				// if there is a single argument
		{
			help();
		}
		case(2):
		{
			task = argv[1]; 		// set the first argument as a task
			start();
		}
		default:
		{
			task = argv[1];
			command = argv[2]; 		// set the second argument as a command	
			if(command == "stop")
			{
				stop();
			}
		}
	}
	return 0;
}
