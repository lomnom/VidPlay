#include <thread>
#include <vector>
#include <unordered_map>
#include "nncurses/nncurses.hpp"
#include "Image.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

using std::string,std::vector,std::out_of_range,std::unordered_map,std::stoi;
using nc::gettermsize,nc::TimeLimiter,nc::Renderable,nc::Terminal,nc::Texture,nc::Style,nc::Screen;
using nc::Text,nc::midOfst,nc::sleep,nc::TimeTracker,nc::Video,nc::HollowRectangle;
using std::cout,std::to_string,std::thread;
using cv::Mat,cv::imread,cv::Size,cv::resize,cv::Vec3b,nc::blocks,cv::VideoCapture;
namespace esc=nc::Esc;

/*
compiling:
	mac:
		brew install opencv
		g++ -std=c++2a $(pkg-config --cflags --libs opencv4) -O3 Play.cpp 
	raspberry pi/debian:
		sudo apt-get install libopencv-dev
		clang++ -std=c++2a -O3 $(pkg-config --cflags --libs opencv) -lpthread -pthread Play.cpp
*/

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

Texture vertLine("|",Style(252,256,0));
Texture horizLine("-",Style(252,256,0));
Texture corner("+",Style(252,256,0));

HollowRectangle gtWin(
	0,0, //x,y
	4,24+2, //height,width
	horizLine,
	vertLine,
	corner
);

Text gtText("enter the frame to go to",Style(252,256,0),0,0);
Text inText("",Style(252,256,0),0,0);

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
					"\nCms: "+to_string(video.ms()-video.frameTime)+"ms"+ //correct ms
					"\nRms: "+to_string(video.allTime)+"ms"+ //real ms
					"\nrender: "+to_string(renderTracker.time())+"ms"+ //time taken for render
					"\nproject: "+to_string(projTracker.time())+"ms" //time for project
					"\nerror: "+to_string((video.ms()-video.frameTime)-video.allTime)+"ms"+ //error in correct and real
					"\ngreynessTresh: "+to_string(nc::greynessTresh)
					,Style(-1,-1,0)
				)
					.render(&terminal.screen);
			}

			projTracker.start();
			terminal.project();
			projTracker.end();

			char currCh=cinchr();
			if (currCh=='\0'){}
			else{
				video.end();
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
				}else if (currCh=='g'){ //goto
					gtWin.startx=midOfst(terminal.screen.cols,gtWin.width);
					gtWin.starty=midOfst(terminal.screen.rows,gtWin.height);
					gtText.starty=gtWin.starty+1;
					gtText.startx=midOfst(terminal.screen.cols,(int)gtText.text.size());

					while (true){
						inText.starty=gtWin.starty+2;
						inText.startx=midOfst(terminal.screen.cols,(int)inText.text.size());
						inText.render(&terminal.screen);
						gtWin.render(&terminal.screen);
						gtText.render(&terminal.screen);
						terminal.project();

						char inChar=cinchr();
						if (std::isdigit(inChar)){
							inText.text+=inChar;
						}else if (inChar=='\0'){
							nc::sleep(0,100);
						}else{
							break;
						}
					}
					if (!(inText.text=="")){
						video.frame(stoi(inText.text));
					}
					terminal.screen.fill();
					inText.text="";
				}else if (currCh=='e'){
					nc::greynessTresh+=5;
				}else if (currCh=='r'){
					nc::greynessTresh-=5;
				}else{
					cout << '\a';
				}
				video.start();
			}

			video.end();
			video.sync();
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