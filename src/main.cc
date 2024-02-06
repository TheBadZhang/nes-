#define SHOW_CONSOLE
#include "graphics.h"
#include <iostream>


int main (int argc, char* argv[]) {
	// 手动刷新模式
	ege::setinitmode (INIT_RENDERMANUAL);
	// 界面分辨率
	ege::initgraph (800, 600);
	ege::setbkmode (TRANSPARENT);
	std::cout << "Hello World";

	printf ("%d", 123123);
	for (;ege::is_run();ege::delay_fps(60)) {
		ege::cleardevice();

		ege::setcolor(EGERGB(255, 255, 255));
		ege::xyprintf(10, 10, "FPS: %.1f", ege::getfps());
	}
	getch();
	ege::closegraph();
	return 0;
}
