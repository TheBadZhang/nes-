#define SHOW_CONSOLE
#include "graphics.h"

#include <iostream>
#include <algorithm>
#include <ranges>
#include <string_view>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <format>
#include <span>

using namespace std::literals;

#include "core.h"


#include "libxbmp_extend.hpp"
#include "libxbmp.hpp"


#include "u8g2_port_to_ege.hpp"
#include "freetype.hpp"
#include "key_process.hpp"

int fps_count0 = 0;
int fps_count = 30;

#include "base64.hpp"
char base64_in[] {"Hello World!"};
uint8_t base64_out[tbz::base64::encode_out_size(sizeof(base64_in))];
std::hash<const char*> hash_fn;

uint8_t screen_buffer[128*128/2+2] = { 0x80, 0x80 };
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
	// ege::rectangle(x-1, y-1, x+w*zoom+1, y+h*zoom+1);
	ege::putimage(x, y, w*zoom, h*zoom, img, 0, 0, ege::getwidth(img), ege::getheight(img));
}
void after_frames(void) {
	// static int t = clock();
	// draw_screen(ege_draw_image, 100, 100, 128, 128, 4);

	key_input();
	ege::setcolor(EGERGB(255, 255, 255));
	std::string title = std::format("NESPP - FPS: {: >4.1f}", ege::getfps());
	ege::setcaption(title.c_str());
	ege::delay_fps(60);
	ege::cleardevice();
}

#include "font_base.hpp"

extern "C" const uint8_t test_str_1[];

	ft_fontinfo   font_info;         /* 字库信息 */
	long int      size = 0;          /* 字库文件大小 */
	unsigned char *font_buf = NULL;  /* 字库文件数据 */

int load_font(std::string path) {

	/* 加载字库文件存入font_buf */
	FILE *font_file = fopen(path.c_str(), "rb");
	if (font_file == NULL) {
		printf("Can not open font file!\n");
		getchar();
		return 0;
	}
	fseek(font_file, 0, SEEK_END); /* 设置文件指针到文件尾，基于文件尾偏移0字节 */
	size = ftell(font_file);       /* 获取文件大小（文件尾 - 文件头  单位：字节） */
	fseek(font_file, 0, SEEK_SET); /* 重新设置文件指针到文件头 */

	font_buf = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(font_buf, size, 1, font_file);
	fclose(font_file);

	font_info.mono = 1;  /* 设置为二值化模式 */

	/* 初始化FreeType */
	FT_Init_FreeType(&(font_info.library));

	/* 从font_buf中提取外观 */
	FT_New_Memory_Face(font_info.library, font_buf, size, 0, &(font_info.face));

	/* 设置字体编码方式为UNICODE */
	FT_Select_Charmap(font_info.face, FT_ENCODING_UNICODE);

}
int drawFreeType_char(int x, int y, wchar_t ch) {
	glyph_t g;
	// wchar_t c = L'a';
	float   font_size = 18;  /* 设置字体大小 */
	font_ft_get_glyph(&font_info, ch, font_size, &g);  /* 获取字模 */

/* 打印字模信息 */
	int i = 0, j = 0;
	if (g.format == GLYPH_FMT_MONO) {
		for (j = 0; j < g.h; ++j) {
			for (i = 0; i < g.w; ++i) {
				uint8_t pixel = bitmap_mono_get_pixel(g.data, g.w, g.h, i, j);
				// putpixel(x+i, y+g.y+j, pixel ? (EGERGB(255,255,255)) : (EGERGB(0,0,0)));
				if (pixel) screen_pic.drawPixel(x+i, y+g.y+j);
			}
		}
	} else if (g.format == GLYPH_FMT_ALPHA) {
		for (j = 0; j < g.h; ++j) {
			for (i = 0; i < g.w; ++i) {
				uint8_t pixel = g.data[j*g.w + i];
				putpixel (x+i, y+g.y+j, EGERGB(pixel, pixel, pixel));
			}
		}
	}

	return g.w;
}

void drawFreeType_str(int x, int y, std::span<wchar_t> strw) {
	int w = 0;
	for (int i = 0; i < strw.size(); i++) {
		if (x > 128-w) {
			x = 0;
			y += 16 + 2;
		}
		w = drawFreeType_char(x, y, strw[i]) + 1;
		x += w;
	}
}


