#define SHOW_CONSOLE
#include "graphics.h"
#include <iostream>
#include <algorithm>
#include <ranges>
#include <string_view>
using namespace std::literals;

#include "core.h"


#include "libxbmp_extend.hpp"
#include "libxbmp.hpp"




bool key[16] {false};               // ��һ��ɨ�豻���µİ���
bool key_pressed_flag[16] {false};  // ���ΰ���

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


#include "spin_dice.hpp"
tbz::SPIN_DICE dice;

#include "moving_spider.hpp"
tbz::moving_spider spider_web;

extern "C" const uint8_t psychic_swamp[];
extern "C" const uint8_t tsetBones[];
extern "C" const uint8_t tsetLava[];
extern "C" const uint8_t tsetSand[];
extern "C" const uint8_t tsetTower[];
extern "C" const uint8_t normal_keys[];


//* ege port
uint8_t ege_buffer[128*128*4];

ege::PIMAGE ege_draw_image = nullptr;
void draw_screen(ege::PIMAGE img, int x, int y, int w, int h, int zoom = 1) {

	bit4_to_bit32(screen_buffer+2, (uint8_t*)ege_buffer, 128, 128, 128, 128);

	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			// ege_buffer [i*128+j] = 0xffffffff;
			putpixel(j, i, ((uint32_t*)ege_buffer)[i*128+j], ege_draw_image);
		}
	}
	// ege::rectangle(x-1, y-1, x+1+w, y+1+h);
	// ege::putimage(x, y, img);
	ege::rectangle(x-1, y-1, x+w*zoom+1, y+h*zoom+1);
	ege::putimage(x, y, w*zoom, h*zoom, img, 0, 0, ege::getwidth(img), ege::getheight(img));
}
void after_frames(void) {
	draw_screen(ege_draw_image, 100, 100, 128, 128, 4);

	ege::setcolor(EGERGB(255, 255, 255));
	ege::xyprintf(10, 10, "FPS: %.1f", ege::getfps());
	ege::delay_fps(60);
	ege::cleardevice();
}


#include "font.hpp"

const ASCII_CHAR* g_font;

void setFont(const ASCII_CHAR& font) {
	g_font = &font;
}

// void drawGlyph(int x, int y, const uint8_t* glyph) {
// 	// screen_pic.setColor(0xf);
// 	int w = g_font -> width;
// 	int h = g_font -> height;
// 	screen_pic.draw1BitXBMP2(x, y, w, h, glyph);
// }
void drawGlyph(int x, int y, const char ch) {
	int w = g_font -> width;
	int h = g_font -> height;
	switch (g_font -> mode) {
		case FONT_MODE_0: {
			int blen = w*((h+7)/8);
			const uint8_t* ptr = g_font -> data+((ch-' ')*blen);
			screen_pic.draw1BitXBMP0(x, y, w, h, ptr);
		} break;
		case FONT_MODE_1: {
			int blen = (w+7)/8*h;
			const uint8_t* ptr = g_font -> data+((ch-' ')*blen);
			screen_pic.draw1BitXBMP1(x, y, w, h, ptr);
		} break;
		case FONT_MODE_2: {
			int blen = w*((h+7)/8);
			const uint8_t* ptr = g_font -> data+((ch-' ')*blen);
			screen_pic.draw1BitXBMP2(x, y, w, h, ptr);
		} break;
		case FONT_MODE_3: {
			int blen = (w+7)/8*h;
			const uint8_t* ptr = g_font -> data+((ch-' ')*blen);
			if (g_font->reverse) {
				// screen_pic.draw1BitXBMP310(x, y, w, h, ptr);
				screen_pic.draw1BitXBMP301(x, y, w, h, ptr);
			} else {
				screen_pic.draw1BitXBMP3(x, y, w, h, ptr);
			}
		} break;
	}
}

void drawStr(int x, int y, const char str[], bool auto_newline = true) {
	int w = g_font -> width;
	int h = g_font -> height;
	if (x < -w || x > 128 || y < -h || y > 128) return;
	screen_pic.setColor(0xf);

	for (int y_ = y, x_ = x;*str; str++) {
		if (auto_newline) {
			if (x_ > 128-w) {
				x_ = 0;
				y_ += (h + (g_font -> spacing_y));
			}
		}
		if (x_ > 128) break;
		if (y_ > 128) break;
		drawGlyph(x_, y_, *str);
		x_ += (w + (g_font -> spacing_x));
	}
}

