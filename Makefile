CC=clang-10

hsvbench: hsvbench.c hsv.h
	$(CC) -std=c99 -D_GNU_SOURCE -o hsvbench -O2 -g hsvbench.c -lm

run: hsvbench
	./hsvbench ppm=1
	convert out0.ppm images/out0.png
	convert out1.ppm images/out1.png
	convert out2.ppm images/out2.png
	-display images/out?.png
	pnmarith -difference out0.ppm out1.ppm > diff01.ppm
	pnmarith -difference out0.ppm out2.ppm > diff02.ppm
	convert diff01.ppm images/diff01.png
	convert diff02.ppm images/diff02.png
	-display images/diff??.png

