#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
using namespace std;
class DebugHelper
{
public:
	DebugHelper();
	static void WriteToDebugOutput(string s);
	static void Dump(double values[],int size,string fileName);
	static string ToString(double value);
	~DebugHelper();
};

