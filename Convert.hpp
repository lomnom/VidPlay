#ifndef XtThNC
#define XtThNC

#include <array>
using std::array,std::pair;

namespace nc{
	/////////////math
	int minC(array<uint8_t,3> col){
		int x= col[0]>col[1] ? col[1] : col[0];
		return x>col[2] ? col[2] : x;
	}

	int maxC(array<uint8_t,3> col){
		int x= col[0]<col[1] ? col[1] : col[0];
		return x<col[2] ? col[2] : x;
	}

	int dist(array<uint8_t,3> a,array<uint8_t,3> b){
		return std::sqrt((a[0]-b[0])^2+(a[1]-b[1])^2+(a[2]-a[2])^2);
	}

	uint16_t abs(int16_t n){ 
		int16_t const mask = n >> (sizeof(int16_t) * 8 - 1); 
		return ((n + mask) ^ mask);
	} 

	uint16_t diff(int16_t a,int16_t b){
		return abs(a-b);
	}

	uint8_t greyness(array<uint8_t,3> col){ //lmao
		return (diff(col[0],(col[1]+col[2])/2)+diff(col[1],(col[0]+col[2])/2)+diff(col[2],(col[1]+col[0])/2))/3;
	}

	uint8_t brightness(array<uint8_t,3> col){ //lmao
		return (col[0]+col[1]+col[2])/3;
	}

	/////////////216 xterm-256color added colors
	uint8_t approxNc(uint8_t val){ //approx new colors to indexes in xtVals
		if (val<=55) return 0;
		val-=55;
		return val/40;
	}

	uint8_t approxNc(array<uint8_t,3> color){ //approximate rgb to nearest new color (color-16)
		return (approxNc(color[0])*36)+(approxNc(color[1])*6)+approxNc(color[2]);
	}

	pair<uint8_t,array<uint8_t,3>> approxNcRs(array<uint8_t,3> color){ //approxNc, but returns rgb too
		array<uint8_t,3> xtCol={approxNc(color[0]),approxNc(color[1]),approxNc(color[2])};
		return {(xtCol[0]*36)+(xtCol[1]*6)+xtCol[2],xtCol};
	}

	uint8_t xtVals[16]{0,95,135,175,215,255}; //possible r,g or b vals for xterm-256col's extra 116 cols
	array<uint8_t,3> ncTOrgb(uint8_t nc){
		return {xtVals[nc/36],xtVals[(nc%36)/6],xtVals[nc%6]};
	}

	/////////////24 xterm-256 greyscale colors
	uint8_t approxGs(uint8_t val){
		if (val<=8){
			return 0;
		}
		uint8_t res=(val-8)/10;
		if (res>23){
			return 23;
		}
		return res;
	}

	uint8_t approxGs(array<uint8_t,3> color){ //approximate rgb to nearest greyscale
		return approxGs(brightness(color));
	}

	pair<uint8_t,array<uint8_t,3>> approxGsRs(array<uint8_t,3> color){ 
		uint8_t gs=approxGs((0.2126*(float)color[0])+(0.7152*(float)color[1])+(0.0722*(float)color[2]));
		uint8_t gsVal=gs*10+8;
		return {gs,{gsVal,gsVal,gsVal}};
	}

	//noneed gsVals, its n*10+8
	array<uint8_t,3> gsTOrgb(uint8_t gs){
		uint8_t val=gs*10+8;
		return {val,val,val};
	}

	/////////////16 classic colors SKRERERRERERRRERRRRRREEEEE, TODO
	array<uint8_t,4> an8Vals{0,128,192,155};
	array<uint8_t,3> ansi8[16]{ //doesnt deserve to exist
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
		{255,255,255}
	};

	/////////////rgb to xterm
	uint8_t greynessTresh=155;
	uint8_t brightnessTresh=30;
	bool gsBlack=true;
	uint8_t approxXt(array<uint8_t,3> color){ 
		return brightness(color)<brightnessTresh || (gsBlack && (color[0]<95 && color[1]<95 && color[2]<95)) || greyness(color)>greynessTresh ? 232+approxGs(color) : 16+approxNc(color);
	}

	array<uint8_t,3> xtTOrgb(uint8_t xt){
		if (xt>=16){
			if (xt<=231){
				return ncTOrgb(xt);
			}else{
				return gsTOrgb(xt);
			}
		}
		throw -1;
	}
}

#undef pair
#undef array
#endif