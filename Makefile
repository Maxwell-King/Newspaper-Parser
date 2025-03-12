all: annotator run

annotator: src/annotator.cpp
	g++ `wx-config --cflags` -o annotator src/jsdata.cpp src/dialog.cpp src/imgpanel.cpp src/annotator.cpp `wx-config --libs` `pkg-config --libs tesseract` `pkg-config --libs lept`

run: src/entry.c
	gcc -o run src/entry.c src/filesys_interaction.c