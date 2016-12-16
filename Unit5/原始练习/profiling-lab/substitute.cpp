/* substitute -- substitute strings in a list of files

  This program operates on a set of files listed on 
  the command line. The first file specifies a list of
  string substitutions to be performed on the remaining
  files. The list of string substitutions has the form:

  originalstring1
  replacement1

  originalstring2
  replacement2
  ...

  Note: A newline separates the replacement string from the 
  original string. An empty line separates one pair from the next.
  Each file is searched for instances of "originalstring1".
  Any occurences are replaced with "replacement1".
  In a similar manner, all "originalstring2"s are replaced
  with "replacement2"s, and so on.

  The results are written to the input file. Be sure
  to keep a backup of files if you do not want to lose
  the originals when you run this program.
*/


#include <iostream>
using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Text;

String* insertChar(String *data, int loc, String *replacement, int i) {
	return data->Insert(loc, replacement->Substring(i, 1));
}

String* replace_word(String* data, int loc, int length, String* replacement) {
	// delete the pattern string from loc:
	data = data->Remove(loc, length);
	// insert each character of the replacement string:
	for (int i = 0; i < replacement->Length; i++) {
		data = insertChar(data, loc+i, replacement, i);
		// data->Insert(loc + i, replacement->Substring(i, 1));
	}
	return data;
}

String* string_subst(String *data, String *pattern, String *replacement) {
	try {
			int loc;
			// find every occurrence of pattern:
			for (loc = data->IndexOf(pattern, 0); loc >= 0;
				loc = data->IndexOf(pattern, 0)) {
				// replace word
				data = replace_word(data, loc, pattern->Length, replacement);
			}	
			return data;
	} catch (Exception *e) {
		Console::WriteLine("Error in substitute ");
		Console::WriteLine(e->ToString());
		return data;
	}
}


String* batch_subst(String *data, const char* subs_filename) {
	try {
		String *subs_file = new String(subs_filename);
		StreamReader *subs_reader = new StreamReader(subs_file);
		String *pattern, *replacement, *separator;
		while (subs_reader->Peek() >= 0) {
			pattern = subs_reader->ReadLine();
			replacement = subs_reader->ReadLine();
			separator = subs_reader->ReadLine();
			data = string_subst(data, pattern, replacement);
		}
		return data;
	} catch(Exception* e ) {
		Console::WriteLine( "Error in do_substitutions ");
		Console::WriteLine( e->ToString());
		return data;
	}
}

void process_file(const char* filename, const char* subs_filename) {
	StreamReader *reader;
	StreamWriter *writer;
	String *file = new String(filename);
    try {
		reader = new StreamReader( file );
		String *data = reader->ReadToEnd();
 		data = batch_subst(data, subs_filename);
		reader->Close();
		// write the data
		writer = new StreamWriter(file, false);
		writer->Write(data);
		writer->Close();
	}  catch(Exception* e) {
		Console::WriteLine( "Error while processing file ");
		Console::WriteLine( e->ToString());
	}
}

int main(int argc, char *argv[]) {  
	if (argc < 3) {
		cout << "Not enough input arguments" << endl;
		cout << "Usage: substitute subs-file src1 src2 ..." << endl;
	} else {

		for (int i = 2; i < argc; i++) {
			process_file(argv[i], argv[1]);
		}
	}
	return 0;
}
