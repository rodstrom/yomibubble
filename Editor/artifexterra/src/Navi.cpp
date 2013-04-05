/*
	This file is part of Navi, a library that allows developers to embed movable 
	'Navis' (Dynamic, HTML/JS/CSS-Driven GUI Overlays) within an Ogre3D application.

	Copyright (C) 2007 Adam J. Simmons
	http://www.agelessanime.com/Navi/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Navi.h"
#include "NaviUtilities.h"
#include <OgreBitwise.h>

using namespace Ogre;
using namespace NaviLibrary;
using namespace NaviLibrary::NaviUtilities;

Navi::Navi(Ogre::RenderWindow* renderWin, std::string name, std::string homepage, const NaviPosition &naviPosition, 
		   unsigned short width, unsigned short height, unsigned short zOrder, bool hideUntilLoaded)
{
	browserWin = 0;
	naviName = name;
	naviWidth = width;
	naviHeight = height;
	winWidth = renderWin->getWidth();
	winHeight = renderWin->getHeight();
	renderWindow = renderWin;
	isWinFocused = true;
	position = naviPosition;
	movable = true;
	overlay = 0;
	panel = 0;
	maxUpdatePS = 48;
	forceMax = false;
	lastUpdateTime = 0;
	opacity = 1;
	usingMask = false;
	ignoringTrans = true;
	transparent = 0.05;
	ignoringBounds = false;
	isMaterial = false;
	okayToDelete = false;
	/** edit **/
	isVisible = false;
	/** ---- **/
	isHidingUntilLoaded = hideUntilLoaded;
	fadingOut = false;
	fadingOutStart = fadingOutEnd = 0;
	fadingIn = false;
	fadingInStart = fadingInEnd = 0;
	compensateNPOT = false;
	texWidth = width;
	texHeight = height;
	maskCache = 0;
	maskPitch = 0;
	matPass = 0;
	baseTexUnit = 0;
	maskTexUnit = 0;

	createMaterial();
	createOverlay(zOrder);
	createBrowser(homepage);

	Ogre::WindowEventUtilities::addWindowEventListener(renderWin, this);
}

Navi::Navi(Ogre::RenderWindow* renderWin, std::string name, std::string homepage, unsigned short width, unsigned short height,
		   Ogre::FilterOptions texFiltering)
{
	browserWin = 0;
	naviName = name;
	naviWidth = width;
	naviHeight = height;
	winWidth = renderWin->getWidth();
	winHeight = renderWin->getHeight();
	renderWindow = renderWin;
	isWinFocused = true;
	position = NaviPosition();
	movable = false;
	overlay = 0;
	panel = 0;
	maxUpdatePS = 48;
	forceMax = false;
	lastUpdateTime = 0;
	opacity = 1;
	usingMask = false;
	ignoringTrans = true;
	transparent = 0.05;
	ignoringBounds = false;
	isMaterial = true;
	okayToDelete = false;
	isVisible = false;
	isHidingUntilLoaded = false;
	fadingOut = false;
	fadingOutStart = fadingOutEnd = 0;
	fadingIn = false;
	fadingInStart = fadingInEnd = 0;
	compensateNPOT = false;
	texWidth = width;
	texHeight = height;
	maskCache = 0;
	maskPitch = 0;
	matPass = 0;
	baseTexUnit = 0;
	maskTexUnit = 0;

	createMaterial(texFiltering);
	createBrowser(homepage);	

	WindowEventUtilities::addWindowEventListener(renderWin, this);
}


Navi::~Navi()
{
	if(maskCache)
		delete[] maskCache;

	WindowEventUtilities::removeWindowEventListener(renderWindow, this);

	if(browserWin)
	{
		browserWin->removeListener(this);
		browserWin->destroy();
	}

	if(overlay)
	{
		overlay->remove2D(panel);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(panel);
		OverlayManager::getSingletonPtr()->destroy(overlay);
	}

	MaterialManager::getSingletonPtr()->remove(naviName + "Material");
	TextureManager::getSingletonPtr()->remove(naviName + "Texture");
	if(usingMask) TextureManager::getSingletonPtr()->remove(naviName + "MaskTexture");
}

