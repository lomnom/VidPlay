#include "Player.hpp"
#include <thread>

int main(int argc, char *argv[]){
	vector<string> arguments(argv, argv + argc);
	if (arguments.size()==1){
		cout << "Enter the file to display as an argument!\n";
		return 0 ;
	}
	nc::Terminal terminal(nc::Texture(" ",nc::Style(-1,-1,0)));
	videoPlayer(arguments[1],terminal);
}