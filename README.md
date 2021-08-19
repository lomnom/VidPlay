# VidPlay, a terminal-based ***media*** viewer
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
- `G`: goto file
- `n`: go to previous file
- `m`: go to next file
### Debug stats:
(^ is video only)
- `fileno`: current file by position in terminal args
- ^`Frame`: current frame
- ^`fps`: fps stated in video header
- ^`Cms`: ms that the viewer should be displaying 
- ^`Rms`: ms that the viewer is actually displaying
- ^`preccess`: Time taken to proccess frame from rgb to xterm
- ^`Project`: Time taken to display frame on terminal
- ^`error`: error in previous frame's delay
- `greynessTresh`: greyness treshold before pixels are greyscaled
- `brightnessTresh`: brightness treshold before pixels are greyscaled
- `gsBlack`: if blackened colored pixles are turned to greyscale
- ^`paused`: if video is paused
## Features
- Fast and written in c++
- Accelerates and decelerates to make up for lag/overspeed for videos.
- [Optimises output](https://github.com/lomnom/nncurses/blob/0cc2179216cc2eae5bf13fdbabc8410484605aca/Screens.hpp#L115) to allow for smooth displaying on [*slow terminals*](https://iterm2.com/) and lower latencies over ssh
- Makes use of 240 ansi colors to display media (excluded first 16 as color depends on terminal and is slow to calculate)
- Uses ascii block elements to fit more detail into smaller spaces (2 pixels per character).
- Accurate fps limiting for videos
- Customisable algorithm for converting rgb to xterm-256color
## [Demo](https://drive.google.com/file/d/179ICvBN1iNA6bhjSrHAj72m67gxXzGvx/view?usp=sharing)
(cant asciinema as the escape codes makes it stroke. The block elements also look terrible there.)  
(The command that was used in the video is `play E.mp4 W_A_R_P.png WIDE.mp4 RICK.mp4`)
## TODO
- Add audio (***SKREEEEEEE***)
- dITheRiNG???/?!!?!1?/
## Issues
Please open a github issue if you see any issues, i'll fix it when i see it