void Navi::createOverlay(unsigned short zOrder)
{
	OverlayManager& overlayManager = OverlayManager::getSingleton();

	panel = static_cast<PanelOverlayElement*>(overlayManager.createOverlayElement("Panel", naviName + "Panel"));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setMaterialName(naviName + "Material");
	panel->setDimensions(naviWidth, naviHeight);
	if(compensateNPOT)
		panel->setUV(0, 0, (Real)naviWidth/(Real)texWidth, (Real)naviHeight/(Real)texHeight);	
	
	overlay = overlayManager.create(naviName + "Overlay");
	overlay->add2D(panel);
	overlay->setZOrder(zOrder);
	resetPosition();
	if(isVisible && !isHidingUntilLoaded) overlay->show();
}

void Navi::createBrowser(const std::string& homepage)
{
	browserWin = Astral::AstralManager::Get().createBrowserWindow(naviWidth, naviHeight);
	browserWin->addListener(this);
	browserWin->focus();
	navigateTo(homepage);
}

void Navi::createMaterial(Ogre::FilterOptions texFiltering)
{
	limit<float>(opacity, 0, 1);

	if(!Bitwise::isPO2(naviWidth) || !Bitwise::isPO2(naviHeight))
	{
		if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
		{
			if(Root::getSingleton().getRenderSystem()->getCapabilities()->getNonPOW2TexturesLimited())
				compensateNPOT = true;
		}
		else compensateNPOT = true;
		
		if(compensateNPOT)
		{
			texWidth = Bitwise::firstPO2From(naviWidth);
			texHeight = Bitwise::firstPO2From(naviHeight);
		}
	}

	// Create the texture
	TexturePtr texture = TextureManager::getSingleton().createManual(
		naviName + "Texture", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, texWidth, texHeight, 0, PF_BYTE_BGR,
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE, this);

	HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	texDepth = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	texPitch = (pixelBox.rowPitch*texDepth);

	uint8* pDest = static_cast<uint8*>(pixelBox.data);

	memset(pDest, 128, texHeight*texPitch);

	pixelBuffer->unlock();

	MaterialPtr material = MaterialManager::getSingleton().create(naviName + "Material", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	matPass = material->getTechnique(0)->getPass(0);
	matPass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	matPass->setDepthWriteEnabled(false);

	baseTexUnit = matPass->createTextureUnitState(naviName + "Texture");
	
	baseTexUnit->setTextureFiltering(texFiltering, texFiltering, FO_NONE);
	if(texFiltering == FO_ANISOTROPIC)
		baseTexUnit->setTextureAnisotropy(4);
}

// This is for when the rendering device has a hiccup and loses the dynamic texture
void Navi::loadResource(Resource* resource)
{
	Texture *tex = static_cast<Texture*>(resource); 

	tex->setTextureType(TEX_TYPE_2D);
	tex->setWidth(texWidth);
	tex->setHeight(texHeight);
	tex->setNumMipmaps(0);
	tex->setFormat(PF_BYTE_BGR);
	tex->setUsage(TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	tex->createInternalResources();

	bool curForceState = forceMax;
	forceMax = true;
	update();
	forceMax = curForceState;
}

void Navi::update()
{
	if(!isWinFocused || !isVisible || isHidingUntilLoaded)
		return;

	if(maxUpdatePS)
		if(timer.getMilliseconds() - lastUpdateTime < 1000 / maxUpdatePS)
			return;

	Ogre::Real fadeMod = 1;
	
	if(fadingIn)
	{
		if(fadingInEnd < timer.getMilliseconds())
			fadingInStart = fadingInEnd = fadingIn = 0;
		else
			fadeMod = (float)(timer.getMilliseconds() - fadingInStart) / (float)(fadingInEnd - fadingInStart);
	} 
	else if(fadingOut)
	{
		if(fadingOutEnd < timer.getMilliseconds())
		{
			fadingOutStart = fadingOutEnd = fadeMod = fadingOut = isVisible = 0;

			if(!isMaterial)
				overlay->hide();
		}
		else
			fadeMod = 1 - (float)(timer.getMilliseconds() - fadingOutStart) / (float)(fadingOutEnd - fadingOutStart);
	}

	baseTexUnit->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, fadeMod * opacity);

	if(!browserWin->isDirty() && !forceMax)
		return;

	TexturePtr texture = TextureManager::getSingleton().getByName(naviName + "Texture");
	
	HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();

	uint8* destBuffer = static_cast<uint8*>(pixelBox.data);

	browserWin->render(destBuffer, (int)texPitch, (int)texDepth, forceMax);

	pixelBuffer->unlock();

	lastUpdateTime = timer.getMilliseconds();
}

bool Navi::isPointOverMe(int x, int y)
{
	if(x < 0 || x > (int)winWidth) return false;
	if(y < 0 || y > (int)winHeight) return false;
	if(isMaterial || !isVisible) return false;

	if(panel->getLeft() < x && x < (panel->getLeft()+panel->getWidth()))
		if(panel->getTop() < y && y < (panel->getTop()+panel->getHeight()))
			return !ignoringTrans || !maskCache ? true : 
				maskCache[getRelativeY(y)*maskPitch+getRelativeX(x)*1+(1-1)] > 255*transparent;

	return false;
}

void Navi::onNavigateBegin(Astral::BrowserWindow* caller, const std::string& url, bool &shouldContinue)
{
	if(url.find("local://") == 0)
	{
		std::string redirect(url);
		translateLocalProtocols(redirect);
		navigateTo(redirect);
		shouldContinue = false;
		return;
	}

	bool test = true;
	for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
	{
		(*nel)->onNavigateBegin(this, url, test);

		if(!test)
		{
			shouldContinue = false;
			break;
		}
	}
}

void Navi::onNavigateComplete(Astral::BrowserWindow* caller, const std::string& url, int responseCode)
{
	for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
		(*nel)->onNavigateComplete(this, url, responseCode);

	if(isHidingUntilLoaded && isVisible)
		show();
}

void Navi::onUpdateProgress(Astral::BrowserWindow* caller, short percentComplete)
{
}

void Navi::onStatusTextChange(Astral::BrowserWindow* caller, const std::string& statusText)
{
	if(isPrefixed(statusText, "NAVI_DATA:", false))
	{
		std::vector<std::string> stringVector = split(statusText, "?", false);
		if(stringVector.size() == 3)
		{
			NaviData naviDataEvent(stringVector[1], stringVector[2]);

			if(!eventListeners.empty())
				for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); nel++)
					(*nel)->onNaviDataEvent(this, naviDataEvent);

			if(!delegateMap.empty())
			{
				ensureKeysMapIter = ensureKeysMap.find(stringVector[1]);
				if(ensureKeysMapIter != ensureKeysMap.end())
					naviDataEvent.ensure(ensureKeysMapIter->second);

				dmBounds = delegateMap.equal_range(stringVector[1]);
				for(delegateIter = dmBounds.first; delegateIter != dmBounds.second; delegateIter++)
					delegateIter->second(naviDataEvent);
			}
		}
	}
}

