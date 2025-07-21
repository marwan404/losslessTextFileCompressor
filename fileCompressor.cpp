#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <string>
using namespace std;

// Prompt user to enter file path and return it as a string
string readFilePath() {
	string filePath;
	cout << "please enter the path of the chosen file";
	getline(cin, filePath);
	return filePath;
}

// Read whole file content, lowercase it, and return as string
string readFile() {
	ifstream inputFile(readFilePath());

	if (!inputFile) {
		cerr << "Failed to open file.\n";
		exit(1);  // Exit if file can't be opened
	}

	ostringstream ss; ss << inputFile.rdbuf();
	string content = ss.str();

	// Make all characters lowercase for consistent counting
	transform(content.begin(), content.end(), content.begin(), ::tolower);

	return content;
}

// Build a frequency map of words found in the file content
map <string, int> addtoMap() {
	string str = readFile();
	map<string, int> wordDict;
	string word;

	// Read word by word, removing punctuation, then count frequencies
	istringstream iss(str);
	while (iss >> word) {
		word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
		if (!word.empty()) wordDict[word]++;
	}
	return wordDict;
}

// Assign unique index to every word that occurs more than once (repeated words)
map <int, string> wordsIndex() {
	map<int, string> indexofWords;
	map<string, int> wordMap = addtoMap();

	int index = 0;
	for (auto& pair : wordMap) {
		string word = pair.first;
		int count = pair.second;

		// Only index repeated words to optimize compression
		if (count > 1) {
			indexofWords[index++] = word;
		}
	}
	return indexofWords;
}

// Create a map from word to stringified index for fast lookup during compression
unordered_map<string, string> makeReplacerMap(const map<int, string>& indexMap) {
	unordered_map<string, string> replacer;
	for (auto pair : indexMap) {
		int idx = pair.first;
		string word = pair.second;

		// Store index as string, e.g. "0", "1", etc.
		replacer[word] = to_string(idx);
	}
	return replacer;
}

// Compress original text by replacing repeated words with their index
string compressText(const string& originalText, const unordered_map<string, string>& replacer) {
	istringstream iss(originalText);
	ostringstream output;
	string word;

	while (iss >> word) {
		string clean = word;
		string trailingPunct = "";

		// Separate punctuation attached to the end of a word (e.g., "hello," -> "hello" + ",")
		while (!clean.empty() && ispunct(clean.back())) {
			trailingPunct = clean.back() + trailingPunct;
			clean.pop_back();
		}

		// Lowercase the cleaned word for matching with replacer map
		string lower = clean;
		transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

		// Replace word with its index if it's repeated, else output original word
		if (replacer.find(lower) != replacer.end()) {
			output << replacer.at(lower) << trailingPunct << " ";
		}
		else {
			output << word << " ";
		}
	}
	return output.str();
}

// Replace the extension of a filename with a new one (e.g., "file.txt" -> "file.mzip")
string replaceExtension(const string& path, const string& newExt) {
	size_t lastSlash = path.find_last_of("/\\");
	string filename = (lastSlash == string::npos) ? path : path.substr(lastSlash + 1);

	size_t dot = filename.find_last_of('.');
	if (dot != string::npos) {
		filename = filename.substr(0, dot);
	}

	return filename + newExt;
}

// Write string content to file at specified path, with error checking
void writeToFile(const string& path, const string& newText) {
	ofstream out(path);
	if (!out) {
		cerr << "Couldn't write to file.\n";
		return;
	}
	out << newText;
	out.close();
}

// Save the dictionary of indexed words to a .dict file for decompression
void saveDict(const string& dictPath, const map<int, string>& indexMap) {
	ofstream out(dictPath);
	if (!out) {
		cerr << "Couldn't write the dictionary.\n";
		return;
	}

	// Format: index word (one per line)
	for (auto& pair : indexMap) {
		out << pair.first << " " << pair.second << "\n";
	}
	out.close();
}

// Load dictionary from .dict file, mapping index strings back to words for decompression
map<string, string> loadDict(const string& dictPath) {
	ifstream in(dictPath);
	map<string, string> indexToWord;
	int index;
	string word;

	// Read index and word pairs, store with index wrapped in brackets for lookup
	while (in >> index >> word) {
		indexToWord["[" + to_string(index) + "]"] = word;
	}
	return indexToWord;
}

// Decompress text by replacing indices with original words using loaded dictionary
string decompressText(const string& compressedText, const map<string, string>& indexToWord) {
	istringstream iss(compressedText);
	ostringstream output;
	string token;

	while (iss >> token) {
		string clean = token;
		string trailingPunct = "";

		// Separate trailing punctuation from token
		while (!clean.empty() && ispunct(clean.back())) {
			trailingPunct = clean.back() + trailingPunct;
			clean.pop_back();
		}

		// Replace index tokens with original words if found in dictionary
		if (indexToWord.find(clean) != indexToWord.end()) {
			output << indexToWord.at(clean) << trailingPunct << " ";
		}
		else {
			output << token << " ";
		}
	}

	return output.str();
}

int main() {
	cout << "Choose mode: [1] Compress  [2] Decompress\n> ";
	int choice;
	cin >> choice;
	cin.ignore(); // Clear newline from input buffer

	if (choice == 1) {
		// Compress mode: read file, compress, save compressed file & dictionary
		string path = readFilePath();
		ifstream inputFile(path);
		if (!inputFile) {
			cerr << "File couldn't be opened.\n";
			return 1;
		}

		ostringstream ss; ss << inputFile.rdbuf();
		string fileContent = ss.str();

		map<int, string> indexMap = wordsIndex();
		auto replacer = makeReplacerMap(indexMap);
		string compressedText = compressText(fileContent, replacer);

		string outputFile = replaceExtension(path, ".mzip");
		writeToFile(outputFile, compressedText);

		saveDict(replaceExtension(path, ".dict"), indexMap);

		cout << "File compressed!\n";
	}
	else if (choice == 2) {
		// Decompress mode: load compressed file & dictionary, decompress and save output
		cout << "Enter .mzip file path:\n> ";
		string compressedPath;
		getline(cin, compressedPath);

		cout << "Enter dictionary (.dict) file path:\n> ";
		string dictPath;
		getline(cin, dictPath);

		ifstream in(compressedPath);
		if (!in) {
			cerr << "Could not read compressed file.\n";
			return 1;
		}

		ostringstream ss; ss << in.rdbuf();
		string compressedText = ss.str();

		map<string, string> dict = loadDict(dictPath);
		string original = decompressText(compressedText, dict);

		writeToFile(replaceExtension(compressedPath, ".decompressed.txt"), original);
		cout << "File decompressed!\n";
	}
	else {
		cout << "Invalid option\n";
	}
	return 0;
}