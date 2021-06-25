#include <pspdebug.h>
#include "fontloader.h"

Font* Font_Load(const char* filename) {

        FT_Error initError;
        initError = FT_Init_FreeType( &ft_library );
  	if(initError)
          return 0;

	Font* font = (Font*) malloc(sizeof(Font));

	// Cache font
	FILE* fontFile = fopen(filename, "rb");

	if (!fontFile)
        pspDebugScreenPrintf("Can't open font file.\n");

	fseek(fontFile, 0, SEEK_END);
	int filesize = ftell(fontFile);
	u8* fontData = (u8*) malloc(filesize);
	if (!fontData) {
		fclose(fontFile);
		pspDebugScreenPrintf("Not enough memory to cache font file.\n");
	}
	rewind(fontFile);
	fread(fontData, filesize, 1, fontFile);
	fclose(fontFile);
	int error = FT_New_Memory_Face(ft_library, fontData, filesize, 0, &font->face);
	if (error) {
		free(font);
		free(fontData);
		pspDebugScreenPrintf("Error loading font.\n");
	}
	font->data = fontData;
	font->name = strdup(filename);
	return font;
}

int Font_Unload(Font* font) {
	FT_Done_Face(font->face);
	free(font->name);
	if (font->data)	free(font->data);
	free(font);
	return 0;
}

int Font_SetSize(Font* font, int height) {
	FT_Set_Pixel_Sizes(font->face, 0, height);
	return 1;
}


static void fontPrintTextImpl(FT_Bitmap* bitmap, int xofs, int yofs, u32 color, u32* framebuffer, int width, int height, int lineSize)
{
	u8 rf = color & 0xff;
	u8 gf = (color >> 8) & 0xff;
	u8 bf = (color >> 16) & 0xff;
	u8 af = (color >> 24) & 0xff;
	int x, y;
	
	u8* line = bitmap->buffer;
	u32* fbLine = framebuffer + xofs + yofs * lineSize;
	for (y = 0; y < bitmap->rows; y++) {
		u8* column = line;
		u32* fbColumn = fbLine;
		for (x = 0; x < bitmap->width; x++) {
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0) {
				u8 val = *column;
				color = *fbColumn;
				u8 r = color & 0xff; 
				u8 g = (color >> 8) & 0xff;
				u8 b = (color >> 16) & 0xff;
				u8 a = (color >> 24) & 0xff;
				r = rf * val / 255 + (255 - val) * r / 255;
				g = gf * val / 255 + (255 - val) * g / 255;
				b = bf * val / 255 + (255 - val) * b / 255;
				a = af * val / 255 + (255 - val) * a / 255;
				*fbColumn = r | (g << 8) | (b << 16) | (a << 24);
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += lineSize;
	}
}

static void fontPrintTextScreen(FT_Bitmap* bitmap, int x, int y, u32 color)
{
	fontPrintTextImpl(bitmap, x, y, color, getVramDrawBuffer(), 480, 272, 512);
}

static void fontPrintTextImage(FT_Bitmap* bitmap, int x, int y, Color color, Image* image)
{
	fontPrintTextImpl(bitmap, x, y, color, image->data, image->imageWidth, image->imageHeight, image->textureWidth);
}

int Font_Print_Screen(Font* font, int x, int y, const char* text, u32 color) {

	int num_chars = strlen(text);
	FT_GlyphSlot slot = font->face->glyph;
	int n;
	for (n = 0; n < num_chars; n++) {
		FT_UInt glyph_index = FT_Get_Char_Index(font->face, text[n]);
		int error = FT_Load_Glyph(font->face, glyph_index, FT_LOAD_DEFAULT);
		if (error) continue;
		error = FT_Render_Glyph(font->face->glyph, ft_render_mode_normal);
		if (error) continue;

			fontPrintTextScreen(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, color);

		x += slot->advance.x >> 6;
		y += slot->advance.y >> 6;
	}

	return 0;
}

int Font_Print_Image(Font* font, int x, int y, const char* text, u32 color, Image* image) {

	int num_chars = strlen(text);
	FT_GlyphSlot slot = font->face->glyph;
	int n;
	for (n = 0; n < num_chars; n++) {
		FT_UInt glyph_index = FT_Get_Char_Index(font->face, text[n]);
		int error = FT_Load_Glyph(font->face, glyph_index, FT_LOAD_DEFAULT);
		if (error) continue;
		error = FT_Render_Glyph(font->face->glyph, ft_render_mode_normal);
		if (error) continue;

			fontPrintTextImage(&slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top, color, image);

		x += slot->advance.x >> 6;
		y += slot->advance.y >> 6;
	}

	return 0;
}
