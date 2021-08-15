#ifndef ncImage
#define ncImage

#include "nncurses/Screens.hpp"
#include "nncurses/Styles.hpp"
#include "nncurses/Convert.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <unordered_map>
#include <string>
#include <array>
#include <cmath>

using std::string,std::array,std::unordered_map,std::out_of_range;
using cv::Mat,cv::Size,cv::Scalar,cv::VideoCapture;

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
				xtermPtr[col]=approxXt(gsBlack,greynessTresh,brightnessTresh,array<uint8_t,3>{resizedRPtr[colIndex],resizedGPtr[colIndex],resizedBPtr[colIndex]});
			}
		}

		return xtermImage;
	}

	class Pixles{
	public:
		Mat pixles;
		int height,width;
		int theight,twidth;
		string character=blocks[0b1100];
		Pixles(Mat pixles): pixles(pixles){
			height=pixles.rows;
			width=pixles.cols;
			twidth=width;
			theight=(height/2)+(height%2);
		}

		void render(Screen* screen,int startX,int startY){
			for (int row=0;row<height/2;row++){
				uchar* topPtr = pixles.ptr<uchar>(row*2);
				uchar* btmPtr = pixles.ptr<uchar>(row*2+1);
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
			if ( height%2==1 ){
				uchar* ptr = pixles.ptr<uchar>(height-1);
				for (int col=0;col<width;col++){
					screen->screen[startY+theight-1][startX+col]=Texture(
						character,
						Style(
							ptr[col],
							-1,0
						)
					);
				}
			}
		}
	};

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

		Image(uint8_t greynessTresh,uint8_t brightnessTresh,bool gsBlack,Mat image): 
			image(image), 
			greynessTresh(greynessTresh),
			brightnessTresh(brightnessTresh),
			gsBlack(gsBlack)
		{
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
				Pixles(xtermImage).render(screen,startX,startY);
			}else{
				throw -1;
			}
		}

		void render(Screen* scr){
			render(scr,midOfst(scr->cols,tWidth), midOfst(scr->rows,tHeight));
		}
	};

	class Video{
	public:
		VideoCapture video;
		int fps;

		double frameTime;
		double sleepTime;

		TimeTracker frameTracker;
		double allTime;

		uint8_t greynessTresh;
		uint8_t brightnessTresh;
		bool gsBlack;

		Video(uint8_t greynessTresh,uint8_t brightnessTresh,bool gsBlack,VideoCapture capture): 
			video(capture), 
			fps(video.get(cv::CAP_PROP_FPS)),
			frameTime(((1.0f/fps)*1000.0f)-5),
			greynessTresh(greynessTresh),
			brightnessTresh(brightnessTresh),
			gsBlack(gsBlack)
		{}

		void render(Screen* scr,int startX,int startY){
			Mat frame;
			video >> frame;
			if (frame.empty()){
				throw -1;
			}
			Image image(greynessTresh,brightnessTresh,gsBlack,frame);
			image.proccess(scr);
			image.render(scr,startX,startY);
		}

		void render(Screen* scr){
			Mat frame;
			video >> frame;
			if (frame.empty()){
				throw -1;
			}
			Image image(greynessTresh,brightnessTresh,gsBlack,frame);
			image.proccess(scr);
			image.render(scr);
		}

		void render(Screen* scr,int height,int width,int startX,int startY){
			Mat frame;
			video >> frame;
			if (frame.empty()){
				throw -1;
			}
			Image image(greynessTresh,brightnessTresh,gsBlack,frame);
			image.proccess(height,width);
			image.render(scr,startX,startY);
		}

		int frame(){
			return video.get(cv::CAP_PROP_POS_FRAMES);
		}

		double ms(){
			return video.get(cv::CAP_PROP_POS_MSEC);
		}

		void frame(int id){ //AAAAAAAAA
			allTime=((id/fps)*1000)+((id%fps)*frameTime);
			video.set(cv::CAP_PROP_POS_MSEC,allTime);
		}

		void start(){
			frameTracker.start();
		}

		void end(){
			frameTracker.end();
			allTime+=frameTracker.time();
		}

		void sync(){ //>0 is too fast, <0 is too slow
			start();
			sleepTime=ms()-allTime;
			sleepTime= sleepTime>0 ? sleepTime : 0;

			int sleepTimeMs=sleepTime;
			int sleepTimeMcrs=(sleepTime-(int)sleepTime)*1000;

			sleep(0,sleepTimeMs,sleepTimeMcrs,0);

			end();
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