void Navi::onLocationChange(Astral::BrowserWindow* caller, const std::string& url)
{
	for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
		(*nel)->onLocationChange(this, url);
}

void Navi::windowMoved(RenderWindow* rw) {}

void Navi::windowResized(RenderWindow* rw) 
{
	winWidth = rw->getWidth();
	winHeight = rw->getHeight();
}

void Navi::windowClosed(RenderWindow* rw) {}

void Navi::windowFocusChange(RenderWindow* rw) 
{
	isWinFocused = rw->isVisible();
}

void Navi::navigateTo(const std::string& url)
{
	std::string urlString(url);
	translateLocalProtocols(urlString);
	translateResourceProtocols(urlString);
	browserWin->navigateTo(urlString);
}

void Navi::navigateTo(const std::string& url, const NaviData &naviData)
{
	std::string suffix = "";

	if(naviData.getName().length())
		suffix = "?" + naviData.getName() + "?" + naviData.toQueryString();

	std::string urlString = url;
	translateLocalProtocols(urlString);
	translateResourceProtocols(urlString);
	browserWin->navigateTo(urlString + suffix);
}

void Navi::navigateBack()
{
	browserWin->navigateBack();
}

void Navi::navigateForward()
{
	browserWin->navigateForward();
}

void Navi::navigateStop()
{
	browserWin->navigateStop();
}

bool Navi::canNavigateForward()
{
	return browserWin->canNavigateForward();
}

