#include "Image.hpp"
#include <thread>
#include <vector>
using nc::Text,nc::Texture,nc::HollowRectangle,nc::Style,std::cout,cv::imread;
using cv::VideoCapture,nc::VideoTimer,std::thread,nc::Terminal,std::stoi;
using nc::TimeTracker,std::string,std::to_string,std::vector,nc::Image;
using std::flush;

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

template <class T>
string stat(string name,T value) {return "\n"+name+": "+to_string(value);}
template <class T>
string statms(string name, T value) {return "\n"+name+": "+to_string(value)+"ms";}
template <class T>
string nnlstat(string name, T value) {return name+": "+to_string(value);}

string videoPlayer(
	size_t id,
	bool& debug,
	uint8_t& brightnessTresh,
	uint8_t& greynessTresh,
	bool& gsBlack,
	string file,
	Terminal& terminal
){ //this is a mess
	assert(file!="");
	cv::VideoCapture video(file);
	VideoTimer timer(video);

	bool endKeyListener=false;
	thread keyThread(keyListener,&endKeyListener);

	TimeTracker projTracker,procTracker;

	bool paused=false;

	bool end=false;

	Mat frame;
	video >> frame;
	timer.start(); //start stopwatch

	while (true){
		end=false;

		///////////proccess frames
		procTracker.start();

		Image image(greynessTresh,brightnessTresh,gsBlack,frame);
		image.proccess(&terminal.screen);

		if (!(paused)) {
			Mat aFrame;
			video >> aFrame;
			if (!(aFrame.empty())){
				frame=aFrame;
			}else{
				end=true;
			}
		}

		procTracker.end();

		///////////render frame and text
		double projTime=projTracker.time();
		projTracker.start();

		image.render(&terminal.screen);

		////////render debug text
		if (debug){
			Text(
				nnlstat("fineno",id)										+
				stat("frame",timer.frame())									+
				stat("fps",timer.fps)										+
				statms("Cms",timer.ms())									+ //correct ms
				statms("Rms",timer.timer.time())							+ //real ms
				statms("proccess",procTracker.time())						+ //time taken for render
				statms("project",projTime)									+ //time for project
				statms("error",timer.time()-timer.ms()+timer.frameTime)		+ //error in correct and real
				statms("sleepTime",timer.sleepTime)							+ //time to delay for previous frame
				statms("frameTime",timer.frameTime)							+ //time actually delayed
				stat("greynessTresh",greynessTresh)							+
				stat("brightnessTresh",brightnessTresh)						+
				"\ngsBlack: "+(gsBlack?"true":"false")						+
				"\npaused: "+(paused?"true":"false")
				,Style(-1,-1,0)
			)
				.render(&terminal.screen);
		}

		///////render info text
		if (end) 
			Text("Video ended! press q to quit",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);
		else if (paused) 
			Text("Paused. Press P to unpause",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);

		terminal.project();
		cout << flush;

		if (debug) terminal.screen.fill();

		projTracker.end();

		///////////handle keys
		char currCh=cinchr();
		if (currCh=='\0'){}
		else{ 
			if (currCh=='e') greynessTresh+=5;
			else if (currCh=='r') greynessTresh-=5;
			else if (currCh=='E') brightnessTresh+=5;
			else if (currCh=='R') brightnessTresh-=5;
			else if (currCh=='t') gsBlack= !(gsBlack);
			else if (currCh=='m') {
				endKeyListener=true;
				keyThread.join();
				return "NEXT";
			}else if (currCh=='n'){
				endKeyListener=true;
				keyThread.join();
				return "BACK"; //go front 1 signal
			}else if (currCh=='p') {
				if (paused){
					terminal.screen.fill();
					paused=false;
					timer.start();
				}else{
					paused=true;
					timer.stop();
				}
			}else if (currCh=='q'){
				endKeyListener=true;
				keyThread.join();
				return "QUIT"; //stop signal
			}else if (currCh=='d'){
				debug= !(debug);
				terminal.screen.fill();
			}else if (currCh=='g'){ //*definitely not to mask ffmpeg errors.....*
				Text("Enter the ms to go to!",Style(-1,-1,0),terminal.screen.cols/2-13,terminal.screen.rows/2).render(&terminal.screen);
				string ms="";
				while (true){
					char inCh=cinchr();
					if (inCh=='\0') nc::sleep(0,50,0,0);
					else if (isdigit(inCh)) ms+=inCh;
					else break;
					Text(ms,Style(-1,-1,0),nc::midOfst(terminal.screen.cols,(int)ms.size()),terminal.screen.rows/2+1).render(&terminal.screen);
					terminal.project();
					cout << flush;
				}
				try{//this stops video
					timer.ms(stoi(ms)); 
				}catch (std::invalid_argument){
					timer.ms(0);
				}
				Mat aFrame;
				video >> aFrame;
				if (!(aFrame.empty())){
					frame=aFrame;
				}else{
					end=true;
				}
				if (!(paused)) timer.start(); //start stopwatch again after being stopped and changed
			}else if (currCh=='G'){
				endKeyListener=true;
				keyThread.join();
				return "GOTO";
			}
			else cout << '\a';
		}

		timer.sync(); //make stopwatch time same as video watch
	}
}

string imagePlayer(
	size_t id,
	bool& debug,
	uint8_t& brightnessTresh,
	uint8_t& greynessTresh,
	bool& gsBlack,
	string file,
	Terminal& terminal
){
	assert(file!="");
	Mat theMat=imread(file);

	while (true){
		Image image(greynessTresh,brightnessTresh,gsBlack,theMat);
		image.proccess(&terminal.screen);
		image.render(&terminal.screen);

		if (debug){
			Text(
				nnlstat("fineno",id)+
				stat("greynessTresh",greynessTresh)+
				stat("brightnessTresh",brightnessTresh)+
				"\ngsBlack: "+(gsBlack?"true":"false")						
				,Style(-1,-1,0)
			).render(&terminal.screen);
		}

		terminal.project();
		cout << flush;
		terminal.screen.fill();

		char currCh=nc::cinchr();

		if (currCh=='q') return "QUIT"; //quit signal
		else if (currCh=='d') debug= !(debug);
		else if (currCh=='e') greynessTresh+=5;
		else if (currCh=='r') greynessTresh-=5;
		else if (currCh=='E') brightnessTresh+=5;
		else if (currCh=='R') brightnessTresh-=5;
		else if (currCh=='m') return "NEXT"; //go back 1 signal
		else if (currCh=='n') return "BACK"; //go front 1 signal
		else if (currCh=='G') return "GOTO";
		else if (currCh=='t') gsBlack= !(gsBlack);
		else cout << '\a';
	}
}