#include "hanoi.hpp"
tbz::game::hanoi hanoi;

#include "snake.hpp"
tbz::game::SNAKE snake;

#include "round_watch_face.hpp"
tbz::round_watch_face rwf;
#include "square_watch.hpp"
tbz::SquareWatch sw;
#include "modern_art_generator.hpp"
tbz::MODERN_ART_GENERATOR mag;
// #include "sound_wave.hpp"
#include "animation.hpp"
extern "C" const uint8_t snow_animation_pic[];
tbz::SPRITE_ANIMATION<10> ani1(snow_animation_pic, 72, 8, 8, 8, 128, 128, 9);
tbz::SPRITE_ANIMATION<10> ani2(snow_animation_pic+72, 54, 8, 8, 8, 128, 128, 6);
#include "app_selector.hpp"
tbz::APP_SELECTOR app_selector;


int show_keyboard(tbz::PIC& pic) {
	int pressed_key_count = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 5; j++) {
			int index_of_res = 2+(i*5+j)*2*16;
			if (key[i*5+j]) {
				if (pressed_key_count < 6)
					keyboard[2+pressed_key_count] = 4+5*i+j;
				pressed_key_count ++;

				int offset = (16/8)*16*32;
				pic.draw1BitXBMP2x(8+22*j, 30+22*i, 16, 16, normal_keys+index_of_res+offset);
			} else {
				keyboard[2+pressed_key_count] = 0;
				pic.draw1BitXBMP2x(8+22*j, 30+22*i, 16, 16, normal_keys+index_of_res);
			}
		}
	}

	return pressed_key_count;
}

U8G2_SSD1327_MIDAS_128X128_f_4W_HW_SPI u8g2(U8G2_R0);

void draw_screen2(ege::PIMAGE pic, int x, int y, int w, int h, int zoom = 1) {
	trans_u8g2buffer_to_4bitxbmp(u8g2.getBufferPtr(), screen_buffer+2, 128, 128);
	draw_screen(pic, x, y, w, h, zoom);
	cleardevice(pic);
	u8g2.clearBuffer();
	screen_pic.clear();
}

// #include "qrcode.hpp"


enum class WINDOW {
	unexist_scene,   // 不存在的场景
	fade_animation,
	layer_in_animation,
	layer_out_animation,
	window_start_animation,
	alert,

	normal_status

} now_scene = WINDOW::normal_status, next_scene = WINDOW::unexist_scene;

// u8g2_font_NokiaSmallBold_tf

void fade_to_next_scene(APP_ENUM app) {
	// swap_animation(u8g2);
	next_scene = now_scene;
	now_scene = WINDOW::fade_animation;
	now_app = app;
}
void next_scene_func(WINDOW scene) {
	now_scene = scene;
}

int get_sys_hour(void) {
	return time(nullptr)/3600%24;
}
int get_sys_min(void) {
	return time(nullptr)/60%60;
}
int get_sys_sec(void) {
	return time(nullptr)%60;
}


#include "list_selector.hpp"

tbz::LIST_SELECTOR list_selector;

#include "calculator.hpp"
tbz::calculator calc;

