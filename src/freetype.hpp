
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftpfr.h>
#include <freetype/ftadvanc.h>

#include <cstdint>

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