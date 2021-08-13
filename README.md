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
## Features
- Fast and written in c++
- Accelerates and decelerates to make up for lag/overspeed.
- [Optimises output](https://github.com/lomnom/nncurses/blob/0cc2179216cc2eae5bf13fdbabc8410484605aca/Screens.hpp#L115) to allow for smooth displaying on [*slow terminals*](https://iterm2.com/) and lower latencies over ssh
- Makes use of all 256 ansi colors to display images
- Uses ascii block elements to fit more detail into smaller spaces.
## Videos
[The player in action. ](https://cdn.discordapp.com/attachments/819056075998232635/875665379756748820/s1.mov)At the start, it starts out slightly desynchronised because of lag, but it accelerates to make up for it.
## TODO
- Make lookuptable smaller