# VidPlay, a terminal-based video viewer
## Installation
### mac
```bash
git clone https://github.com/lomnom/VidPlay
cd VidPlay
git clone https://github.com/lomnom/nncurses
brew install opencv
g++ -std=c++2a $(pkg-config --cflags --libs opencv4) -O3 Play.cpp -o play 
mv play /usr/local/bin/play
cd ..
rm -r VidPlay
```
### raspberry pi/debian
```bash
git clone https://github.com/lomnom/VidPlay
cd VidPlay
git clone https://github.com/lomnom/nncurses
sudo apt-get install libopencv-dev
clang++ -std=c++2a -O3 $(pkg-config --cflags --libs opencv) -lpthread -pthread Play.cpp -o play
mv play /usr/local/bin/play
cd ..
rm -r VidPlay
```
## Uninstallation
### unix-based systems
```bash
rm /usr/local/bin/play
```
## Usage
Playing a video:
```bash
play /path/to/video.extension
```
### Controls:
- `q`: quit
- `p`: pause/unpause
- `d`: enable debug mode 
- `e`: reduce greyscaling
- `r`: increase greyscaling
- `E`: reduce brightnessTresh
- `R`: increase brightnessTresh
- `t`: toggle turning blackened colored pixles to greyscale
- `g`: goto ms
### Debug stats:
- `Frame`: current frame
- `fps`: fps stated in video header
- `Cms`: ms that the viewer should be displaying 
- `Rms`: ms that the viewer is actually displaying
- `Render`: Time taken to proccess frame from rgb to xterm
- `Project`: Time taken to display frame on terminal
- `error`: error in previous frame's delay
- `greynessTresh`: grayscaling 
- `brightnessTresh`: treshold before pixels are greyscaled
- `gsBlack`: if blackened colored pixles are turned to greyscale
- `paused`: if video is paused
## Features
- Fast and written in c++
- Accelerates and decelerates to make up for lag/overspeed.
- [Optimises output](https://github.com/lomnom/nncurses/blob/0cc2179216cc2eae5bf13fdbabc8410484605aca/Screens.hpp#L115) to allow for smooth displaying on [*slow terminals*](https://iterm2.com/) and lower latencies over ssh
- Makes use of all 256 ansi colors to display images
- Uses ascii block elements to fit more detail into smaller spaces.
## Videos
- [Wide putin](https://drive.google.com/file/d/13UgRL1MGbNcufDHdK--rPDHIWegg4iRj/view?usp=sharing)
- [Yoasabi - Monster](https://drive.google.com/file/d/1iMFqQUvV7-KRJiZ2E4ukrAtQNSoo2q0g/view?usp=sharing)
- [Tunal tunak tun](https://drive.google.com/file/d/1U-miDEqPg-n7MepF9BeI8_7sxqDgHSZE/view?usp=sharing)
## TODO
- Allow viewing of images
- dITheRiNG???/?!!?!1?/
- More accurate timer