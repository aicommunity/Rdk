/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2009.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef ENGINE_H
#define ENGINE_H

#include "UEnvSupport.h"
#include "UModule.h"
#include "UStorage.h"
#include "ULibrary.h"
#include "UEnvironment.h"
#include "UContainer.h"
#include "UStorage.h"
#include "UConnector.h"
#include "UItem.h"
#include "UAModel.h"
#include "UNet.h"
#include "UComponent.h"
#include "UController.h"
#include "UStorage.h"
#include "UEnvironment.h"
#include "UEngine.h"
#include "UEnvSupport.h"
#include "UXMLEnvSerialize.h"
#include "UComponentDescription.h"
#include "UContainerDescription.h"
#include "UEnvException.h"
#include "UExceptionLogger.h"
#include "UPropertyEndpoints.h"
#include "../Math/MDMatrix.h"

//#include "Libraries/UELibraries.h"

namespace RDK {

// Typedef for backward compatibility
// UItemData was previously defined in UItemData.h which has been removed
typedef MDMatrix<double> UItemData;

}

#endif

