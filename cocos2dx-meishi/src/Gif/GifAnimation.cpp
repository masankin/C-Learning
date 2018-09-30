#include "cocos2d.h"
#include "GifAnimation.h"
#include "gif_lib/gif_lib.h"
using namespace cocos2d;

namespace bitmap
{
#define UNINITIALIZED_UINT ((uint32_t)-1)

#define MulS16(x, y)  ((int16_t)(x) * (y))
#define AlphaMul(value, alpha256)     (MulS16(value, alpha256) >> 8)

	typedef uint8_t ColorType;

	template <typename T> inline void TSwap(T &a, T &b)
	{
		T c(a);
		a = b;
		b = c;
	}

	class Rgba
	{
	public:
		Rgba()
			: red(0)
			, green(0)
			, blue(0)
			, alpha(0)
		{

		}

		Rgba(ColorType ialpha, ColorType ired, ColorType igreen, ColorType iblue)
			: alpha(ialpha)
			, red(ired)
			, green(igreen)
			, blue(iblue)
		{

		}

		Rgba(const Rgba &color)
		{
			alpha = color.alpha;
			blue = color.blue;
			green = color.green;
			red = color.red;
		}

		void setColor(ColorType ialpha, ColorType ired, ColorType igreen, ColorType iblue)
		{
			alpha = ialpha;
			red = ired;
			green = igreen;
			blue = iblue;
		}

	public:
		ColorType red;
		ColorType green;
		ColorType blue;
		ColorType alpha;
	};

	class Bitmap
	{
	public:
		Bitmap()
			: _uWidth(0)
			, _uHight(0)
		{

		};

		~Bitmap()
		{

		};

	public:
		void allocate(uint32_t width, uint32_t height)
		{
			_uWidth = width;
			_uHight = height;

			if (_rData.size() < _uWidth * _uHight * sizeof(Rgba))
			{
				_rData.resize(_uWidth * _uHight * sizeof(Rgba));
			}
		};

		uint32_t getWidth() const
		{
			return _uWidth;
		}

		uint32_t getHeight() const
		{
			return _uHight;
		}

		bool isValid()
		{
			return _uWidth > 0 && _uHight > 0 && !_rData.empty();
		}

		uint32_t getPixelLenth()
		{
			return _uWidth * _uHight;
		};

		const uint32_t* getRGBA()
		{
			if (_rData.empty())
			{
				return nullptr;
			}
			return (uint32_t *)(&(_rData[0]));
		}

		void eraseColor(const Rgba &color)
		{
			Rgba paintColor = color;

			if (255 != color.alpha)
			{
				paintColor.red = AlphaMul(color.red, color.alpha);
				paintColor.green = AlphaMul(color.green, color.alpha);
				paintColor.blue = AlphaMul(color.blue, color.alpha);
			}

			for (uint32_t i = 0; i < _uWidth * _uHight; ++i)
			{
				_rData[i] = paintColor;
			}
		}

		Rgba* getLocation(int left, int top)
		{
			return &_rData[top * _uWidth + left];
		}

		void swap(Bitmap *toSwap)
		{
			auto temp = std::move(_rData);
			_rData = std::move(toSwap->_rData);
			toSwap->_rData = std::move(temp);

			TSwap(_uWidth, toSwap->_uWidth);
			TSwap(_uHight, toSwap->_uHight);
		}

	private:
		uint32_t		  _uWidth;
		uint32_t		  _uHight;
		std::vector<Rgba> _rData;
	};

	static Bitmap sForeground;
	static Bitmap sBackground;
}

class GifRead
{
public:
	static int get()
	{
		return _readPos;
	}

	static void reset()
	{
		_readPos = 0;
	}

	static void move(int n)
	{
		_readPos += n;
	}

private:
	static int _readPos;
};

int GifRead::_readPos = 0;

static int GifReadCallBackFunc(GifFileType *gif, GifByteType *bytes, int size)
{
	int next = GifRead::get() + size;
	Data *pData = static_cast<Data *>(gif->UserData);

	if (next <= pData->getSize())
	{
		memcpy(bytes, pData->getBytes() + GifRead::get(), size);
		GifRead::move(size);
		return size;
	}
	else
	{
		unsigned int gap = pData->getSize() - next;
		if (gap > 0)
		{
			memcpy(bytes, pData->getBytes() + GifRead::get(), gap);
			GifRead::move(gap);
		}
		return gap;
	}
}

static void TransparencyAndDisposalMethod(const SavedImage *frame, bool *trans, int *disposal)
{
	*trans = false;
	*disposal = 0;
	for (int i = 0; i < frame->ExtensionBlockCount; ++i)
	{
		ExtensionBlock *eb = frame->ExtensionBlocks + i;
		if (eb->Function == GRAPHICS_EXT_FUNC_CODE && eb->ByteCount == 4)
		{
			*trans = ((eb->Bytes[0] & 1) == 1);
			*disposal = ((eb->Bytes[0] >> 2) & 7);
		}
	}
}

