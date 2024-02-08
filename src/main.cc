#define SHOW_CONSOLE
#include "graphics.h"
#include <iostream>
#include <algorithm>
#include <ranges>
#include <string_view>
using namespace std::literals;

#include "core.h"

#include <span>

#include "libxbmp_extend.hpp"
#include "libxbmp.hpp"


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
	FT_Int32 flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_RENDER_MODE_NORMAL;

	if (font_info->mono) {
		flags |= FT_LOAD_TARGET_MONO;
	}
	/* 设置字体大小 */
	FT_Set_Char_Size(font_info->face, 0, font_size * 64, 0, 72);
	//FT_Set_Pixel_Sizes(font_info->face, 0, font_size);

	/* 通过编码加载字形并将其转化为位图（保存在face->glyph->bitmap中） */
	if (!FT_Load_Char(font_info->face, c, flags)) {
		glyf = font_info->face->glyph;
		FT_Get_Glyph(glyf, &glyph);

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


bool key[16] {false};               // 锟斤拷一锟斤拷扫锟借被锟斤拷锟铰的帮拷锟斤拷
bool key_pressed_flag[16] {false};  // 锟斤拷锟轿帮拷锟斤拷

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
	// static int t = clock();
	draw_screen(ege_draw_image, 100, 100, 128, 128, 4);

	ege::setcolor(EGERGB(255, 255, 255));
	ege::xyprintf(10, 10, "FPS: %.1f", ege::getfps());
	// ege::xyprintf(10, 10, "FPS: %05.1f", 1000.0/(clock()-t));
	// t = clock();
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
	float   font_size = 16;  /* 设置字体大小 */
	font_ft_get_glyph(&font_info, ch, font_size, &g);  /* 获取字模 */

/* 打印字模信息 */
	int i = 0, j = 0;
	if (g.format == GLYPH_FMT_MONO) {
		for (j = 0; j < g.h; ++j) {
			for (i = 0; i < g.w; ++i) {
				uint8_t pixel = bitmap_mono_get_pixel(g.data, g.w, g.h, i, j);
				putpixel(x+i, y+g.y+j, pixel ? (EGERGB(255,255,255)) : (EGERGB(0,0,0)));
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

	return g.advance;
}

void drawFreeType_str(int x, int y, std::span<wchar_t> strw) {

	for (int i = 0; i < strw.size(); i++) {
		x += drawFreeType_char(x, y, strw[i]) + 2;
		if (x > 600) {
			x = 40;
			y += 36;
		}
	}
}

int main (int argc, char* argv[]) {
	ege::setinitmode (INIT_RENDERMANUAL);
	ege::initgraph (700, 700);
	ege::setbkmode (TRANSPARENT);

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

	load_font("./font/WenQuanYi Bitmap Song 16px.ttf");
	// wchar_t strw[] = L"The quick brown fox jumps over the lazy dog";
	wchar_t strw[] = L"你好世界";



	ege_draw_image = ege::newimage(128, 128);

	dice.setPic(&screen_pic).setup();
	spider_web.setPic(&screen_pic).setup();

	int frames_count = 0;

	for (;ege::is_run();frames_count++, after_frames()) {


		screen_pic.clear();

		drawFreeType_str(40, 40, strw);
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

		setFont(font_fixedsys);
		std::string str;
		for (int i = 0; i < 95; i++) {
			// str += "Hello World"sv;
			// str += "B"sv;
			str += (char)(' '+i);
		}
		screen_pic.setColor(0xf);
		// drawStr(0, 0, str.c_str());
		screen_pic.setColor(0xa);
		// drawGBK(hz16, 0, 77, "");
		screen_pic.setColor(0xf);
		// char str [] {"\xc4\xe3\xba\xc3\xca\xc0\xbd\xe7"};
		drawGBK(0, 0, test_str_1);

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
