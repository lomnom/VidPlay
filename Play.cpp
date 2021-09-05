#include "Player.hpp"
#include <thread>
#include <fstream>

using std::string,toroid;

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

	nc::Terminal* terminal=new nc::Terminal(nc::Texture(" ",nc::Style(-1,-1,0)));
	size_t imgPtr=0;
	string signal;

	uint8_t brightnessTresh=45;
	uint8_t greynessTresh=155;
	bool gsBlack=true;
	bool debug=false;

	while (true){
		std::ifstream file(arguments[1+imgPtr]);
		if(!file.is_open()){
			delete terminal;
		    cout << "File not found!\n";
		    return -1;
		}
		if (isVideo(arguments[1+imgPtr])){
			signal=videoPlayer(imgPtr,debug,brightnessTresh,greynessTresh,gsBlack,arguments[1+imgPtr],*terminal);
		}else if (isImage(arguments[1+imgPtr])){
			signal=imagePlayer(imgPtr,debug,brightnessTresh,greynessTresh,gsBlack,arguments[1+imgPtr],*terminal);
		}else{
			delete terminal;
			cout << "File format not recognised!\n";
			return -1;
			break;
		}

		if (signal=="QUIT") {
			delete terminal;
			break;
		}else if (signal=="NEXT"){
			imgPtr=toroid(imgPtr+1,arguments.size()-1);
		}else if (signal=="BACK"){
			imgPtr=toroid(imgPtr-1,arguments.size()-1);
		}else if (signal=="GOTO"){
			Text("Enter the file to go to!",Style(-1,-1,0),terminal->screen.cols/2-13,terminal->screen.rows/2).render(&terminal->screen);
			terminal->project();
			string file="";
			while (true){
				char inCh=nc::cinchr();
				if (isdigit(inCh)) file+=inCh;
				else break;
				Text(file,Style(-1,-1,0),nc::midOfst(terminal->screen.cols,(int)file.size()),terminal->screen.rows/2+1).render(&terminal->screen);
				terminal->project();
			}
			try{
				imgPtr=toroid(stoi(file),(int)arguments.size()-1);
			}catch (std::invalid_argument){
				imgPtr=0;
			}
		}
	}
}