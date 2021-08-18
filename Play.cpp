#include "Player.hpp"
#include <thread>

using std::string;

bool hasEnding(string const &fullString, string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

bool isVideo(string& fname){
	return hasEnding(fname,".mp4") ||
		   hasEnding(fname,".gif") ||
		   hasEnding(fname,".avi") ||
		   hasEnding(fname,".h64") ||
		   hasEnding(fname,".mov");
}

bool isImage(string& fname){
	return hasEnding(fname,".bmp") ||
		   hasEnding(fname,".dib") ||
		   hasEnding(fname,".jpeg") ||
		   hasEnding(fname,".jpg") ||
		   hasEnding(fname,".jpe") ||
		   hasEnding(fname,".jp2") ||
		   hasEnding(fname,".png") ||
		   hasEnding(fname,".pbm") ||
		   hasEnding(fname,".pgm") ||
		   hasEnding(fname,".ppm") ||
		   hasEnding(fname,".sr") ||
		   hasEnding(fname,".ras") ||
		   hasEnding(fname,".tiff") ||
		   hasEnding(fname,".tif");
}

int main(int argc, char *argv[]){
	vector<string> arguments(argv, argv + argc);
	if (arguments.size()==1){
		cout << "Enter the file to display as an argument!\n";
		return 0 ;
	}

	if (isVideo(arguments[1])){
		nc::Terminal terminal(nc::Texture(" ",nc::Style(-1,-1,0)));
		videoPlayer(arguments[1],terminal);
	}else if (isImage(arguments[1])){
		nc::Terminal terminal(nc::Texture(" ",nc::Style(-1,-1,0)));
		imagePlayer(arguments[1],terminal);
	}else{
		cout << "File format not recognised!\n";
	}
}