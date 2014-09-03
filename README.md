text_to_cpp_string
==================

Convert text files or source code files to C++ strings that can be embedded in your code

usage example
./text_to_cpp_string ./your_text_file.txt

Currently outputs and OVERWRITES without warning a file named ./your_text_file.hpp.hpp
It is believed that you can add many files as input, but it has not been tested yet. :)

What can I use it for?

If you have a source file in some language (C++, Python, JavaScript etc.), and you want to embed that code into another C++ source file as a C++ std::string (or const char*), you'll have to preprocess it a bit. This program automates that processing.

For a source file like:

  void unlinkAllByValue()
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.unlinkByValue():";
			if(obj) cout<<*obj<<"\n"; else cout<<"nullptr\n";
		#endif
		for(link<T>* a_link : links)
		{
			a_link->unlinkByValue();
		}
		links.clear();
		obj = nullptr;
	}
	
	it will output:

#include <string>
std::string processedString = 	
"\tvoid unlinkAllByValue()\n"
"\t{\n"
"\t\t#ifdef RAE_DEBUG\n"
"\t\t\tcout<<\"LinkList.unlinkByValue():\";\n"
"\t\t\tif(obj) cout<<*obj<<\"\n\"; else cout<<\"nullptr\n\";\n"
"\t\t#endif\n"
"\t\tfor(link<T>* a_link : links)\n"
"\t\t{\n"
"\t\t\ta_link->unlinkByValue();\n"
"\t\t}\n"
"\t\tlinks.clear();\n"
"\t\tobj = nullptr;\n"
"\t}\n"
"\n"
"";

So you can embed that in your code.