bool Navi::canNavigateBack()
{
	return browserWin->canNavigateBack();
}

std::string Navi::evaluateJS(std::string script, const NaviUtilities::Args &args)
{
	if(args.size() && script.size())
	{
		std::vector<std::string> temp = split(script, "?", false);
		script.clear();

		for(unsigned int i = 0; i < temp.size(); ++i)
		{
			script += temp[i];
			if(args.size() > i && i != temp.size()-1)
			{
				if(args[i].isWideString())
				{
					script += "decodeURIComponent(\"" + encodeURIComponent(args[i].wstr()) + "\")";
				}
				else if(args[i].isNumber() && args[i].wstr().find_first_not_of(L"-0123456789.") == std::wstring::npos)
				{
					script += args[i].str();
				}
				else
				{
					std::string escapedStr = args[i].str();
					replaceAll(escapedStr, "\"", "\\\"");
					script += "\"" + escapedStr + "\"";
				}
			}
		}
	}

	return browserWin->evaluateJS(script);
}

Navi* Navi::addEventListener(NaviEventListener* newListener)
{
	if(newListener)
	{
		for(std::vector<NaviEventListener*>::iterator i = eventListeners.begin(); i != eventListeners.end(); ++i)
			if(*i == newListener) return this;

		eventListeners.push_back(newListener);
	}

	return this;
}

Navi* Navi::removeEventListener(NaviEventListener* removeListener)
{
	for(std::vector<NaviEventListener*>::iterator i = eventListeners.begin(); i != eventListeners.end();)
	{
		if(*i == removeListener)
			i = eventListeners.erase(i);
		else
			++i;
	}

	return this;
}

Navi* Navi::bind(const std::string &naviDataName, const NaviDelegate &callback, const NaviUtilities::Strings &keys)
{
	if(callback.empty() || naviDataName.empty()) return this;
	
	delegateMap.insert(std::pair<std::string, NaviDelegate>(naviDataName, callback));

	if(keys.size())
		ensureKeysMap[naviDataName] = keys;

	return this;
}

Navi* Navi::unbind(const std::string &naviDataName, const NaviDelegate &callback)
{
	if(delegateMap.empty()) return this;
	dmBounds = delegateMap.equal_range(naviDataName);

	delegateIter = dmBounds.first;
	while(delegateIter != dmBounds.second)
	{
		if(callback.empty())
			delegateIter = delegateMap.erase(delegateIter);
		else
		{
			if(delegateIter->second == callback)
			{
				delegateMap.erase(delegateIter);
				dmBounds = delegateMap.equal_range(naviDataName);
				delegateIter = dmBounds.first;
			}
			else delegateIter++;
		}
	}

	if(!delegateMap.count(naviDataName))
		ensureKeysMap.erase(naviDataName);

	return this;
}

Navi* Navi::setForceMaxUpdate(bool forceMaxUpdate)
{
	forceMax = forceMaxUpdate;
	return this;
}

Navi* Navi::setIgnoreBounds(bool ignoreBounds)
{
	ignoringBounds = ignoreBounds;
	return this;
}

Navi* Navi::setIgnoreTransparent(bool ignoreTrans, float threshold)
{
	ignoringTrans = ignoreTrans;

	limit<float>(threshold, 0, 1);

	transparent = threshold;
	return this;
}

