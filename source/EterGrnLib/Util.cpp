#include "StdAfx.h"
#include "Util.h"

/*
bool GrannyMeshGetTextureAnimation(granny_mesh* pgrnMesh, float* puVelocity, float* pvVelocity)
{
	static granny_data_type_definition s_UVType[] =
	{
		{GrannyInt32Member, "UV Animation Flag"},
		{GrannyInt32Member, "X Velocity"},
		{GrannyInt32Member, "Y Velocity"},
		{GrannyEndMember}
	};

	typedef struct SUVData
	{
		granny_int32 UVAnimationFlag;
		granny_int32 xVelocity;
		granny_int32 yVelocity;
	} TUVData;

	TUVData UVData;

	granny_variant& rExtendedData = pgrnMesh->ExtendedData;
	GrannyConvertSingleObject(rExtendedData.Type, rExtendedData.Object, s_UVType, &UVData);

	*puVelocity = float(UVData.xVelocity) / 100000.0f;
	*pvVelocity = float(UVData.yVelocity) / 100000.0f;

	return UVData.UVAnimationFlag ? true : false;
}
*/

/*
bool GrannyMeshIsTextureAnimation(granny_mesh* pgrnMesh)
{
	if (GrannyMeshIsRigid(pgrnMesh))
	{
		float xVelocity, yVelocity;

		if (GrannyMeshGetTextureAnimation(pgrnMesh, &xVelocity, &yVelocity))
			return true;
	}

	return false;
}
*/

bool GrannyMeshIsDeform(granny_mesh* pgrnMesh)
{
	if (GrannyMeshIsRigid(pgrnMesh))
		return false;

	return true;
}
