#include "GeometryCone.h"

Mesh::Geometry GeometryCone::create(Params& p)
{
	return GeometryCylinder::create(0, p.radius, p.height, p.radialSegments, 
		p.heightSegments, p.openEnded, p.thetaStart, p.thetaLength);
}

Mesh::Geometry GeometryCone::create(float radius, float height, 
	unsigned int radialSegments, unsigned int heightSegments, 
	bool openEnded, float thetaStart, float thetaLength)
{
	return GeometryCylinder::create(0, radius, height, radialSegments, 
		heightSegments, openEnded, thetaStart, thetaLength);
}
