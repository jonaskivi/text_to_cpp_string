/*
Building:
install premake4
To create a Visual Studio 2012 solution in command line:
premake4.exe vs2012

usage:
./text_to_cpp_string yourfile.hpp

will output yourfile.hpp.cpp_string.hpp
*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
	using namespace std;

#include <stdio.h>//for FILE
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

namespace PathType
{
enum e
{
	DOES_NOT_EXIST,
	FILE,
	DIRECTORY
};
}

PathType::e checkPathType(string currentFilenamePath)
{	
	PathType::e path_type;
	path_type = PathType::DOES_NOT_EXIST; //we're a bit pessimistic here.

	struct stat s;

	if( stat(currentFilenamePath.c_str(), &s) == 0 )
	{
		if( s.st_mode & S_IFDIR )
		{
			//it's a directory
			path_type = PathType::DIRECTORY;
		}
		else if( s.st_mode & S_IFREG )
		{
			//it's a file
			path_type = PathType::FILE;
		}
		else
		{
			//something else
		}
	}
	else
	{
		//error
	}	

	return path_type;
}

class TextToCppString
{
public:
	TextToCppString()
	{
		isDone = false;
		
		char cCurrentPath[FILENAME_MAX];
		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
			cout<<"Error while getting working dir.\n";
		}
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
		cout << "The current working directory is " << cCurrentPath << "\n";

		workingPath = string(cCurrentPath); // It seems we are not yet using this workingPath for anything!

		//moduleSearchPaths.push_back( workingPath );
		//cout<<"Added current directory to file search paths: "<<moduleSearchPaths.back()<<"\n";
	}

	~TextToCppString()
	{
	}
	
	string workingPath; // It seems we are not yet using this workingPath for anything!
	
	void addSourceFile(string set_filename)
	{
		sourceFiles.push_back(set_filename);
	}

	FILE* currentFile;
	bool isDone;

	int currentChar;

	//the end product:
	string processedString;

	void parseAndWriteSourceFile(string set_filename)
	{
		currentFilenamePath = set_filename;
		sourceFiles.push_back(currentFilenamePath);

		processedString = 
		"#include <string>\n\n"
		"std::string processedString = \n\"";//first quote added already here for the first line.

		if( checkPathType(currentFilenamePath) == PathType::FILE )
		{
			currentFile = fopen(set_filename.c_str(), "r");
			
			if( currentFile != NULL )
			{	
				while( isDone == false )
				{
					parseIdle();
				}
				
				fclose(currentFile);
			}

			write(currentFilenamePath);
		}
	}

	void parseIdle()
	{
		if( EOF != (currentChar = getc(currentFile)) )
		{
			handleChar();
			return;
		}
		//else

		processedString += "\";\n\n";//add a closing quote for the last line.
		//and add the final ;

		isDone = true;
	}

	void handleChar()
	{
		if( currentChar == '\n')
		{
			processedString += "\\n\"\n\"";//add \n and quotation marks,
			//then add a real \n and another quote for the beginning line.
		}
		else if( currentChar == '\\')
		{
			processedString += "\\\\";//add two backslashes
		}
		else if( currentChar == '\"')
		{
			processedString += "\\\"";//add backslash and quote.
		}
		else if( currentChar == '\'')
		{
			processedString += "\\\'";//add backslash and apostrophe.
		}
		else if( currentChar == '\t')
		{
			processedString += "\\t";//add backslash and t
		}
		else
		{
			processedString += currentChar;
		}

	}

	string parentPath(string original_path)
	{
		std::size_t found = original_path.find_last_of("/\\");

		if(found > original_path.size())
			return ""; // no path separators, so empty string is the parent dir.

		//std::cout << " path: " << str.substr(0,found) << '\n';
		//std::cout << " file: " << str.substr(found+1) << '\n';
		cout << "path is: " << original_path.substr(0, found) << "\n";
		return original_path.substr(0, found);
	}

	string getFilenameFromPath(string original_path)
	{
		std::size_t found = original_path.find_last_of("/\\");
		//std::cout << " path: " << str.substr(0,found) << '\n';
		//std::cout << " file: " << str.substr(found+1) << '\n';
		cout << "file is: " << original_path.substr(found+1) << "\n";
		return original_path.substr(found+1);
	}

	void write(string original_filename_path)
	{
		string root_path;

		root_path = parentPath(original_filename_path);
		cout<<"root_path: "<<root_path<<"\n";
		
		if(root_path == ".")
			root_path = "./";
		
		string orig_filename = getFilenameFromPath(original_filename_path);
		original_filename_path = root_path + orig_filename + ".cpp_string.hpp";

		outFile = fopen(original_filename_path.c_str(), "w");

		if( outFile == NULL )
		{
			cout<<"Writing to file failed: "<<original_filename_path<<"\n";
			return;
		}

		fwrite( processedString.c_str(), sizeof(char), processedString.size(), outFile );

		fclose(outFile);

		cout<<"Wrote: "<<original_filename_path<<"\n";

		return;
	}

	FILE* outFile;

	//we need a modulesList here...
	//and put only a single langElement in SourceParser

	string currentFilenamePath;
	
	//A list of source files that we have already parsed:
	vector<string> sourceFiles;
};

int main(int argc, char * const argv[])
{
	if(argc <= 1)
	{
		cout<<"No source files added.\n";
		return -1;
	}

	TextToCppString TextToCppString;

	for(int i = 1; i < argc; ++i)
	{
		cout<<"Adding source file: "<<argv[i]<<"\n";
		TextToCppString.parseAndWriteSourceFile(argv[i]);
	}

	return 0;
}