static bool CheckIfCover(const SavedImage* target, const SavedImage* covered)
{
	if (target->ImageDesc.Left <= covered->ImageDesc.Left
		&& covered->ImageDesc.Left + covered->ImageDesc.Width <=
		target->ImageDesc.Left + target->ImageDesc.Width
		&& target->ImageDesc.Top <= covered->ImageDesc.Top
		&& covered->ImageDesc.Top + covered->ImageDesc.Height <=
		target->ImageDesc.Top + target->ImageDesc.Height)
	{
		return true;
	}
	return false;
}

static void FillRect(bitmap::Bitmap *bm, GifWord left, GifWord top, GifWord width, GifWord height, bitmap::Rgba col)
{
	int bmWidth = bm->getWidth();
	int bmHeight = bm->getHeight();
	bitmap::Rgba *dst = bm->getLocation(left, top);
	GifWord copyWidth = width;
	if (left + copyWidth > bmWidth)
	{
		copyWidth = bmWidth - left;
	}

	GifWord copyHeight = height;
	if (top + copyHeight > bmHeight)
	{
		copyHeight = bmHeight - top;
	}

	for (; copyHeight > 0; --copyHeight)
	{
		for (int wIndex = 0; wIndex < bmWidth; wIndex++, ++dst)
		{
			*dst = col;
		}
	}
}

static void DisposeFrameIfNeeded(bitmap::Bitmap *bm, const SavedImage *cur, const SavedImage *next, bitmap::Bitmap *backup, bitmap::Rgba color)
{
	bool curTrans, nextTrans;
	int curDisposal, nextDisposal;
	TransparencyAndDisposalMethod(cur, &curTrans, &curDisposal);
	TransparencyAndDisposalMethod(next, &nextTrans, &nextDisposal);

	if ((curDisposal == 2 || curDisposal == 3) && (nextTrans || !CheckIfCover(next, cur)))
	{
		switch (curDisposal)
		{
		case 2:
		{
			FillRect(bm, cur->ImageDesc.Left, cur->ImageDesc.Top, cur->ImageDesc.Width, cur->ImageDesc.Height, color);
			break;
		}
		case 3:
		{
			bm->swap(backup);
			break;
		}
		}
	}

	if (nextDisposal == 3)
	{
		memcpy(backup->getLocation(0, 0), bm->getLocation(0, 0), bm->getPixelLenth() * sizeof(bitmap::Rgba));
	}
}

static void CopyLine(bitmap::Rgba *dst, const unsigned char *src, const ColorMapObject *cmap, int transparent, int width)
{
	for (; width > 0; --width, ++src, ++dst)
	{
		if (*src != transparent)
		{
			const GifColorType &col = cmap->Colors[*src];
			*dst = bitmap::Rgba(0xFF, col.Red, col.Green, col.Blue);
		}
	}
}

static void CopyInterlaceGroup(bitmap::Bitmap *bm,
	const unsigned char *&src,
	const ColorMapObject *cmap,
	int transparent,
	int copyWidth,
	int copyHeight,
	const GifImageDesc &imageDesc,
	int rowStep,
	int startRow)
{
	int row = 0;
	for (row = startRow; row < copyHeight; row += rowStep)
	{
		bitmap::Rgba *dst = bm->getLocation(imageDesc.Left, imageDesc.Top + row);
		CopyLine(dst, src, cmap, transparent, copyWidth);
		src += imageDesc.Width;
	}

	src += imageDesc.Width * ((imageDesc.Height - row + rowStep - 1) / rowStep);
}

static void BlitNormal(bitmap::Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap, int transparent)
{
	GifWord width = bm->getWidth();
	GifWord height = bm->getHeight();
	const unsigned char *src = (unsigned char *)frame->RasterBits;
	bitmap::Rgba *dst = bm->getLocation(frame->ImageDesc.Left, frame->ImageDesc.Top);

	GifWord copyWidth = frame->ImageDesc.Width;
	if (frame->ImageDesc.Left + copyWidth > width)
	{
		copyWidth = width - frame->ImageDesc.Left;
	}

	GifWord copyHeight = frame->ImageDesc.Height;
	if (frame->ImageDesc.Top + copyHeight > height)
	{
		copyHeight = height - frame->ImageDesc.Top;
	}

	for (; copyHeight > 0; --copyHeight)
	{
		CopyLine(dst, src, cmap, transparent, copyWidth);
		src += frame->ImageDesc.Width;
		dst += width;
	}
}

