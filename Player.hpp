#include "Image.hpp"
#include <thread>
#include <vector>
using nc::Text,nc::Texture,nc::HollowRectangle,nc::Style,std::cout,cv::imread;
using cv::VideoCapture,nc::VideoTimer,std::thread,nc::Terminal,std::stoi;
using nc::TimeTracker,std::string,std::to_string,std::vector,nc::Image;

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

void videoPlayer(string file,Terminal& terminal){ //this is a mess
	cv::VideoCapture video(file);
	VideoTimer timer(video);

	bool debug=false;
	bool endKeyListener=false;
	thread keyThread(keyListener,&endKeyListener);

	TimeTracker projTracker,procTracker;

	uint8_t brightnessTresh=45;
	uint8_t greynessTresh=155;
	bool gsBlack=true;
	bool paused=false;

	bool end=false;

	Mat frame;
	video >> frame;

	while (true){
		timer.start();	
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
				nnlstat("frame",timer.frame())								+
				stat("fps",timer.fps)										+
				statms("Cms",timer.ms())									+ //correct ms
				statms("Rms",timer.allTime)									+ //real ms
				statms("proccess",procTracker.time())						+ //time taken for render
				statms("project",projTime)									+ //time for project
				statms("error",timer.sleepTime-timer.sleptTime)				+ //error in correct and real
				statms("sleepTime",timer.sleepTime)							+ //time to delay for previous frame
				statms("sleptTime",timer.sleptTime)							+ //time actually delayed
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

		if (debug) terminal.screen.fill();

		projTracker.end();

		///////////handle keys
		char currCh=cinchr();
		if (currCh=='\0'){}
		else{ 
			timer.end(!(paused));
			if (currCh=='e') greynessTresh+=5;
			else if (currCh=='r') greynessTresh-=5;
			else if (currCh=='E') brightnessTresh+=5;
			else if (currCh=='R') brightnessTresh-=5;
			else if (currCh=='t') gsBlack= !(gsBlack);
			else if (currCh=='p') paused=!(paused);
			else if (currCh=='q'){
				endKeyListener=true;
				keyThread.join();
				break;
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
				}
				timer.ms(stoi(ms));
				Mat aFrame;
				video >> aFrame;
				if (!(aFrame.empty())){
					frame=aFrame;
				}else{
					end=true;
				}
			}
			else cout << '\a';
			timer.start();
		}

		timer.end(!(paused));
		timer.sync(!(paused));
	}
}

void imagePlayer(string file,Terminal& terminal){
	Mat theMat=imread(file);
	uint8_t brightnessTresh=45;
	uint8_t greynessTresh=155;
	bool gsBlack=true;
	bool debug=false;

	while (true){
		Image image(greynessTresh,brightnessTresh,gsBlack,theMat);
		image.proccess(&terminal.screen);
		image.render(&terminal.screen);

		if (debug){
			Text(
				nnlstat("greynessTresh",greynessTresh)+
				stat("brightnessTresh",brightnessTresh)+
				"\ngsBlack: "+(gsBlack?"true":"false")						
				,Style(-1,-1,0)
			).render(&terminal.screen);
		}

		terminal.project();
		terminal.screen.fill();

		char currCh=nc::cinchr();

		if (currCh=='q') break;
		else if (currCh=='d') debug= !(debug);
		else if (currCh=='e') greynessTresh+=5;
		else if (currCh=='r') greynessTresh-=5;
		else if (currCh=='E') brightnessTresh+=5;
		else if (currCh=='R') brightnessTresh-=5;
		else if (currCh=='t') gsBlack= !(gsBlack);
	}
}