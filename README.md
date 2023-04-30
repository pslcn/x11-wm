# x11-wm

## Testing (Using Xephyr)

After running the `test/xephyr_start.sh` shell script:

``` bash	
DISPLAY=:1 $TERMINAL & # Optionally spawn a terminal
env DISPLAY=:1 ./wm
```
