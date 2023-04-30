#!/bin/sh

Xephyr -ac -screen 800x600 -br -reset -terminate 2> /dev/null :1 &
