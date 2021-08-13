#include <thread>
#include <vector>
#include <unordered_map>
#include "../nncurses/nncurses.hpp"
#include "Image.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using std::string,std::vector,std::out_of_range,std::unordered_map;
using nc::gettermsize,nc::TimeLimiter,nc::Renderable,nc::Terminal,nc::Texture,nc::Style,nc::Screen;
using nc::Text,nc::midOfst,nc::sleep,nc::TimeTracker,nc::Video;
using std::cout,std::to_string,std::thread;
using cv::Mat,cv::imread,cv::Size,cv::resize,cv::Vec3b,nc::blocks,cv::VideoCapture;
namespace samples=cv::samples;
namespace esc=nc::Esc;

vector<char> keys;
void keyListener(bool* end){
	while (!(*end)){
		char ch=nc::ubCinchr();
		if (!(ch=='\0'))
			keys.push_back(ch);
	}
}

char cinchr(){
	try{
		char ch=keys.at(0);
		keys.erase(keys.begin());
		return ch;
	}catch (std::out_of_range){
		return '\0';
	}
}

int main(int argc, char *argv[]){
	vector<string> arguments(argv, argv + argc);
	if (arguments.size()==1){
		cout << "Enter the file to play as an argument!\n";
		return 0 ;
	}

	Terminal terminal(Texture(" ",Style(-1,-1,0)));
	VideoCapture capture(arguments[1]);
	Video video(capture);
	bool endKeyListener=false;
	thread keyThread(keyListener,&endKeyListener);
	TimeTracker renderTracker,projTracker;

	bool debug=false;

	try{
		while (true){
			video.start();

			renderTracker.start();
			video.render(&terminal.screen);
			renderTracker.end();

			if (debug){
				Text(
					"frame: "+to_string(video.frame())+
					"\nfps: "+to_string(video.fps)+
					"\nms: "+to_string(video.ms())+
					"\nrender: "+to_string(renderTracker.time())+"ms"+
					"\nproject: "+to_string(projTracker.time())+"ms"
					"\noffset: "+to_string(video.offset)+
					"\nallTime"+to_string(video.allTime)
					,Style(-1,-1,0)
				)
					.render(&terminal.screen);
			}

			projTracker.start();
			terminal.project();
			projTracker.end();

			video.end();
			video.sync();

			char currCh=cinchr();
			if (currCh=='p'){
				Text("Paused. Press P to unpause",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);
				terminal.project(); 
				while (!(cinchr()=='p')){nc::sleep(0,500);}
				terminal.screen.fill();
			}else if (currCh=='q'){
				endKeyListener=true;
				keyThread.join();
				break;
			}else if (currCh=='d'){
				debug= !(debug);
				terminal.screen.fill();
			}else if (!(currCh=='\0')){
				cout << '\a';
			}
		}
	}catch (int){
		Text("Video ended! press any key to quit",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);
		terminal.project(); 
		while (cinchr()=='\0'){nc::sleep(0,500);}
		endKeyListener=true;
		keyThread.join();
	}
	return 0;
}