#define SHOW_CONSOLE
#include "graphics.h"
#include <iostream>
#include "core.h"


#include "libxbmp_extend.hpp"
#include "libxbmp.hpp"


bool key[16] {false};               // 这一次扫描被按下的按键
bool key_pressed_flag[16] {false};  // 单次按键

void key_input(void) {
	// 0x1
	if (ege::kbhit()) {
		switch (ege::getch()) {
			case '1': key[0x1] = true; break;
			case '2': key[0x2] = true; break;
			case '3': key[0x3] = true; break;
			case '4': key[0xC] = true; break;
			case 'q': key[0x4] = true; break;
			case 'w': key[0x5] = true; break;
			case 'e': key[0x6] = true; break;
			case 'r': key[0xD] = true; break;
			case 'a': key[0x7] = true; break;
			case 's': key[0x8] = true; break;
			case 'd': key[0x9] = true; break;
			case 'f': key[0xE] = true; break;
			case 'z': key[0xA] = true; break;
			case 'x': key[0x0] = true; break;
			case 'c': key[0xB] = true; break;
			case 'v': key[0xF] = true; break;
		}
	}
}



bool key_pressed_func (int id_of_key, bool _pressed_key_ = true) {
	if (key[id_of_key] && !key_pressed_flag[id_of_key]) {
		key_pressed_flag[id_of_key] = _pressed_key_;
		return true;
	} else {
		return false;
	}
}
#include "base64.hpp"
char base64_in[] {"Hello World!"};
uint8_t base64_out[tbz::base64::encode_out_size(sizeof(base64_in))];

uint8_t screen_buffer[128*128/2+2] = { 0x7f, 0x7f };
tbz::PIC screen_pic(screen_buffer, [](tbz::PIC& pic) {
	pic.setMode(tbz::PIC::MODE::BIT4).setColor(0x01);
});



// #include "qrcode.hpp"
// tbz::QRCode<3> qrcode;

#include "spin_dice.hpp"
tbz::SPIN_DICE dice;

#include "moving_spider.hpp"
tbz::moving_spider spider_web;



//* ege port
uint8_t ege_buffer[128*128*4];
ege::PIMAGE ege_draw_image = nullptr;
void draw_screen(void) {
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			// ege_buffer [i*128+j] = 0xffffffff;
			putpixel(j, i, ((uint32_t*)ege_buffer)[i*128+j], ege_draw_image);
		}
	}
}
void draw_pimage_with_rectangle(ege::PIMAGE img, int x, int y, int w, int h, int zoom = 1) {
	// ege::rectangle(x-1, y-1, x+1+w, y+1+h);
	// ege::putimage(x, y, img);
	ege::rectangle(x-1, y-1, x+w*zoom+1, y+h*zoom+1);
	ege::putimage(x, y, w*zoom, h*zoom, img, 0, 0, ege::getwidth(img), ege::getheight(img));
}


int main (int argc, char* argv[]) {
	// 手动刷新模式
	ege::setinitmode (INIT_RENDERMANUAL);
	// 界面分辨率
	ege::initgraph (700, 700);
	ege::setbkmode (TRANSPARENT);
	std::cout << "Hello World";
	printf ("%d", 123123);



	ege_draw_image = ege::newimage(128, 128);

	dice.setPic(&screen_pic).setup();
	spider_web.setPic(&screen_pic).setup();
	// qrcode.setPIC(&screen_pic).setContent("Hello World!");

	// qrcode.draw();


	for (;ege::is_run();ege::delay_fps(60), ege::cleardevice()) {



		dice.draw();
		screen_pic.fade_clear();
		// screen_buffer[0] = 0xff;
		spider_web.draw();


		bit4_to_bit32(screen_buffer+2, (uint8_t*)ege_buffer, 128, 128, 128, 128);
		draw_screen();
		draw_pimage_with_rectangle(ege_draw_image, 100, 100, 128, 128, 4);

		ege::setcolor(EGERGB(255, 255, 255));
		ege::xyprintf(10, 10, "FPS: %.1f", ege::getfps());
	}


	ege::closegraph();

	return 0;
}