int main (int argc, char* argv[]) {
	ege::setinitmode (INIT_RENDERMANUAL);
	ege::initgraph (640, 640);
	ege::setbkmode (TRANSPARENT);
	srand(time(nullptr));
	// FT_LIBRARY library;                         //FreeType库的句柄
	// FT_Error error = FT_Init_FreeType( &library );
	// if ( error )
	// {  /* 初始化失败 */  }

	// FT_Face face;                        //FT_Face对象的句柄
	// FT_New_Face( library, "/usr/share/fonts/truetype/arial.ttf", 0, &face);

	// FT_Face face;                        //FT_Face对象的句柄
	// FT_New_Memory_Face(font_info.library, font_buf, size, 0, &face);

	// FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	// FT_Set_Char_Size( face, 36 * 64, 0, 96 , 0);    //0表示与另一个尺寸值相等。

	// wchar_t char_code = L'A';
	// FT_UInt glyph_index = FT_Get_Char_Index(face, char_code);     /* 若 glyph_index 为0，表示没找到字形索引 */

	// FT_Load_Glyph(face, glyph_index, load_flags); /* load_flags：装载标志，一般填FT_LOAD_DEFAULT */

	// FT_Render_Glyph(face->glyph, render_mode); /* render_mode：渲染模式 */

	// FT_Load_Char(face, char_code, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);

	load_font("./font/wqy/WenQuanYi Bitmap Song 16px.ttf");
	// wchar_t strw[] = L"The quick brown fox jumps over the lazy dog";
	wchar_t strw[] = L"Hello World!你好,世界。The quick brown fox jumps over the lazy dog我能吞下玻璃而不伤身体Innovation in China中国智造慧及全球";

	ege_draw_image = ege::newimage(128, 128);

	hanoi.set_U8G2(&u8g2);
	snake.set_U8G2(&u8g2);
	rwf.set_U8G2(&u8g2);
	sw.set_U8G2(&u8g2);
	mag.set_U8G2(&u8g2);
	mag.random_to_next();
	dice.setup().setPic(screen_pic);
	spider_web.setPic(&screen_pic).setup();
	// sound_wave.setPic(&screen_pic).setup();
	list_selector.set_U8G2(&u8g2);
	app_selector.set_U8G2(&u8g2);
	app_selector.setPIC(&screen_pic);
	// app_selector.setTime(sTime);
	calc.setup().setPic(screen_pic);


	// tbz::QRCode<3> qrcode;
	// qrcode.setPic(&screen_pic).setContent("Hello, World!");

	ani1.set_U8G2(&u8g2);
	ani2.set_U8G2(&u8g2);
	int frames_count = 0;

	for (;ege::is_run();frames_count++, after_frames()) {

		// 最高层级的弹窗
		if (key_pressed_func(4)) {
			switch(now_scene) {
				default: {
					next_scene = now_scene;
					now_scene = WINDOW::alert;
				} break;
				case WINDOW::alert: {
					now_scene = next_scene;
				}
			}
		}


		// 退出 APP 的动画
		if (key_pressed_func(9)) {
			if (now_app != APP_ENUM::main) {
				next_app = APP_ENUM::main;
				fade_to_next_scene(next_app);
				app_selector.slide_in();
			}
		}


		// screen_pic.clear();
		screen_pic.fade_clear2();
		// 当前场景（部分动画或者弹窗依赖此状态）
		switch (now_scene) {
			case WINDOW::fade_animation: {
				tbz::animation::slide(&u8g2, now_scene, next_scene);
			} break;
			case WINDOW::alert: {
				tbz::ui::menu::alert::draw(&u8g2);
			} break;

			case WINDOW::normal_status: {
				switch (now_app) {
					case APP_ENUM::main: {
						if (key_pressed_func(0)) {
							app_selector.select_prev_app();
						}
						if (key_pressed_func(10)) {
							app_selector.select_next_app();
						}
						if (key_pressed_func(5)) {
							next_app = static_cast<APP_ENUM>(tbz::APP::now_select_app_id);
							fade_to_next_scene(next_app);
						}
						app_selector.setFPS(fps_count/0.5);
						app_selector.draw();
					} break;
					case APP_ENUM::SNAKE_GAME: {
						if (key_pressed_func(1)) {
							snake.move_left();
						}
						if (key_pressed_func(11)) {
							snake.move_right();
						}
						if (key_pressed_func(6)) {
							snake.move_down();
						}
						if (key_pressed_func(7)) {
							snake.move_up();
						}
						if (key_pressed_func(2)) {
							snake.speed_down();
						}
						if (key_pressed_func(12)) {
							snake.speed_up();
						}
						snake.game();
					} break;
					case APP_ENUM::ui_test3: {
						u8g2.drawBox(0,0,20,20);
						u8g2.drawBox(20,20,20,20);
						u8g2.drawFrame(10,40,20,20);
						u8g2.setFont(u8g2_font_DigitalDiscoThin_tf);
						sprintf(buf,"%d",114514);
						u8g2.drawStr(0,20,buf);
					} break;
					// case APP_ENUM::qrcode_test: {
					// 	qrcode.draw();
					// } break;
					case APP_ENUM::animation1: {
						alert_font = u8g2_font_wqy16_t_gb2312;
						sprintf(alert_message, "你好，世界");
						u8g2.setFont(u8g2_font_wqy16_t_gb2312);
						int w = u8g2.getUTF8Width(alert_message);
						int h = u8g2.getMaxCharHeight();
						u8g2.drawUTF8(64-w/2, 64-h/2, "你好，世界");
						ani1.draw();
						ani2.draw();
					} break;
					// case APP_ENUM::adc_animation: {
					// 	// 动画2
					// 	rwf.draw(adc_value2[0]*1000/65536);
					// 	sound_wave.draw();
					// } break;
					case APP_ENUM::animation3: {
						// 动画3
						sw.draw(get_sys_hour(), get_sys_min(), get_sys_sec());
					} break;
					case APP_ENUM::HANOI_GAME: {
						if (key_pressed_func(1)) {
							hanoi.cursor_left();
						}
						if (key_pressed_func(11)) {
							hanoi.cursor_right();
						}
						if (key_pressed_func(6)) {
							hanoi.cursor_select();
						}
						alert_font = u8g2_font_wqy16_t_gb2312;
						sprintf(alert_message, "汉诺塔");
						hanoi.start_scene();
					} break;
					case APP_ENUM::WELCOM_SCENE: {
						alert_font = u8g2_font_NokiaSmallBold_tf;
						sprintf(alert_message, "WELCOM TO PLAY!");
						hanoi.welcom_scene();
					} break;
					case APP_ENUM::art_generator: {
						if (key_pressed_func(0)) {
							mag.random_to_next();
						}
						mag.draw();
					} break;
					case APP_ENUM::ui_test: {
						if (key_pressed_func(5, false)) {
							list_selector.select_next_item();
						}
						if (key_pressed_func(6, false)) {
							list_selector.select_prev_item();
						}
						list_selector.draw();
					} break;
					case APP_ENUM::key_test: {
						alert_font = u8g2_font_NokiaSmallBold_tf;
						sprintf(alert_message, "Hello World!");
						u8g2.clearBuffer();
						u8g2.setFont(u8g2_font_6x10_tf);
						// u8g2.setContrast(25);
						u8g2.drawStr(0, 10, "Hello World!");

						auto pressed_key_count = show_keyboard(screen_pic);

						sprintf (buf, "pressed:%d", pressed_key_count);
						u8g2.drawStr(0, 20, buf);
						sprintf (buf, "hash:%x", hash_fn("hello world!"));
						u8g2.drawStr(0, 110, buf);
						u8g2.setFont(u8g2_font_6x10_tf);
						u8g2.drawStr(0, 120, (const char*)base64_out);

					} break;
					case APP_ENUM::ui_test2: {
						u8g2.clearBuffer();
						for (int i = 0; i < 128; i++) {
							for (int j = 0; j < 128; j++) {
								if (i < 64 || i > 128 || j < 20 || j > 44)
								if ((i^j)&1) u8g2.drawPixel(i, j);
							}
						}
						// u8g2.drawLine(64, 20, 128, 20);
						// u8g2.drawLine(64, 20, 64, 44);
						// u8g2.drawLine(128,20, 128,44);
						// u8g2.drawLine(64,44,128,44);
						u8g2.drawStr(33,38, "WARNING");
						// u8g2.drawRBox()
						// u8g2.drawBox(64, 20, 64, 24);
					} break;
					case APP_ENUM::MINESWEEPER_GAME: {
						// 绘制图片的时候应该考虑图源是多少bit的
						// 通常来说不直接使用 drawXBMP，会导致图片无法正常显示
						screen_pic.draw1BitXBMP(0, 0, psychic_swamp);
						screen_pic.draw4BitXBMP(0, 0, tsetBones);
						screen_pic.draw4BitXBMP(64, 0, tsetLava);
						screen_pic.draw4BitXBMP(0, 64, tsetSand);
						screen_pic.draw4BitXBMP(64, 64, tsetTower);
					} break;
					case APP_ENUM::TETRIS_GAME: {
						dice.draw();
					} break;
					// case APP_ENUM::streamer: {
					// 	streamer.draw();
					// } break;
					case APP_ENUM::calculator: {
						calc.draw();
					} break;
					default: {
						u8g2.clearBuffer();
						u8g2.setFont(u8g2_font_wqy16_t_gb2312);
						sprintf(buf, "-无信号-");
						int str_w = u8g2.getUTF8Width(buf);
						u8g2.drawUTF8(64-str_w/2, 64+16/2, buf);
					} break;
				}
			} break;


			// case WINDOW::unexist_scene:
			default: {
				now_scene = WINDOW::normal_status;
			} break;
		}


		screen_pic.setColor(0x3);
		ani1.draw2(screen_pic);
		ani2.draw2(screen_pic);

		spider_web.draw();

		// qrcode.draw();
		// u8g2.setFont(u8g2_font_wqy16_t_gb2312);
		// // u8g2.drawUTF8(10, 20, "你好世界Innovation in China");
		// cleardevice(ege_draw_image);
		// screen_pic.clear();
		// u8g2.clearBuffer();
		// screen_pic.setColor(0xf);
		// dice.draw();
		// draw_screen2(ege_draw_image, 100, 100, 128, 128, 1);
		// spider_web.draw();
		// draw_screen2(ege_draw_image, 228, 100, 128, 128, 1);
		// screen_pic.setColor(0x8);
		// hanoi.start_scene();
		// draw_screen2(ege_draw_image, 356, 100, 128, 128, 1);
		// screen_pic.setColor(0xa);
		// snake.game();
		// draw_screen2(ege_draw_image, 484, 100, 128, 128, 1);
		// rwf.draw(500);
		// draw_screen2(ege_draw_image, 100, 228, 128, 128, 1);
		// sw.draw(10, 10, 10);
		// draw_screen2(ege_draw_image, 228, 228, 128, 128, 1);
		// mag.draw();
		// draw_screen2(ege_draw_image, 356, 228, 128, 128, 1);
		// ani1.draw();
		// ani2.draw();
		// draw_screen2(ege_draw_image, 484, 228, 128, 128, 1);
		// app_selector.draw();
		// draw_screen2(ege_draw_image, 100, 356, 128, 128, 1);
		// screen_pic.setColor(0xf);
		// drawFreeType_str(0, 16, strw);
		// draw_screen2(ege_draw_image, 228, 356, 128, 128, 1);
		// show_keyboard(screen_pic);
		// draw_screen2(ege_draw_image, 356, 356, 128, 128, 1);
		// screen_pic.drawXBMP(0, 0, 64, 64, tsetBones+2);
		// screen_pic.drawXBMP(64, 0, 64, 64, tsetLava+2);
		// screen_pic.drawXBMP(0, 64, 64, 64, tsetSand+2);
		// screen_pic.drawXBMP(64, 64, 64, 64, tsetTower+2);

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

		// setFont(font_fixedsys);
		// std::string str;
		// for (int i = 0; i < 95; i++) {
		// 	// str += "Hello World"sv;
		// 	// str += "B"sv;
		// 	str += (char)(' '+i);
		// }
		// screen_pic.setColor(0xf);
		// // drawStr(0, 0, str.c_str());
		// screen_pic.setColor(0xa);
		// // drawGBK(hz16, 0, 77, "");
		// screen_pic.setColor(0xf);
		// char str [] {"\xc4\xe3\xba\xc3\xca\xc0\xbd\xe7"};
		// screen_pic.drawGBK(0, 0, (uint8_t*)str);
		// draw_screen2(ege_draw_image, 484, 356, 128, 128, 1);

		draw_screen2(ege_draw_image, 0, 0, 128, 128, 5);
		fps_count0 ++;
		// screen_pic.drawRBox(10, 10, 20, 20, 3);

		// char c = ' ';
		// for (int i = 0; i < 40; i++) {
		// 	c ++;
		// 	int blen = g_font -> width*((g_font -> height+7)/8);
		// 	// screen_pic.draw1BitXBMP2(0, 0, 5, 7, font5x7.data+5*(c-' '));
		// 	drawGlyph(0, 0, c);
		// }
		// drawGlyph(0, 0, '!');

	}
		/* 释放资源 */
	// FT_Done_Glyph((FT_Glyph)g.handle);
	// FT_Done_FreeType(font_info.library);
	free(font_buf);

	ege::closegraph();

	return 0;
}
