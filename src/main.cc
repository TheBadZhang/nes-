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
using namespace std::literals;

#include "core.h"

#include <span>

#include "libxbmp_extend.hpp"
#include "libxbmp.hpp"

#include "u8g2.h"
#include "U8g2lib.h"


uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr) {
	return 0;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	return 0;
}

class U8G2_SSD1327_MIDAS_128X128_f_4W_HW_SPI : public U8G2 {
	public: U8G2_SSD1327_MIDAS_128X128_f_4W_HW_SPI(const u8g2_cb_t *rotation) : U8G2() {
		u8g2_Setup_ssd1327_midas_128x128_f(&u8g2, rotation, u8x8_byte_4wire_hw_spi, u8x8_gpio_and_delay);
	}
};


#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftpfr.h>
#include <freetype/ftadvanc.h>

/* 字体数据（ttf） */
typedef struct _ft_fontinfo {
	FT_Face    face;     /* FreeType库句柄对象 */
	FT_Library library;  /* 外观对象（描述了特定字样和风格，比如斜体风格等） */
	int32_t     mono;    /* 是否为二值化模式 */
} ft_fontinfo;

/* 字模格式常量定义 */
typedef enum _glyph_format_t {
	GLYPH_FMT_ALPHA, /* 每个像素占用1个字节 */
	GLYPH_FMT_MONO,  /* 每个像素占用1个比特 */
} glyph_format_t;

/* 字模（位图） */
typedef struct _glyph_t {
	int16_t  x;
	int16_t  y;
	uint16_t w;
	uint16_t h;
	uint16_t advance;  /* 占位宽度 */
	uint8_t  format;   /* 字模格式 */
	uint8_t  pitch;    /* 跨距（每行像素个数 * 单个像素所占字节数） */
	uint8_t  *data;    /* 字模数据：每个像素点占用一个字节 */
	void     *handle;  /* 保存需要释放的句柄 */
} glyph_t;


/* 获取二值化位图上像素点的值 */
uint8_t bitmap_mono_get_pixel(const uint8_t* buff, uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
	/* 计算字节偏移 */
	uint32_t line_length = ((w + 15) >> 4) << 1;
	uint32_t offset = y * line_length + (x >> 3);

	/* 计算位偏移 */
	uint32_t offset_bit = 7 - (x % 8);

	const uint8_t* data = buff + offset;
	if (buff == NULL || (x > w && y > h))
		return 0;
	return (*data >> offset_bit) & 0x1;
}

/* 获取字模 */
static int font_ft_get_glyph(ft_fontinfo *font_info, wchar_t c, float font_size, glyph_t* g) {
	FT_Glyph glyph;
	FT_GlyphSlot glyf;
	FT_Int32 flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_NO_BITMAP;

	if (font_info->mono) {
		flags |= FT_LOAD_TARGET_MONO;
	}
	/* 设置字体大小 */
	FT_Set_Char_Size(font_info->face, 0, font_size * 64, 0, 72);
	// FT_Set_Pixel_Sizes(font_info->face, 0, font_size);

	/* 通过编码加载字形并将其转化为位图（保存在face->glyph->bitmap中） */
	if (!FT_Load_Char(font_info->face, c, flags)) {
		glyf = font_info->face->glyph;
		FT_Get_Glyph(glyf, &glyph);
		// FT_Render_Glyph(glyf, FT_RENDER_MODE_NORMAL);

		g->format = GLYPH_FMT_ALPHA;
		g->h = glyf->bitmap.rows;
		g->w = glyf->bitmap.width;
		g->pitch = glyf->bitmap.pitch;
		g->x = glyf->bitmap_left;
		g->y = -glyf->bitmap_top;
		g->data = glyf->bitmap.buffer;
		g->advance = glyf->metrics.horiAdvance / 64;

		if (g->data != NULL) {
			if (glyf->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
				g->format = GLYPH_FMT_MONO;
			}
			g->handle = glyph;
		}
		else {
			FT_Done_Glyph(glyph);
		}
	}
	return g->data != NULL || c == ' ' ? 1 : 0;
}


uint8_t keyboard[8] {0,0,0,0,0,0,0,0};
bool key[16] {false};               // 锟斤拷一锟斤拷扫锟借被锟斤拷锟铰的帮拷锟斤拷
bool key_pressed_flag[16] {false};  // 锟斤拷锟轿帮拷锟斤拷

