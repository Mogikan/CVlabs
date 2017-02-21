#include "DebugHelper.h"
#include <Windows.h>



DebugHelper::DebugHelper()
{
}

void DebugHelper::WriteToDebugOutput(string s)
{
	std::ostringstream os_;
	os_ << s;
	OutputDebugStringA(os_.str().c_str());	
}

void DebugHelper::WriteToDebugOutput(double value)
{
	WriteToDebugOutput(ToString(value));
}

void DebugHelper::Dump(double values[], int size, string fileName)
{
	ofstream myfile;
	myfile.open(fileName);
	for (int i = 0; i < size; i++)
	{
		myfile << values[i] << " ";
	}
	myfile.close();
}


string DebugHelper::ToString(double value)
{
	std::ostringstream strs;
	strs << value << "\n\r";
	std::string str = strs.str();
	return str;
}

DebugHelper::~DebugHelper()
{
}
