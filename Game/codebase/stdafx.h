#ifndef _NOSTROMO_PRECOMPILED_H_
#define _NOSTROMO_PRECOMPILED_H_

#define NOMINMAX
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "OGRE\Ogre.h"
#include "OIS\OIS.h"
#include "Bullet\btBulletDynamicsCommon.h"
#include "Bullet\btBulletCollisionCommon.h"

#include "nSystem.h"
#include "Convert.h"
#include "nSystemPrereq.h"

// C++ Generic includes
#include <vector>
#include <map>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>

#endif // _NOSTROMO_PRECOMPILED_H_