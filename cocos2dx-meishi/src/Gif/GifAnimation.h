/****************************************************************************
Copyright (c) 2015 Zhangpanyi

Created by Zhangpanyi on 2015

zhangpanyi@live.com
****************************************************************************/
#ifndef __GIFANIMATION_H__
#define __GIFANIMATION_H__

#include "cocos2d.h"
#include "Singleton.h"

typedef std::vector<cocos2d::Texture2D *> TextureArray;

/**
 * Gif动画定义
 */
struct GifAnimationDef
{
	std::string  filePath;                         // 文件路径
	unsigned int loops;                            // 循环次数
	float        delayPerUnit;                     // 间隔时间
	bool         restoreOriginalFrame;             // 是否还原起始帧
	GifAnimationDef() : loops(-1), delayPerUnit(0.1f), restoreOriginalFrame(false) {};
};

class GifAnimation : public Singleton < GifAnimation >
{
	GifAnimation() {};
	~GifAnimation() {};
	friend class Singleton < GifAnimation > ;

public:
	/**
	 * 创建动画
	 * @ 参数 GifAnimationDef 动画定义
	 */
	cocos2d::Animation* createAnimation(const GifAnimationDef &def);

	/**
	 * 获取第一张纹理
	 * @ 参数 std::string 文件路径
	 */
	cocos2d::Texture2D* getFristTexture(const std::string &filePath);

	/**
	 * 移除Gif动画
	 * @ 参数 std::string 动画名称
	 * @ 返回值 void
	 */
	void removeAnimation(const std::string &name);

	/**
	 * 移除所有Gif动画
	 * @ 参数 void
	 * @ 返回 void
	 */
	void removeAllAnimation();

private:
	/**
	 * 获取纹理列表
	 * @ 参数 std::string 图片路径, TextureArray 纹理容器
	 * @ 返回值 数量
	 */
	int getTextureList(const std::string &filePath, TextureArray &textureArray);

private:
	TextureArray m_textureArray;
	std::multimap<std::string, cocos2d::Texture2D *> m_animation;
};

#endif
