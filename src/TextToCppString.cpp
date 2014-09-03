#include <iostream>
#include <string>
#include <vector>
#include <memory>
	using namespace std;

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <boost/filesystem.hpp>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/remove_if.hpp>
	using namespace boost;

#include <stdio.h>//for FILE

typedef unsigned int uint;

namespace PathType
{
enum e
{
	DOES_NOT_EXIST,
	FILE,
	DIRECTORY
};
}

PathType::e checkPathType(boost::filesystem::path currentFilenamePath)
{	
	PathType::e path_type;
	path_type = PathType::DOES_NOT_EXIST; //we're a bit pessimistic here.

	try
	{
		if (boost::filesystem::exists(currentFilenamePath))    // does currentFilenamePath actually exist?
		{
			if (boost::filesystem::is_regular_file(currentFilenamePath))        // is currentFilenamePath a regular file?
			{
				#ifdef DEBUG_RAE_HUMAN
				cout << currentFilenamePath << " size is " << boost::filesystem::file_size(currentFilenamePath) << "\n";
				//rae::log(currentFilenamePath.string(), " size is ", boost::filesystem::file_size(currentFilenamePath), "\n");
				#endif

				path_type = PathType::FILE;
			}
			else if( boost::filesystem::is_directory(currentFilenamePath) )      // is currentFilenamePath a directory?
			{
				path_type = PathType::DIRECTORY;

				cout << currentFilenamePath << " is a directory containing:\n";
				//rae::log(currentFilenamePath.string(), "is a directory containing:\n");


				typedef vector<boost::filesystem::path> vec;             // store paths,
				vec v;                                // so we can sort them later

				copy(boost::filesystem::directory_iterator(currentFilenamePath), boost::filesystem::directory_iterator(), back_inserter(v));

				sort(v.begin(), v.end());             // sort, since directory iteration
													  // is not ordered on some file systems

				for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
				{
				  cout << "   " << *it << "\n";
					//rae::log("   ", *it, "\n");
				}
			}
			else
			{
				//Hmm, we are not handling this in PathType... oh well. What is it really? A link? A ufo?

				cout << currentFilenamePath << " exists, but is neither a regular file nor a directory\n";
				//rae::log(currentFilenamePath, " exists, but is neither a regular file nor a directory\n");
			}
		}
		else
		{
			cout << currentFilenamePath << " does not exist\n";
			//rae::log(currentFilenamePath, " does not exist\n");

			path_type = PathType::DOES_NOT_EXIST;
		}
	}
	catch (const boost::filesystem::filesystem_error& ex)
	{
		cout << ex.what() << "\n";
		//rae::log(ex.what(), "\n");
	}

	return path_type;
}

class TextToCppString
{
public:
	TextToCppString()
	{
		isDone = false;
		workingPath = boost::filesystem::current_path();
		//moduleSearchPaths.push_back( workingPath );
		//cout<<"Added current directory to file search paths: "<<moduleSearchPaths.back()<<"\n";
	}

	~TextToCppString()
	{
	}

	boost::filesystem::path workingPath;
	
	void addSourceFile(string set_filename)
	{
		boost::filesystem::path currentFilenamePath = set_filename;
		sourceFiles.push_back(currentFilenamePath);
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

	void write(boost::filesystem::path original_filename_path)
	{
		original_filename_path.replace_extension(".hpp.hpp");

		if(boost::filesystem::exists(original_filename_path.parent_path()) == false)
		{
			if(boost::filesystem::create_directories( original_filename_path.parent_path() ) )
			{
				cout<<"Created folder: "<<original_filename_path.parent_path()<<"\n";
			}
			else
			{
				cout<<"Failed to create folder: "<<original_filename_path.parent_path()<<"\n";
				return;
			}
		}

		outFile = fopen(original_filename_path.string().c_str(), "w");

		if( outFile == NULL )
		{
			cout<<"Writing to file failed: "<<original_filename_path.string()<<"\n";
			return;
		}

		fwrite( processedString.c_str(), sizeof(char), processedString.size(), outFile );

		fclose(outFile);

		cout<<"Wrote: "<<original_filename_path.string()<<"\n";

		return;
	}

	FILE* outFile;

	//we need a modulesList here...
	//and put only a single langElement in SourceParser

	boost::filesystem::path currentFilenamePath;//this is the whole filename and the path...
	
	//A list of source files that we have already parsed:
	vector<boost::filesystem::path> sourceFiles;
};

int main(int argc, char * const argv[])
{
	if(argc <= 1)
	{
		cout<<"No source files added.\n";
		return -1;
	}

	TextToCppString TextToCppString;

	for(uint i = 1; i < argc; i++)
	{
		cout<<"Adding source file: "<<argv[i]<<"\n";
		TextToCppString.parseAndWriteSourceFile(argv[i]);
	}

	return 0;
}

