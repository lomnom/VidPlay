#include "../nncurses/Screens.hpp"
#include "../nncurses/Styles.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <unordered_map>
#include <string>
#include <array>

using std::string,std::array;


namespace nc{
	array<uint8_t,3> XtermToHex[256]{
		{0,0,0},
		{128,0,0},
		{0,128,0},
		{128,128,0},
		{0,0,128},
		{128,0,128},
		{0,128,128},
		{192,192,192},
		{128,128,128},
		{255,0,0},
		{0,255,0},
		{255,255,0},
		{0,0,255},
		{255,0,255},
		{0,255,255},
		{255,255,255},
		{0,0,0},
		{0,0,95},
		{0,0,135},
		{0,0,175},
		{0,0,215},
		{0,0,255},
		{0,95,0},
		{0,95,95},
		{0,95,135},
		{0,95,175},
		{0,95,215},
		{0,95,255},
		{0,135,0},
		{0,135,95},
		{0,135,135},
		{0,135,175},
		{0,135,215},
		{0,135,255},
		{0,175,0},
		{0,175,95},
		{0,175,135},
		{0,175,175},
		{0,175,215},
		{0,175,255},
		{0,215,0},
		{0,215,95},
		{0,215,135},
		{0,215,175},
		{0,215,215},
		{0,215,255},
		{0,255,0},
		{0,255,95},
		{0,255,135},
		{0,255,175},
		{0,255,215},
		{0,255,255},
		{95,0,0},
		{95,0,95},
		{95,0,135},
		{95,0,175},
		{95,0,215},
		{95,0,255},
		{95,95,0},
		{95,95,95},
		{95,95,135},
		{95,95,175},
		{95,95,215},
		{95,95,255},
		{95,135,0},
		{95,135,95},
		{95,135,135},
		{95,135,175},
		{95,135,215},
		{95,135,255},
		{95,175,0},
		{95,175,95},
		{95,175,135},
		{95,175,175},
		{95,175,215},
		{95,175,255},
		{95,215,0},
		{95,215,95},
		{95,215,135},
		{95,215,175},
		{95,215,215},
		{95,215,255},
		{95,255,0},
		{95,255,95},
		{95,255,135},
		{95,255,175},
		{95,255,215},
		{95,255,255},
		{135,0,0},
		{135,0,95},
		{135,0,135},
		{135,0,175},
		{135,0,215},
		{135,0,255},
		{135,95,0},
		{135,95,95},
		{135,95,135},
		{135,95,175},
		{135,95,215},
		{135,95,255},
		{135,135,0},
		{135,135,95},
		{135,135,135},
		{135,135,175},
		{135,135,215},
		{135,135,255},
		{135,175,0},
		{135,175,95},
		{135,175,135},
		{135,175,175},
		{135,175,215},
		{135,175,255},
		{135,215,0},
		{135,215,95},
		{135,215,135},
		{135,215,175},
		{135,215,215},
		{135,215,255},
		{135,255,0},
		{135,255,95},
		{135,255,135},
		{135,255,175},
		{135,255,215},
		{135,255,255},
		{175,0,0},
		{175,0,95},
		{175,0,135},
		{175,0,175},
		{175,0,215},
		{175,0,255},
		{175,95,0},
		{175,95,95},
		{175,95,135},
		{175,95,175},
		{175,95,215},
		{175,95,255},
		{175,135,0},
		{175,135,95},
		{175,135,135},
		{175,135,175},
		{175,135,215},
		{175,135,255},
		{175,175,0},
		{175,175,95},
		{175,175,135},
		{175,175,175},
		{175,175,215},
		{175,175,255},
		{175,215,0},
		{175,215,95},
		{175,215,135},
		{175,215,175},
		{175,215,215},
		{175,215,255},
		{175,255,0},
		{175,255,95},
		{175,255,135},
		{175,255,175},
		{175,255,215},
		{175,255,255},
		{215,0,0},
		{215,0,95},
		{215,0,135},
		{215,0,175},
		{215,0,215},
		{215,0,255},
		{215,95,0},
		{215,95,95},
		{215,95,135},
		{215,95,175},
		{215,95,215},
		{215,95,255},
		{215,135,0},
		{215,135,95},
		{215,135,135},
		{215,135,175},
		{215,135,215},
		{215,135,255},
		{215,175,0},
		{215,175,95},
		{215,175,135},
		{215,175,175},
		{215,175,215},
		{215,175,255},
		{215,215,0},
		{215,215,95},
		{215,215,135},
		{215,215,175},
		{215,215,215},
		{215,215,255},
		{215,255,0},
		{215,255,95},
		{215,255,135},
		{215,255,175},
		{215,255,215},
		{215,255,255},
		{255,0,0},
		{255,0,95},
		{255,0,135},
		{255,0,175},
		{255,0,215},
		{255,0,255},
		{255,95,0},
		{255,95,95},
		{255,95,135},
		{255,95,175},
		{255,95,215},
		{255,95,255},
		{255,135,0},
		{255,135,95},
		{255,135,135},
		{255,135,175},
		{255,135,215},
		{255,135,255},
		{255,175,0},
		{255,175,95},
		{255,175,135},
		{255,175,175},
		{255,175,215},
		{255,175,255},
		{255,215,0},
		{255,215,95},
		{255,215,135},
		{255,215,175},
		{255,215,215},
		{255,215,255},
		{255,255,0},
		{255,255,95},
		{255,255,135},
		{255,255,175},
		{255,255,215},
		{255,255,255},
		{8,8,8},
		{18,18,18},
		{28,28,28},
		{38,38,38},
		{48,48,48},
		{58,58,58},
		{68,68,68},
		{78,78,78},
		{88,88,88},
		{98,98,98},
		{18,18,18},
		{118,118,118},
		{128,128,128},
		{138,138,138},
		{148,148,148},
		{158,158,158},
		{168,168,168},
		{178,178,178},
		{188,188,188},
		{198,198,198},
		{28,28,28},
		{218,218,218},
		{228,228,228},
		{238,238,238}
	};

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
}