static void BlitInterlace(bitmap::Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap, int transparent)
{
	int width = bm->getWidth();
	int height = bm->getHeight();
	GifWord copyWidth = frame->ImageDesc.Width;
	GifWord copyHeight = frame->ImageDesc.Height;

	if (frame->ImageDesc.Left + copyWidth > width)
	{
		copyWidth = width - frame->ImageDesc.Left;
	}

	if (frame->ImageDesc.Top + copyHeight > height)
	{
		copyHeight = height - frame->ImageDesc.Top;
	}

	const unsigned char *src = (unsigned char *)frame->RasterBits;
	CopyInterlaceGroup(bm, src, cmap, transparent, copyWidth, copyHeight, frame->ImageDesc, 8, 0);
	CopyInterlaceGroup(bm, src, cmap, transparent, copyWidth, copyHeight, frame->ImageDesc, 8, 4);
	CopyInterlaceGroup(bm, src, cmap, transparent, copyWidth, copyHeight, frame->ImageDesc, 4, 2);
	CopyInterlaceGroup(bm, src, cmap, transparent, copyWidth, copyHeight, frame->ImageDesc, 2, 1);
}

static void DrawFrame(bitmap::Bitmap *bm, const SavedImage *frame, const ColorMapObject *cmap)
{
	int transparent = -1;

	for (int i = 0; i < frame->ExtensionBlockCount; ++i)
	{
		ExtensionBlock *eb = frame->ExtensionBlocks + i;
		if (eb->Function == GRAPHICS_EXT_FUNC_CODE && eb->ByteCount == 4)
		{
			bool has_transparency = ((eb->Bytes[0] & 1) == 1);
			if (has_transparency)
			{
				transparent = (unsigned char)eb->Bytes[3];
			}
		}
	}

	if (frame->ImageDesc.ColorMap)
	{
		cmap = frame->ImageDesc.ColorMap;
	}

	if (cmap == nullptr || cmap->ColorCount != (1 << cmap->BitsPerPixel))
	{
		return;
	}

	if (frame->ImageDesc.Interlace)
	{
		BlitInterlace(bm, frame, cmap, transparent);
	}
	else
	{
		BlitNormal(bm, frame, cmap, transparent);
	}
}

Texture2D* CreateTextureByBitmap(bitmap::Bitmap *bm, const std::string &textureName)
{
	Texture2D *pTexture = nullptr;
	auto pTextureCache = Director::getInstance()->getTextureCache();

	pTexture = pTextureCache->getTextureForKey(textureName);
	if (pTexture) return pTexture;

	if (!bm || !bm->isValid())
	{
		return nullptr;
	}

	Image *pImage = new Image;
	const uint32_t* RgbaData = bm->getRGBA();

	bool bResult = pImage->initWithRawData((unsigned char *)RgbaData, bm->getPixelLenth(), bm->getWidth(), bm->getHeight(), 8);
	if (!bResult) return nullptr;

	pTexture = pTextureCache->addImage(pImage, textureName);
	delete pImage;

	return pTexture;
}

cocos2d::Texture2D* GifAnimation::getFristTexture(const std::string &filePath)
{
	const int suffix = filePath.rfind(".gif");
	if (suffix != filePath.size() - 4)
	{
		CCAssert(false, "file path is wrong!");
		return nullptr;
	}

	Data data = FileUtils::getInstance()->getDataFromFile(filePath);
	if (data.isNull()) return nullptr;

	int error = 0;
	GifRead::reset();
	GifFileType *pGifFileType = DGifOpen(&data, &GifReadCallBackFunc, &error);

	if (pGifFileType == nullptr || DGifSlurp(pGifFileType) != GIF_OK)
	{
		DGifCloseFile(pGifFileType);
		return nullptr;
	}

	if (pGifFileType->ImageCount < 1)
	{
		DGifCloseFile(pGifFileType);
		return nullptr;
	}

	const int width = pGifFileType->SWidth;
	const int height = pGifFileType->SHeight;
	if (width <= 0 || height <= 0)
	{
		DGifCloseFile(pGifFileType);
		return nullptr;
	}

	bitmap::sForeground.allocate(width, height);
	bitmap::sBackground.allocate(width, height);

	bitmap::Rgba bgColor;
	if (pGifFileType->SColorMap)
	{
		const GifColorType &col = pGifFileType->SColorMap->Colors[pGifFileType->SBackGroundColor];
		bgColor.setColor(255, col.Red, col.Green, col.Blue);
	}

	bool trans;
	int disposal;
	bitmap::Rgba paintingColor;
	const SavedImage *cur = &pGifFileType->SavedImages[0];

	TransparencyAndDisposalMethod(cur, &trans, &disposal);
	if (trans == false && pGifFileType->SColorMap)
	{
		paintingColor = bgColor;
	}

	bitmap::sForeground.eraseColor(paintingColor);
	bitmap::sBackground.eraseColor(paintingColor);

	DrawFrame(&bitmap::sForeground, cur, pGifFileType->SColorMap);
	auto pTexture = CreateTextureByBitmap(&bitmap::sForeground, filePath);

	DGifCloseFile(pGifFileType);

	return pTexture;
}

