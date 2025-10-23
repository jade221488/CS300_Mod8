#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <limits>
#include <cctype>

using namespace std;

struct Course{

	string courseName;
	string courseNum;
	vector<string> prereq;
};

//helps trim line down removing uneccesary whitespace from the data recieved
static inline string trimLine(const string& line) {

	size_t beginning = 0;
	size_t end = line.size();

	//start from beggining searching for whitespace and removing
	while (beginning < end && isspace((unsigned char)line[beginning])) {

		beginning++;
	}

	//start from the end searching for whitesoace and removing
	while (end > beginning && isspace((unsigned char)line[end - 1])) {

		end--;
	}

	//return trimmed line without any whitespace 
	return line.substr(beginning, end - beginning);
}

//calls trimLine to remove any whitespace and then changes course number to uppercase
static inline string checkCaseSensitve(const string& line) {

	//trim line first
	string data = trimLine(line);

	//check for any lower case and turn uppercase
	transform(data.begin(), data.end(), data.begin(), [](unsigned char c) {

		return toupper(c);
		});

	return data;
}

//find the course if exists and output all prerequisites if exists
void findCourse(unordered_map<string, Course> &courseTable, string courseNum) {

	//case-sensitive check
	string key = checkCaseSensitve(courseNum);

	//find course in course table
	auto courseIterator = courseTable.find(key);

	//if found output the course number and name
	if (courseIterator != courseTable.end()) {

		//output course number and name
		const Course& course = courseIterator->second;
		cout << course.courseNum << ", " << course.courseName << endl;

		//if there are no prerequisites
		if (course.prereq.empty()) {

			cout << "Prerequisites: None" << endl;
		}

		//output existing prerequisites
		else {

			cout << "Prerequisites: ";

			bool first = true; 
			for (auto const& prereq : course.prereq) {

				string prereqKey = checkCaseSensitve(prereq);
				auto prereqIterator = courseTable.find(prereqKey);

				//output commas if not first prereq
				if (!first) {

					cout << ", ";
				}

				//output prereq number and name if not end in course list
				if (prereqIterator != courseTable.end()) {

					cout << prereqIterator->second.courseNum << " (" << prereqIterator->second.courseName << ")";
				}

				//if there is no prereq
				else {

					cout << prereq << "(Unknown)";
				}

				first = false;
			}

			cout << endl;
		}
	}

	//course does not exist in table
	else {

		cout << "Course does not exist." << endl;
	}
}

//open and close course data file, gather all information in file line by line and separate it in its own course object
bool loadCourses(unordered_map<string, Course> &courseTable, string courseDataFile) {

	//open data file
	ifstream file(courseDataFile);
	string line;

	//check if file opened
	if (!file) {

		cout << "Failed to open course data file." << endl;
		return false;
	}

	//CHANGE clear existing tables if loading again
	courseTable.clear();

	//read each line of file
	while (getline(file, line)) {

		stringstream ss(line);
		vector<string> tokens;
		string token;

		//if there is an empty line skip over it
		if (line.empty()) {

			continue;
		}

		//create token divided by commas and add onto vector of tokens
		while (getline(ss, token, ',')) {

			//Call trimLine to process file information into table 
			tokens.push_back(trimLine(token));
		}

		//if missing data (missing course number or name) continue to next line
		if (tokens.size() < 2) {

			continue;
		}

		//create course object and add token data to course number and name
		Course course;
		course.courseNum = tokens[0];
		course.courseName = tokens[1];

		//get prerequisites and add onto course object
		for (size_t i = 2; i < tokens.size(); ++i) {

			course.prereq.push_back(tokens[i]);
		}

		//add course object into hasg table by course number 
		//case-sensitive check
		courseTable[checkCaseSensitve(course.courseNum)] = course;
	}

	//close file 
	file.close();
	return true;
}

//sort courses alphabetically using bubble sort
void abcSort(vector<Course>& courses) {

	//while i is less than course list continue for loop
	for (int i = 0; i < courses.size() - 1; ++i) {

		//while j is less than course list and i continue for loop
		for (int j = 0; j < courses.size() - i - 1; ++j) {

			//compare and swap if in wrong order
			if (courses[j].courseNum > courses[j + 1].courseNum) {

				swap(courses[j], courses[j + 1]);
			}
		}
	}
}

//create a vector of courses, abc sort them, and output all courses in list
void printAllCourses(unordered_map<string, Course> courseTable) {

	//create list
	vector<Course> vectorOfCourses;

	//for each course in hash table append onto vector 
	for (const auto& course : courseTable) {

		vectorOfCourses.push_back(course.second);
	}

	//call function to sort before outputting list
	abcSort(vectorOfCourses);

	//output each course in vector of courses
	for (const auto& course : vectorOfCourses) {

		cout << course.courseNum << ", " << course.courseName << endl;
	}
}

//manage input function
int getValidInput() {

	int input;
	
	while (true) {

		//pass through valid input
		if (cin >> input) {

			return input;
		}

		//if input is invalid output message and clear input
		else {

			cout << input << "is not a valid option" << endl;

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
}

//main function with main menu
int main() {

	//set varaibles
	unordered_map<string, Course> courseTable;
	string courseDataFile;
	int option = 0;

	cout << "Welcome to the course planner." << endl;

	//get data file from user and validate it exists
	while (true) {

		cout << "Enter the course data file name: ";

		getline(cin, courseDataFile);
		ifstream testFile(courseDataFile);

		if (testFile) {

			testFile.close();
			break;
		}
		else {

			cout << "Unable to open: " << courseDataFile << endl;
		}
	}

	//loop until user enters 9/exit program
	while (option != 9) {

		cout << "1. Load data structure." << endl;
		cout << "2. Print course list." << endl;
		cout << "3. Print course." << endl;
		cout << "9. Exit." << endl << endl;

		cout << "What would you like to do? "; 
		option = getValidInput();
		cout << endl;

		//option 1 call load function 
		if (option == 1) {

			loadCourses(courseTable, courseDataFile);
		}

		//option 2 output all the courses
		else if (option == 2) {

			//check if data file is not loaded to table
			if (courseTable.empty()) {

				loadCourses(courseTable, courseDataFile);
			}

			//call print all courses function
			cout << "Here is a sample schedule:" << endl << endl;
			printAllCourses(courseTable);
			cout << endl;
		}

		//option 3 find a specific course
		else if (option == 3) {

			//check if data file is not loaded to table
			if (courseTable.empty()) {

				loadCourses(courseTable, courseDataFile);
			}

			string courseChoice;

			//get users course they want to find
			cout << "What course do you want to know about?";
			cin >> courseChoice;
			cout << endl;

			//call find course function
			findCourse(courseTable, courseChoice);
			cout << endl << endl;
		}

		//option 9 exit program
		else if (option == 9) {

			cout << "Goodbye.." << endl;
			return 0;
		}

		//for invalid inputs
		else {

			cout << "Your input is not a valid option." << endl << endl;
		}

	}

	return 0;
}