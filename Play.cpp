#include <opencv2/videoio/videoio_c.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <unordered_map>
#include "HexToXterm.hpp"
#include "../nncurses/nncurses.hpp"

using std::string,std::vector,std::out_of_range,std::unordered_map;
using nc::gettermsize,nc::TimeLimiter,nc::Renderable,nc::Terminal,nc::Texture,nc::Style,nc::Screen;
using nc::Text,nc::midOfst,nc::sleep,nc::TimeTracker;
using std::cout,std::to_string,std::thread;
using cv::Mat,cv::imread,cv::imshow,cv::waitKey,cv::Size,cv::resize,cv::Vec3b,nc::blocks,cv::VideoCapture;
namespace samples=cv::samples;
namespace esc=nc::Esc;

template<class T, size_t N> 
struct std::hash<std::array<T, N>> {
    auto operator() (const std::array<T, N>& key) const {
        std::hash<T> hasher;
        size_t result = 0;
        for(size_t i = 0; i < N; ++i) {
            result = result * 31 + hasher(key[i]); // ??
        }
        return result;
    }
};

class Image{
public:
	Mat image;
	bool converted=false;
	int height,width;
	Mat xtermImage;
	string character=blocks[0b1100];

	Image(Mat image): image(image){
		height=image.rows;
		width=image.cols;
	}

	void proccess(int nheight,int nwidth){
		height=nheight;
		width=nwidth;

		Size newSize(width,height);
		Mat resized;

		xtermImage=Mat(height,width, CV_8UC1,cv::Scalar(0));

		resize(image,resized,newSize);

		unordered_map<array<uint8_t,3>,uint8_t> lookup;
		for (int row=0;row<height;row++){
			uchar* resizedBPtr = resized.ptr<uchar>(row);
			uchar* resizedGPtr = resizedBPtr+1;
			uchar* resizedRPtr = resizedBPtr+2;
			uchar* xtermPtr = xtermImage.ptr<uchar>(row);
			for (int col=0;col<width;col++){
				int colIndex=col*3;
				array<uint8_t,3> color={resizedRPtr[colIndex],resizedGPtr[colIndex],resizedBPtr[colIndex]};
				try{
					xtermPtr[col]=lookup.at(color);
				}catch(out_of_range){
					lookup[color]=HexToXterm(color);
					xtermPtr[col]=lookup.at(color);
				}
			}
		}
		converted=true;
	}

	void proccess(Screen* screen){ //fill as much as possible while mantaining ratio
		int maxHeight=screen->rows*2;
		int maxWidth=screen->cols;
		double transformX=(double)width/maxWidth;
		double transformY=(double)height/maxHeight;
		double transform= transformY>transformX ? transformY : transformX;

		proccess(height/transform,width/transform);
	}

	void render(Screen* screen,int startX,int startY){
		if (converted){
			for (int row=0;row<height/2;row++){
				uchar* topPtr = xtermImage.ptr<uchar>(row*2);
				uchar* btmPtr = xtermImage.ptr<uchar>(row*2+1);
				for (int col=0;col<width;col++){
					screen->screen[row+startY][col+startX]=Texture(
						character,
						Style(
							topPtr[col],
							btmPtr[col],
							0
						)
					);
				}
			}
			// if ( height-((height/2)*2)==1 ){ //extra row
			// 	uchar* ptr = xtermImage.ptr<uchar>(height-1);
			// 	for (int col=0;col<width;col++){
			// 		screen->screen[startX+height][col]=Texture(
			// 			character,
			// 			Style(
			// 				ptr[col],
			// 				-1,0
			// 			)
			// 		);
			// 	}
			// }
		}else{
			throw -1;
		}
	}

	void render(Screen* scr){
		render(scr,midOfst(scr->cols,width), midOfst(scr->rows,height/2+(height%2) ));
	}
};

class Video{
public:
	cv::VideoCapture video;
	int fps;
	double frameTime;
	double offset,adjust;
	TimeLimiter frameTracker;
	TimeTracker frameSecondTracker;

	Video(VideoCapture capture): 
		video(capture), 
		fps(video.get(cv::CAP_PROP_FPS)),
		frameTime(((1.0f/fps)*1000.0f)-5),
		frameTracker(frameTime)
	{}

	void render(Screen* scr,int startX,int startY){
		Mat frame;
		video >> frame;
		if (frame.empty()){
			throw -1;
		}
		Image image(frame);
		image.proccess(scr);
		image.render(scr,startX,startY);
	}

	void render(Screen* scr){
		Mat frame;
		video >> frame;
		if (frame.empty()){
			throw -1;
		}
		Image image(frame);
		image.proccess(scr);
		image.render(scr);
	}

	void render(Screen* scr,int height,int width,int startX,int startY){
		Mat frame;
		video >> frame;
		if (frame.empty()){
			throw -1;
		}
		Image image(frame);
		image.proccess(height,width);
		image.render(scr,startX,startY);
	}

	int frame(){
		return video.get(cv::CAP_PROP_POS_FRAMES);
	}

	int ms(){
		return video.get(cv::CAP_PROP_POS_MSEC);
	}

	void frame(int id){
		video.set(cv::CAP_PROP_POS_FRAMES,id);
	}

	void start(){
		frameTracker.start();
		if (frame()%fps==0){
			frameSecondTracker.start();
		}
	}

	void end(){
		frameTracker.end();
		if (frame()%fps==fps-1){
			frameSecondTracker.end();
		}
	}

	void sync(){
		frameTracker.delay();
		if (frame()%fps==fps-1){
			offset=frameSecondTracker.time()-1000;
			double frameOff= fmod(offset,frameTime);
			adjust= -frameOff*0.1;
			frameTracker.time+=adjust;
			if (frameTracker.time<0) frameTracker.time=0;
		}
	}
};

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
					"\ndelay: "+to_string(video.frameTracker.time)+
					"\noffset: "+to_string(video.offset)+
					"\nadjust: "+to_string(video.adjust)
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