Navi* Navi::setMask(std::string maskFileName, std::string groupName)
{
	if(usingMask)
	{
		if(maskTexUnit)
		{
			matPass->removeTextureUnitState(1);
			maskTexUnit = 0;
		}

		if(!TextureManager::getSingleton().getByName(naviName + "MaskTexture").isNull())
			TextureManager::getSingleton().remove(naviName + "MaskTexture");
	}

	if(maskCache)
	{
		delete[] maskCache;
		maskCache = 0;
	}

	if(maskFileName == "")
	{
		usingMask = false;
		return this;
	}

	if(!maskTexUnit)
	{
		maskTexUnit = matPass->createTextureUnitState();
		maskTexUnit->setIsAlpha(true);
		maskTexUnit->setTextureFiltering(FO_NONE, FO_NONE, FO_NONE);
		maskTexUnit->setColourOperationEx(LBX_SOURCE1, LBS_CURRENT, LBS_CURRENT);
		maskTexUnit->setAlphaOperation(LBX_MODULATE);
	}
	
	Image srcImage;
	srcImage.load(maskFileName, groupName);

	Ogre::PixelBox srcPixels = srcImage.getPixelBox();
	unsigned char* conversionBuf = 0;
	
	if(srcImage.getFormat() != Ogre::PF_BYTE_A)
	{
		size_t dstBpp = Ogre::PixelUtil::getNumElemBytes(Ogre::PF_BYTE_A);
		conversionBuf = new unsigned char[srcImage.getWidth() * srcImage.getHeight() * dstBpp];
		Ogre::PixelBox convPixels(Ogre::Box(0, 0, srcImage.getWidth(), srcImage.getHeight()), Ogre::PF_BYTE_A, conversionBuf);
		Ogre::PixelUtil::bulkPixelConversion(srcImage.getPixelBox(), convPixels);
		srcPixels = convPixels;
	}

	TexturePtr maskTexture = TextureManager::getSingleton().createManual(
		naviName + "MaskTexture", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, texWidth, texHeight, 0, PF_BYTE_A, TU_STATIC_WRITE_ONLY);

	HardwarePixelBufferSharedPtr pixelBuffer = maskTexture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	size_t maskTexDepth = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	maskPitch = pixelBox.rowPitch;

	maskCache = new unsigned char[maskPitch*texHeight];

	uint8* buffer = static_cast<uint8*>(pixelBox.data);

	memset(buffer, 0, maskPitch * texHeight);

	size_t minRowSpan = std::min(maskPitch, srcPixels.rowPitch);
	size_t minHeight = std::min(texHeight, (unsigned short)srcPixels.getHeight());

	if(maskTexDepth == 1)
	{
		for(unsigned int row = 0; row < minHeight; row++)
			memcpy(buffer + row * maskPitch, (unsigned char*)srcPixels.data + row * srcPixels.rowPitch, minRowSpan);

		memcpy(maskCache, buffer, maskPitch*texHeight);
	}
	else if(maskTexDepth == 4)
	{
		size_t destRowOffset, srcRowOffset, cacheRowOffset;

		for(unsigned int row = 0; row < minHeight; row++)
		{
			destRowOffset = row * maskPitch * maskTexDepth;
			srcRowOffset = row * srcPixels.rowPitch;
			cacheRowOffset = row * maskPitch;

			for(unsigned int col = 0; col < minRowSpan; col++)
				maskCache[cacheRowOffset + col] = buffer[destRowOffset + col * maskTexDepth + 3] = ((unsigned char*)srcPixels.data)[srcRowOffset + col];
		}
	}
	else
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
			"Unexpected depth and format were encountered while creating a PF_BYTE_A HardwarePixelBuffer. Pixel format: " + 
			StringConverter::toString(pixelBox.format) + ", Depth:" + StringConverter::toString(maskTexDepth), "Navi::setMask");
	}

	pixelBuffer->unlock();

	if(conversionBuf)
		delete[] conversionBuf;

	maskTexUnit->setTextureName(naviName + "MaskTexture");
	usingMask = true;

	return this;
}

Navi* Navi::setMaxUPS(unsigned int maxUPS)
{
	maxUpdatePS = maxUPS;
	return this;
}

Navi* Navi::setMovable(bool isMovable)
{
	if(!isMaterial)
		movable = isMovable;

	return this;
}

Navi* Navi::setOpacity(float opacity)
{
	limit<float>(opacity, 0, 1);
	
	this->opacity = opacity;

	return this;
}

Navi* Navi::setPosition(const NaviPosition &naviPosition)
{
	if(isMaterial)
		return this;

	position = naviPosition;
	resetPosition();
	
	return this;
}

