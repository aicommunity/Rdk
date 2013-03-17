#ifndef MYRDK_H
#define MYRDK_H

#ifdef _MSC_VER
#pragma warning(disable : 4100)
#pragma warning(disable : 4355)
#pragma warning(disable : 4800)
#pragma warning(disable : 4503)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wswitch"
#endif

#include "../../Core/System/rdk_system.h"
#include "../../ThirdParty/ThirdParty.h"
#include "../../Core/Math/UMath.h"
#include "../../Core/Utilities/Utilities.h"
#include "../../Core/Serialize/Serialize.h"

#include "../../Core/Engine/UEnvSupport.h"
#include "../../Core/Engine/UXMLEnvSerialize.h"
#include "../../Core/Engine/UBinaryEnvSerialize.h"
#include "../../Core/Graphics/UBitmap.h"
#include "../../Core/Graphics/UAGraphics.h"
#include "../../Core/Graphics/UGraphicsIO.h"
#include "../../Core/Graphics/UGraphics.h"
#include "../../Core/Graphics/UFont.h"
#include "../../Core/Graphics/UDrawEngine.h"
#include "../../Core/Graphics/UGraphicsXMLSerialize.h"
#include "../../Core/Graphics/UGraphicsBinarySerialize.h"
#include "../../Core/Graphics/UBPtzCamera.h"

#include "../../Core/Application/UApplication.h"

#endif