int GifAnimation::getTextureList(const std::string &filePath, TextureArray &textureArray)
{
	int textureCount = 0;
	const int suffix = filePath.rfind(".gif");
	if (suffix != filePath.size() - 4)
	{
		CCAssert(false, "file path is wrong!");
		return textureCount;
	}

	Data data = FileUtils::getInstance()->getDataFromFile(filePath);
	if (data.isNull()) return textureCount;

	int error = 0;
	GifRead::reset();
	GifFileType *pGifFileType = DGifOpen(&data, &GifReadCallBackFunc, &error);

	if (pGifFileType == nullptr || DGifSlurp(pGifFileType) != GIF_OK)
	{
		DGifCloseFile(pGifFileType);
		return textureCount;
	}

	if (pGifFileType->ImageCount < 1)
	{
		DGifCloseFile(pGifFileType);
		return textureCount;
	}

	const int width = pGifFileType->SWidth;
	const int height = pGifFileType->SHeight;
	if (width <= 0 || height <= 0)
	{
		DGifCloseFile(pGifFileType);
		return textureCount;
	}

	bitmap::sForeground.allocate(width, height);
	bitmap::sBackground.allocate(width, height);

	bitmap::Rgba bgColor;
	if (pGifFileType->SColorMap)
	{
		const GifColorType &col = pGifFileType->SColorMap->Colors[pGifFileType->SBackGroundColor];
		bgColor.setColor(255, col.Red, col.Green, col.Blue);
	}

	char tempStr[8];
	std::string textureName;
	bitmap::Rgba paintingColor;
	for (int i = 0; i < pGifFileType->ImageCount; ++i)
	{
		const SavedImage* cur = &pGifFileType->SavedImages[i];
		if (i == 0)
		{
			bool trans;
			int disposal;
			TransparencyAndDisposalMethod(cur, &trans, &disposal);
			if (!trans && pGifFileType->SColorMap)
			{
				paintingColor = bgColor;
			}

			bitmap::sForeground.eraseColor(paintingColor);
			bitmap::sBackground.eraseColor(paintingColor);
		}
		else
		{
			const SavedImage *prev = &pGifFileType->SavedImages[i - 1];
			DisposeFrameIfNeeded(&bitmap::sForeground, prev, cur, &bitmap::sBackground, paintingColor);
		}

		DrawFrame(&bitmap::sForeground, cur, pGifFileType->SColorMap);

		snprintf(tempStr, sizeof(tempStr), "_%03d", i + 1);
		textureName = filePath;
		textureName.insert(suffix, tempStr);
		auto pTexture = CreateTextureByBitmap(&bitmap::sForeground, textureName);
		if (pTexture)
		{
			++textureCount;
			textureArray.push_back(pTexture);
		}
	}

	DGifCloseFile(pGifFileType);

	return textureCount;
}

Animation* GifAnimation::createAnimation(const GifAnimationDef &def)
{
	auto result = m_animation.find(def.filePath);
	if (result != m_animation.end())
	{
		return AnimationCache::getInstance()->getAnimation(def.filePath);
	}

	m_textureArray.clear();
	int count = getTextureList(def.filePath, m_textureArray);
	if (count < 1) return nullptr;

	Animation *pAnimation = Animation::create();
	pAnimation->setDelayPerUnit(def.delayPerUnit);
	pAnimation->setRestoreOriginalFrame(def.restoreOriginalFrame);
	pAnimation->setLoops(def.loops);

	for (size_t i = 0; i < m_textureArray.size(); i++)
	{
		pAnimation->addSpriteFrameWithTexture(m_textureArray[i], Rect(0, 0, m_textureArray[i]->getContentSize().width, m_textureArray[i]->getContentSize().height));
	}
	AnimationCache::getInstance()->addAnimation(pAnimation, def.filePath);

	for (unsigned int i = 0; i < m_textureArray.size(); ++i)
	{
		m_animation.insert(std::make_pair(def.filePath, m_textureArray[i]));
	}

	return pAnimation;
}

void GifAnimation::removeAnimation(const std::string &name)
{
	auto range = m_animation.equal_range(name);
	for (auto itr = range.first; itr != range.second;)
	{
		Director::getInstance()->getTextureCache()->removeTexture((*itr).second);
		itr = m_animation.erase(itr);
	}
	AnimationCache::getInstance()->removeAnimation(name);
}

void GifAnimation::removeAllAnimation()
{
	for (auto itr = m_animation.begin(); itr != m_animation.end(); ++itr)
	{
		AnimationCache::getInstance()->removeAnimation((*itr).first);
		Director::getInstance()->getTextureCache()->removeTexture((*itr).second);
	}
	m_animation.clear();
}
