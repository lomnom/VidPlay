# VidPlay, a terminal-based media viewer
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
rm -rf VidPlay
```
### raspberry pi/debian
```bash
git clone https://github.com/lomnom/VidPlay
cd VidPlay
git clone https://github.com/lomnom/nncurses
sudo apt-get install libopencv-dev
clang++ -std=c++2a -O3 $(pkg-config --cflags --libs opencv) -lpthread -pthread Play.cpp -o play || clang++ -std=c++2a -O3 $(pkg-config --cflags --libs opencv4) -lpthread -pthread Play.cpp -o play
sudo mv play /usr/local/bin/play
cd ..
rm -rf VidPlay
```
## Uninstallation
### unix-based systems
```bash
rm /usr/local/bin/play
```
## Usage
Viewing media:
```bash
play /path/to/media1.extension /path/to/media2.extension ...
```
### Controls:
(^ is video only)
- `q`: quit
- ^`p`: pause/unpause
- `d`: enable debug mode 
- `e`: reduce greyscaling
- `r`: increase greyscaling
- `E`: reduce brightnessTresh
- `R`: increase brightnessTresh
- `t`: toggle turning blackened colored pixles to greyscale
- ^`g`: goto ms
- `n`: go to previous file
- `m`: go to next file
### Debug stats:
(^ is video only)
- `fileno`: current file by position in terminal args
- ^`Frame`: current frame
- ^`fps`: fps stated in video header
- ^`Cms`: ms that the viewer should be displaying 
- ^`Rms`: ms that the viewer is actually displaying
- ^`Render`: Time taken to proccess frame from rgb to xterm
- ^`Project`: Time taken to display frame on terminal
- ^`error`: error in previous frame's delay
- `greynessTresh`: grayscaling 
- `brightnessTresh`: treshold before pixels are greyscaled
- `gsBlack`: if blackened colored pixles are turned to greyscale
- ^`paused`: if video is paused
## Features
- Fast and written in c++
- Accelerates and decelerates to make up for lag/overspeed.
- [Optimises output](https://github.com/lomnom/nncurses/blob/0cc2179216cc2eae5bf13fdbabc8410484605aca/Screens.hpp#L115) to allow for smooth displaying on [*slow terminals*](https://iterm2.com/) and lower latencies over ssh
- Makes use of 240 ansi colors to display images (excluded first 16 as color depends on terminal and is slow to calculate)
- Uses ascii block elements to fit more detail into smaller spaces (2 pixels per character).
## Videos
- [Wide putin](https://drive.google.com/file/d/13UgRL1MGbNcufDHdK--rPDHIWegg4iRj/view?usp=sharing)
- [Yoasabi - Monster](https://drive.google.com/file/d/1iMFqQUvV7-KRJiZ2E4ukrAtQNSoo2q0g/view?usp=sharing)
- [Tunal tunak tun](https://drive.google.com/file/d/1U-miDEqPg-n7MepF9BeI8_7sxqDgHSZE/view?usp=sharing)
## TODO
- Update videos
- dITheRiNG???/?!!?!1?/