void key_input(void) {
	// 0x1
	key[0] = ege::keystate('Q');
	key[1] = ege::keystate('W');
	key[2] = ege::keystate('E');
	key[3] = ege::keystate('R');
	key[4] = ege::keystate('T');
	// 0x2
	key[5] = ege::keystate('A');
	key[6] = ege::keystate('S');
	key[7] = ege::keystate('D');
	key[8] = ege::keystate('F');
	key[9] = ege::keystate('G');
	// 0x4
	key[10] = ege::keystate('Z');
	key[11] = ege::keystate('X');
	key[12] = ege::keystate('C');
	key[13] = ege::keystate('V');
	key[14] = ege::keystate('B');

	for (int i = 0; i < 15; i++) {
		if (key[i] == false) {
			key_pressed_flag[i] = false;
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
	const uint8_t* ptr = g_font -> data;
	switch (g_font -> mode) {
		case FONT_MODE_0: {
			int blen = w*((h+7)/8);
			ptr += ((ch-' ')*blen);
			screen_pic.draw1BitXBMP0(x, y, w, h, ptr);
		} break;
		case FONT_MODE_1: {
			int blen = (w+7)/8*h;
			ptr += ((ch-' ')*blen);
			screen_pic.draw1BitXBMP1(x, y, w, h, ptr);
		} break;
		case FONT_MODE_2: {
			int blen = w*((h+7)/8);
			ptr += ((ch-' ')*blen);
			screen_pic.draw1BitXBMP2(x, y, w, h, ptr);
		} break;
		case FONT_MODE_3: {
			int blen = (w+7)/8*h;
			ptr += ((ch-' ')*blen);
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

extern "C" const uint8_t test_str_1[];
void drawGBK(int x, int y, const uint8_t* str, bool auto_newline = true) {

	// int length = 100;
	for (const uint8_t* ptr = str; *ptr != '\0'; ) {
		if (*ptr > 0xa0 && *(ptr+1) > 0xa0) {
			uint8_t gbkh = *ptr++;
			uint8_t gbkl = *ptr++;
			// // uint32_t index = (gbkh-0x81)*190 + gbkl - 6256;
			// printf ("\n0x%02x%02x\t", gbkh, gbkl);

			// // if (gbkl < 0x7f) {
			// // 	index = (index - 0x40);
			// // } else {
			// // 	index = (index - 0x41);
			// // }
			// gbkh = 0xb0;
			// gbkl = 0xa1;
			gbkh -= 0xb0;
			gbkl -= 0xa1;
			uint32_t index = ((0xff-0xa1)*gbkh + gbkl);
			// https://blog.csdn.net/anyuliuxing/article/details/84326207

			if (index > 6768) continue;
			// printf("%d\t", index);

			if (x > (128 - 16)) {
				y += 16 + sim_16x16_gb2312.spacing_y;
				x = 0;
			}

			// screen_pic.draw1BitXBMP310(x, y, 16, 16, sim_16x16_gb2312.data+index*32);
			// 这个字库只有汉字没有标点符号，所以 gbkh 要减 0xb0
			// 涉及到标点符号会使计算结果出现问题，最佳解决方案是把标点符号一起包进来
			screen_pic.draw1BitXBMP310(x, y, 16, 16, sim_16x16_gb2312.data+index*32);

			x += 16 + sim_16x16_gb2312.spacing_x;
		} else {
			uint8_t ch = *ptr++;
			if (x > (128 - g_font->width)) {
				y += 16 + g_font->spacing_y;
				x = 0;
			}
			drawGlyph(x, y, ch);
			x += g_font->width + g_font->spacing_x;
		}
	}
	// sim_16x16_gb2312
}

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

#include "tinyexpr.h"

#include "list_selector.hpp"

tbz::LIST_SELECTOR list_selector;

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
	dice.setPic(&screen_pic).setup();
	spider_web.setPic(&screen_pic).setup();
	// sound_wave.setPic(&screen_pic).setup();
	list_selector.set_U8G2(&u8g2);
	app_selector.set_U8G2(&u8g2);
	app_selector.setPIC(&screen_pic);
	// app_selector.setTime(sTime);

	// tbz::QRCode<3> qrcode;
	// qrcode.setPic(&screen_pic).setContent("Hello, World!");

	ani1.set_U8G2(&u8g2);
	ani2.set_U8G2(&u8g2);
	int frames_count = 0;

	for (;ege::is_run();frames_count++, after_frames()) {

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
		// // char str [] {"\xc4\xe3\xba\xc3\xca\xc0\xbd\xe7"};
		// drawGBK(0, 0, test_str_1);
		// draw_screen2(ege_draw_image, 484, 356, 128, 128, 1);


	// while (true) {
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
						int error;

						// int a = (int)te_interp("(5+5)", 0); // Returns 10.
						int b = (int)te_interp("(5+5)", &error); // Returns 10, error is set to 0.
						// int c = (int)te_interp("(5+5", &error);
						sprintf(buf, "(5+5)=%d", b);
						u8g2.drawStr(0, 70, buf);
					} break;
					case APP_ENUM::MINESWEEPER_GAME: {
						// draw_pic(&u8g2, 0, 0, psychic_swamp);
						// draw_pic(&u8g2, 0, 0, tsetBones);
						// screen_pic.setMode(tbz::PIC::MODE::BIT4);
						screen_pic.drawXBMP(0, 0, 64, 64, tsetBones+2);
						screen_pic.drawXBMP(64, 0, 64, 64, tsetLava+2);
						screen_pic.drawXBMP(0, 64, 64, 64, tsetSand+2);
						screen_pic.drawXBMP(64, 64, 64, 64, tsetTower+2);
					} break;
					case APP_ENUM::TETRIS_GAME: {
						dice.draw();
					} break;
					// case APP_ENUM::streamer: {
					// 	streamer.draw();
					// } break;
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

	// }

	}
		/* 释放资源 */
	// FT_Done_Glyph((FT_Glyph)g.handle);
	// FT_Done_FreeType(font_info.library);
	free(font_buf);

	ege::closegraph();

	return 0;
}
