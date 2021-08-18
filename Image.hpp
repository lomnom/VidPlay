#ifndef ncImage
#define ncImage

#include "nncurses/nncurses.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <unordered_map>
#include <string>
#include <array>
#include <cmath>

using std::string,std::array,std::unordered_map,std::out_of_range;
using cv::Mat,cv::Size,cv::Scalar,cv::VideoCapture,nc::Stopwatch;

template<size_t N> 
struct std::hash<std::array<uint8_t, N>> {
	auto operator() (const std::array<uint8_t, N>& key) const {
		std::hash<uint8_t> hasher;
		size_t result = 0;
		for(size_t i = 0; i < N; ++i) {
			result = result * 31 + hasher(key[i]); // ??
		}
		return result;
	}
};

namespace nc{
	Mat HexToXterm(uint8_t greynessTresh,uint8_t brightnessTresh,bool gsBlack,Mat rgbImg){
		int height=rgbImg.rows;
		int width=rgbImg.cols;
		Mat xtermImage=Mat(height,width,CV_8UC1,Scalar(0));

		for (int row=0;row<height;row++){
			uchar* resizedBPtr = rgbImg.ptr<uchar>(row);
			uchar* resizedGPtr = resizedBPtr+1;
			uchar* resizedRPtr = resizedBPtr+2;
			uchar* xtermPtr = xtermImage.ptr<uchar>(row);
			for (int col=0;col<width;col++){
				int colIndex=col*3;
				xtermPtr[col]=approxXt(gsBlack,greynessTresh,brightnessTresh,array<int16_t,3>{resizedRPtr[colIndex],resizedGPtr[colIndex],resizedBPtr[colIndex]});
			}
		}

		return xtermImage;
	}

	class Image{ //AAHGVGVNHSG
	public:
		Mat image;
		uint8_t greynessTresh;
		uint8_t brightnessTresh;
		bool gsBlack;
		bool converted=false;
		int height,width;
		int rHeight,rWidth;
		int tHeight,tWidth;
		Mat xtermImage;
		const string character=blocks[0b1100];

		Image(uint8_t greynessTresh,uint8_t brightnessTresh,bool gsBlack,Mat image): 
			image(image), 
			greynessTresh(greynessTresh),
			brightnessTresh(brightnessTresh),
			gsBlack(gsBlack)
		{
			if (image.empty()) throw -1;
			height=image.rows;
			width=image.cols;
		}

		void proccess(int nheight,int nwidth){
			rHeight=nheight;
			rWidth=nwidth;
			Size newSize(rWidth,rHeight);
			Mat resized;
			resize(image,resized,newSize);
			xtermImage=HexToXterm(greynessTresh,brightnessTresh,gsBlack,resized);
			converted=true;
			tHeight=(rHeight/2)+(rHeight%2);
			tWidth=rWidth;
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
				for (int row=0;row<rHeight/2;row++){
					uchar* topPtr = xtermImage.ptr<uchar>(row*2);
					uchar* btmPtr = xtermImage.ptr<uchar>(row*2+1);
					for (int col=0;col<rWidth;col++){
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
				if ( height%2==1 ){
					uchar* ptr = xtermImage.ptr<uchar>(height-1);
					for (int col=0;col<rWidth;col++){
						screen->screen[startY+rHeight-1][startX+col]=Texture(
							character,
							Style(
								ptr[col],
								-1,0
							)
						);
					}
				}
			}else{
				throw -1;
			}
		}

		void render(Screen* scr){
			render(scr,midOfst(scr->cols,tWidth), midOfst(scr->rows,tHeight));
		}
	};

	class VideoTimer{ //SKRERERRERERERRERE
	public:
		VideoCapture& video;
		double fps;

		double frameTime;
		double sleepTime;
		double sleptTime;

		bool stopped;

		Stopwatch timer;

		VideoTimer(VideoCapture& video):
			video(video), 
			fps(video.get(cv::CAP_PROP_FPS)),
			frameTime(((1.0f/fps)*1000.0f)-5)
		{}

		int frame(){
			return video.get(cv::CAP_PROP_POS_FRAMES);
		}

		double ms(){
			return video.get(cv::CAP_PROP_POS_MSEC);
		}

		void ms(double ms){
			video.set(cv::CAP_PROP_POS_MSEC,ms);
			timer.othTime=ms;
			timer.stopped=true;
		}

		void frame(int id){ //AAAAAAAAA
			timer.othTime=((id/fps)*1000)+(fmod(id,fps)*frameTime);
			video.set(cv::CAP_PROP_POS_MSEC,timer.othTime);
			timer.stopped=true;
		}

		int frames(){
			return video.get(cv::CAP_PROP_FRAME_COUNT);
		}

		void start(){
			timer.start();
		}

		void restart(){
			timer.reset();
		}

		void stop(){
			timer.stop();
		}

		double time(){
			return timer.time();
		}

		void sync(){ //>0 is too fast, <0 is too slow
			double currTime=timer.time();
			sleepTime=ms()-currTime;
			sleepTime= sleepTime>0 ? sleepTime : 0;

			int sleepTimeMs=sleepTime;
			int sleepTimeMcrs=(sleepTime-(int)sleepTime)*1000;

			sleep(0,sleepTimeMs,sleepTimeMcrs,0);
		}
	};
}

#undef string
#undef array
#undef unordered_map
#undef Mat
#undef Size
#undef Scalar
#undef VideoCapture
#undef Image

#endif