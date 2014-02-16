#include "glyph.h"
#include "towidechar.h"

sad::freetype::Glyph::Glyph()
: Index(0), Width(0), 
Height(0), BearingY(0), 
Descender(0), AdvanceX(0),
TexCoordinateWidth(0), TexCoordinateHeight(0)
{

}

sad::freetype::Glyph::Glyph(FT_Face face, unsigned char c)
{
	wchar_t widecharacter = sad::freetype::to_wide_char(c);
	Index = FT_Get_Char_Index( face, widecharacter );
	bool constructed = false;
	
	if (!FT_Load_Glyph( face, Index, FT_LOAD_DEFAULT))
	{
		FT_Glyph glyph = NULL;
		if (!FT_Get_Glyph(face->glyph, &glyph))
		{
			makeGlyph(face, glyph);
			constructed = true;
		}		
	}

	if (!constructed)
	{
		makeEmptyGlyph();
	}
}

void sad::freetype::Glyph::render(float x, float y)
{
	Texture.bind();

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f,0.0f); 
	glVertex2f(x, y + BearingY);
	
	glTexCoord2f(0.0f, TexCoordinateHeight); 
	glVertex2f(x, y + Descender);
	
	glTexCoord2f(TexCoordinateWidth, TexCoordinateHeight); 
	glVertex2f(x + Width, y + Descender);

	glTexCoord2f(TexCoordinateWidth, 0.0f); 
	glVertex2f(x + Width, y + BearingY);
	
	glEnd();
}

void sad::freetype::Glyph::makeGlyph(FT_Face face, FT_Glyph glyph)
{
	FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	FT_Bitmap & bitmap = bitmap_glyph->bitmap;

	Texture.storeBitmap(bitmap);

	Width = (float)(bitmap.width);
	Height = (float)(bitmap.rows);
	TexCoordinateWidth = Width / Texture.Width;
	TexCoordinateHeight = Height / Texture.Height;

	float diff = (float)(bitmap_glyph->top - bitmap.rows);
	Descender = diff;
	BearingY = Height + Descender;
	AdvanceX = (float)(face->glyph->advance.x >> 6);
}

void sad::freetype::Glyph::makeEmptyGlyph()
{
	Width = 0;
	Height = 0;
	Descender = 0;
	BearingY = 0;
	AdvanceX = 0;
	TexCoordinateWidth = 1.0f;
	TexCoordinateHeight = 1.0f;
	Texture.IsOnGPU = false;
	Texture.Height = 2.0f;
	Texture.Width = 2.0f;
	Texture.Pixels.resize(4);
	std::fill_n(Texture.Pixels.begin(), 4, 0);
}