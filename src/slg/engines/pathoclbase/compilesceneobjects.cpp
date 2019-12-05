/***************************************************************************
 * Copyright 1998-2018 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxCoreRender.                                   *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#if !defined(LUXRAYS_DISABLE_OPENCL)

#include "slg/engines/pathoclbase/compiledscene.h"

using namespace std;
using namespace luxrays;
using namespace slg;

void CompiledScene::CompileSceneObjects() {
	wasSceneObjectsCompiled = true;

	//--------------------------------------------------------------------------
	// Translate mesh material indices
	//--------------------------------------------------------------------------

	const u_int objCount = scene->objDefs.GetSize();
	sceneObjs.resize(objCount);
	for (u_int i = 0; i < objCount; ++i) {
		slg::ocl::SceneObject &oclScnObj = sceneObjs[i];
		const SceneObject *scnObj = scene->objDefs.GetSceneObject(i);

		oclScnObj.objectID = scnObj->GetID();

		const Material *m = scnObj->GetMaterial();
		oclScnObj.materialIndex = scene->matDefs.GetMaterialIndex(m);

		const ImageMap *combinedBakeMap = scnObj->GetCombinedBakeMap();
		if (combinedBakeMap) {
			oclScnObj.combinedBakeMapIndex = scene->imgMapCache.GetImageMapIndex(combinedBakeMap);
			oclScnObj.combinedBakeMapUVIndex = scnObj->GetCombinedBakeMapUVIndex();
		} else {
			oclScnObj.combinedBakeMapIndex = NULL_INDEX;
			oclScnObj.combinedBakeMapUVIndex = NULL_INDEX;
		}

		oclScnObj.cameraInvisible = scnObj->IsCameraInvisible();
	}
}

#endif
