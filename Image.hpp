#ifndef ncImage
#define ncImage

#include "../nncurses/Screens.hpp"
#include "../nncurses/Styles.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <unordered_map>
#include <string>
#include <array>
#include <cmath>
#include "XtermToHex.hpp"

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
	uint16_t abs(int16_t n){ 
		int16_t const mask = n >> (sizeof(int16_t) * 8 - 1); 
		return ((n + mask) ^ mask);
	} 

	uint16_t diff(int16_t a,int16_t b){
		return abs(a-b);
	}

	uint8_t HexToXterm(array<uint8_t,3> color){ //SLOWWHGFRDVBHGFVTYG
		uint8_t result=0;
		array<uint8_t,3> currCol=XtermToHex[0];
		uint16_t currDiff=diff(color[0],currCol[0])+diff(color[1],currCol[1])+diff(color[2],currCol[2]);
		uint16_t leastDiff=currDiff;

		for (int col=1;col<256;col++){
			currCol=XtermToHex[col];
			currDiff=diff(color[0],currCol[0])+diff(color[1],currCol[1])+diff(color[2],currCol[2]);
			if (currDiff<leastDiff){
				leastDiff=currDiff;
				result=col;
			}
		}
		return result;
	}

	Mat HexToXterm(Mat rgbImg){ //(un)speed
		int height=rgbImg.rows;
		int width=rgbImg.cols;
		Mat xtermImage=Mat(height,width,CV_8UC1,Scalar(0));

		unordered_map<array<uint8_t,3>,uint8_t> lookup;
		for (int row=0;row<height;row++){
			uchar* resizedBPtr = rgbImg.ptr<uchar>(row);
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
					screen->screen[startX+theight][col]=Texture(
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
		bool converted=false;
		int height,width;
		int rHeight,rWidth;
		int tHeight,tWidth;
		Mat xtermImage;

		Image(Mat image): image(image){
			height=image.rows;
			width=image.cols;
		}

		void proccess(int nheight,int nwidth){
			rHeight=nheight;
			rWidth=nwidth;
			Size newSize(rWidth,rHeight);
			Mat resized;
			resize(image,resized,newSize);
			xtermImage=HexToXterm(resized);
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
		double offset;

		TimeTracker frameTracker;
		double allTime;

		Video(VideoCapture capture): 
			video(capture), 
			fps(video.get(cv::CAP_PROP_FPS)),
			frameTime(((1.0f/fps)*1000.0f)-5)
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

		double ms(){
			return video.get(cv::CAP_PROP_POS_MSEC);
		}

		void frame(int id){
			video.set(cv::CAP_PROP_POS_FRAMES,id);
		}

		void start(){
			frameTracker.start();
		}

		void end(){
			frameTracker.end();
			allTime+=frameTracker.time();
		}

		void sync(){
			sleepTime=ms()-allTime;
			sleepTime= sleepTime>0 ? sleepTime : 0;

			int sleepTimeMs=sleepTime;
			int sleepTimeMcrs=(sleepTime-(int)sleepTime)*1000;

			start();
			sleep(0,sleepTimeMs,sleepTimeMcrs,0);
			end();

			offset=ms()-allTime;
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