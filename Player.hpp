#include "Image.hpp"
#include <thread>
#include <vector>
using nc::Text,nc::Texture,nc::HollowRectangle,nc::Style,std::cout;
using cv::VideoCapture,nc::VideoTimer,std::thread,nc::Terminal;
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

void videoPlayer(string file,Terminal& terminal){
	Texture vertLine("|",Style(252,256,0));
	Texture horizLine("-",Style(252,256,0));
	Texture corner("+",Style(252,256,0));
	Text gtText("enter the frame to go to",Style(252,256,0),0,0);
	Text inText("",Style(252,256,0),0,0);

	HollowRectangle gtWin(
		0,0, //x,y
		4,24+2, //height,width
		horizLine,
		vertLine,
		corner
	);

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

	Mat frame;
	video >> frame;

	while (true){
		timer.start();

		procTracker.start();
		Image image(greynessTresh,brightnessTresh,gsBlack,frame);
		if (!(paused)) video >> frame;
		image.proccess(&terminal.screen);
		procTracker.end();

		double projTime=projTracker.time();
		projTracker.start();
		image.render(&terminal.screen);
		if (debug){
			Text(
				nnlstat("frame",timer.frame())								+
				stat("fps",timer.fps)										+
				statms("Cms",timer.ms()-timer.frameTime)					+ //correct ms
				statms("Rms",timer.allTime)									+ //real ms
				statms("proccess",procTracker.time())						+ //time taken for render
				statms("project",projTime)									+ //time for project
				statms("error",(timer.ms()-timer.frameTime)-timer.allTime)	+ //error in correct and real
				stat("greynessTresh",greynessTresh)							+
				stat("brightnessTresh",brightnessTresh)						+
				"\ngsBlack: "+(gsBlack?"true":"false")						+
				"\npaused: "+(paused?"true":"false")
				,Style(-1,-1,0)
			)
				.render(&terminal.screen);
		}
		if (paused) Text("Paused. Press P to unpause",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);;
		terminal.project();
		projTracker.end();

		char currCh=cinchr();
		if (currCh=='\0'){}
		else{ //TODO: goto
			timer.end(!(paused));
			if (currCh=='q'){
				endKeyListener=true;
				keyThread.join();
				break;
			}else if (currCh=='d'){
				debug= !(debug);
				terminal.screen.fill();
			}else if (currCh=='e') greynessTresh+=5;
			else if (currCh=='r') greynessTresh-=5;
			else if (currCh=='E') brightnessTresh+=5;
			else if (currCh=='R') brightnessTresh-=5;
			else if (currCh=='t') gsBlack= !(gsBlack);
			else if (currCh=='p') paused=!(paused);
			else if (currCh=='a'){
				double time=timer.ms();
				if (time > 5000) timer.ms(timer.ms()-5000);
				else timer.ms(0);
				video >> frame;
			}else if (currCh=='s'){
				timer.ms(timer.ms()+5000);
				video >> frame;
			}
			else cout << '\a';
			timer.start();
		}

		timer.end(!(paused));
		timer.sync(!(paused));
	}
	Text("Video ended! press any key to quit",Style(-1,-1,0),terminal.screen.cols/2-13,0).render(&terminal.screen);
	cinchr();
}