template<typename T>
void drawGBK (const T& font, int x, int y, const char str[], bool auto_newline = true) {

	for (int i = 0; i < 20; i++) {
		int x_ = x + i%8 * 16;
		int y_ = y + i/8 * 16;
		screen_pic.draw1BitXBMP310(x_, y_, 16, 16, font[i].mask);

	}
}

int main (int argc, char* argv[]) {
	// �ֶ�ˢ��ģʽ
	ege::setinitmode (INIT_RENDERMANUAL);
	// ����ֱ���
	ege::initgraph (700, 700);
	ege::setbkmode (TRANSPARENT);



	ege_draw_image = ege::newimage(128, 128);

	dice.setPic(&screen_pic).setup();
	spider_web.setPic(&screen_pic).setup();

	int frames_count = 0;

	for (;ege::is_run();frames_count++, after_frames()) {


		screen_pic.clear();


		// screen_pic.drawXBMP(0, 0, 64, 64, tsetBones+2);
		// screen_pic.drawXBMP(64, 0, 64, 64, tsetLava+2);
		// screen_pic.drawXBMP(0, 64, 64, 64, tsetSand+2);
		// screen_pic.drawXBMP(64, 64, 64, 64, tsetTower+2);
		// dice.draw();
		// spider_web.draw();

		// screen_pic.setColor(0xf);
		// screen_pic.draw1BitXBMP(0, 0, 12, 12, tfont12[0].mask);
		// screen_pic.draw1BitXBMP(12, 0, 12, 12, tfont12[1].mask);
		// screen_pic.draw1BitXBMP(24, 0, 12, 12, tfont12[2].mask);
		// screen_pic.draw1BitXBMP(36, 0, 12, 12, tfont12[3].mask);
		// screen_pic.draw1BitXBMP(48, 0, 12, 12, tfont12[4].mask);

		// screen_pic.draw1BitXBMP(0, 13, 16, 16, tfont16[0].mask);
		// screen_pic.draw1BitXBMP(16, 13, 16, 16, tfont16[1].mask);
		// screen_pic.draw1BitXBMP(32, 13, 16, 16, tfont16[2].mask);
		// screen_pic.draw1BitXBMP(48, 13, 16, 16, tfont16[3].mask);
		// screen_pic.draw1BitXBMP(64, 13, 16, 16, tfont16[4].mask);

		// screen_pic.draw1BitXBMP(0, 30, 24, 24, tfont24[0].mask);
		// screen_pic.draw1BitXBMP(24, 30, 24, 24, tfont24[1].mask);
		// screen_pic.draw1BitXBMP(48, 30, 24, 24, tfont24[2].mask);
		// screen_pic.draw1BitXBMP(72, 30, 24, 24, tfont24[3].mask);
		// screen_pic.draw1BitXBMP(96, 30, 24, 24, tfont24[4].mask);

		// screen_pic.draw1BitXBMP(0, 55, 32, 32, tfont32[0].mask);
		// screen_pic.draw1BitXBMP(32, 55, 32, 32, tfont32[1].mask);
		// screen_pic.draw1BitXBMP(64, 55, 32, 32, tfont32[2].mask);
		// screen_pic.draw1BitXBMP(96, 55, 32, 32, tfont32[3].mask);
		// screen_pic.draw1BitXBMP(128, 55, 32, 32, tfont32[4].mask);

		setFont(font7x10);
		std::string str;
		for (int i = 0; i < 95; i++) {
			// str += "Hello World"sv;
			// str += "B"sv;
			str += (char)(' '+i);
		}
		screen_pic.setColor(0xf);
		drawStr(0, 0, str.c_str());

		drawGBK(hz16, 0, 77, "");

		screen_pic.drawRBox(10, 10, 20, 20, 3);

		// char c = ' ';
		// for (int i = 0; i < 40; i++) {
		// 	c ++;
		// 	int blen = g_font -> width*((g_font -> height+7)/8);
		// 	// screen_pic.draw1BitXBMP2(0, 0, 5, 7, font5x7.data+5*(c-' '));
		// 	drawGlyph(0, 0, c);
		// }
		// drawGlyph(0, 0, '!');

	}

	ege::closegraph();

	return 0;
}
