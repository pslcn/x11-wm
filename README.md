# x11-wm

### Testing (Using Xephyr)

``` bash	
make
Xephyr -ac -screen 1280x1024 -br -reset -terminate 2> /dev/null :1 &
DISPLAY=:1 xterm &
env DISPLAY=:1 ./wm
```