Navi* Navi::resetPosition()
{
	if(isMaterial || !overlay || !panel) return this;

	if(position.usingRelative)
	{
		int left = 0 + position.data.rel.x;
		int center = (winWidth/2)-(naviWidth/2) + position.data.rel.x;
		int right = winWidth - naviWidth + position.data.rel.x;

		int top = 0 + position.data.rel.y;
		int middle = (winHeight/2)-(naviHeight/2) + position.data.rel.y;
		int bottom = winHeight-naviHeight + position.data.rel.y;

		switch(position.data.rel.position)
		{
		case Left:
			panel->setPosition(left, middle);
			break;
		case TopLeft:
			panel->setPosition(left, top);
			break;
		case TopCenter:
			panel->setPosition(center, top);
			break;
		case TopRight:
			panel->setPosition(right, top);
			break;
		case Right:
			panel->setPosition(right, middle);
			break;
		case BottomRight:
			panel->setPosition(right, bottom);
			break;
		case BottomCenter:
			panel->setPosition(center, bottom);
			break;
		case BottomLeft:
			panel->setPosition(left, bottom);
			break;
		case Center:
			panel->setPosition(center, middle);
			break;
		default:
			panel->setPosition(position.data.rel.x, position.data.rel.y);
			break;
		}
	}
	else
		panel->setPosition(position.data.abs.left, position.data.abs.top);

	return this;
}

Navi* Navi::hide(bool fade, unsigned short fadeDurationMS)
{
	if(fadingIn || fadingOut)
		fadingInStart = fadingInEnd = fadingOutStart = fadingOutEnd = fadingIn = fadingOut = 0;

	if(fade)
	{
		fadingOutStart = timer.getMilliseconds();
		fadingOutEnd = timer.getMilliseconds() + fadeDurationMS + 1;
		fadingOut = true;
	}
	else
	{
		if(!isMaterial) overlay->hide();
		isVisible = false;
	}

	return this;
}

Navi* Navi::show(bool fade, unsigned short fadeDurationMS)
{
	if(fadingIn || fadingOut)
		fadingInStart = fadingInEnd = fadingOutStart = fadingOutEnd = fadingIn = fadingOut = 0;

	if(fade)
	{
		fadingInStart = timer.getMilliseconds();
		fadingInEnd = timer.getMilliseconds() + fadeDurationMS + 1;
		fadingIn = true;
	}

	isVisible = true;
	isHidingUntilLoaded = false;
	if(!isMaterial) overlay->show();

	return this;
}

Navi* Navi::focus()
{
	if(NaviManager::GetPointer() && !isMaterial)
		NaviManager::GetPointer()->focusNavi(0, 0, this);
	else
		browserWin->focus();

	return this;
}

Navi* Navi::moveNavi(int deltaX, int deltaY)
{
	if(!isMaterial)
		panel->setPosition(panel->getLeft()+deltaX, panel->getTop()+deltaY);
	return this;
}

void Navi::getExtents(unsigned short &width, unsigned short &height)
{
	width = naviWidth;
	height = naviHeight;
}

int Navi::getRelativeX(int absX)
{
	if(isMaterial) return 0;

	int relX = absX - panel->getLeft();
	limit<int>(relX, 0, naviWidth-1);

	return relX;
}

int Navi::getRelativeY(int absY)
{
	if(isMaterial) return 0;

	int relY = absY - panel->getTop();
	limit<int>(relY, 0, naviHeight - 1);
	
	return relY;
}

bool Navi::isMaterialOnly()
{
	return isMaterial;
}

Ogre::PanelOverlayElement* Navi::getInternalPanel()
{
	if(isMaterial)
		return 0;
		
	return panel;
}

std::string Navi::getName()
{
	return naviName;
}

std::string Navi::getMaterialName()
{
	return naviName + "Material";
}

bool Navi::getVisibility()
{
	return isVisible;
}

void Navi::getDerivedUV(Ogre::Real& u1, Ogre::Real& v1, Ogre::Real& u2, Ogre::Real& v2)
{
	u1 = v1 = 0;
	u2 = v2 = 1;

	if(compensateNPOT)
	{
		u2 = (Ogre::Real)naviWidth/texWidth;
		v2 = (Ogre::Real)naviHeight/(Ogre::Real)texHeight;
	}
}

void Navi::injectMouseMove(int xPos, int yPos)
{
	browserWin->injectMouseMove(xPos, yPos);
}

void Navi::injectMouseWheel(int relScroll)
{
	browserWin->injectScroll(-(relScroll/30));
}

void Navi::injectMouseDown(int xPos, int yPos)
{
	browserWin->injectMouseDown(xPos, yPos);
}

void Navi::injectMouseUp(int xPos, int yPos)
{
	browserWin->injectMouseUp(xPos, yPos);
}