/* ocol2d.c */
#include "ocol2d.h"
#include "raymath.h"

bool CheckCollisionPointOrientedRec(Vector2 point, Rectangle rec, float rotation)
{
    // rotate the point around the rectangel origin but in the opposite dir the rectangle is rotated
    // this transforms the point in to the rectangles local space
    Vector2 rotPoint = { point.x - rec.x, point.y - rec.y };
    rotPoint = Vector2Rotate(rotPoint, DEG2RAD * -rotation);
    rotPoint.x = rotPoint.x + rec.x;
    rotPoint.y = rotPoint.y + rec.y;

    return CheckCollisionPointRec(rotPoint, rec);
}

bool CheckCollisionLineCircle(Vector2 p1, Vector2 p2, Vector2 center, float radius)
{
    // look for the closest point on the line to the center of the circle
    Vector2 ab = Vector2Subtract(p2, p1);
    float t = Vector2DotProduct(Vector2Subtract(center, p1), ab) / Vector2DotProduct(ab, ab);
    if (t < 0.0f || t > 1.0f)
    {
        return false;
    }
    Vector2 closestPoint = Vector2Add(p1, Vector2Scale(ab, t));
    
    // then see if its inside the circle
    return CheckCollisionPointCircle(closestPoint, center, radius);
}

bool CheckCollisionLineRec(Vector2 p1, Vector2 p2, Rectangle rec)
{
    // start with checking if the ends are in the rectangle
    if (CheckCollisionPointRec(p1, rec) || CheckCollisionPointRec(p2, rec))
    {
        return true;
    }

	// x or y values cannot be the same, or tmax will be 0
	// this causes a small ammount of inaccuracy
	p1.x = (p1.x == p2.x) ? p1.x + 0.001f : p1.x;
	p1.y = (p1.y == p2.y) ? p1.y + 0.001f : p1.y;
    
    // raycast at the rectangle
    Vector2 norm = Vector2Normalize(Vector2Subtract(p2, p1));
    norm.x = (norm.x != 0.0f) ? 1.0f / norm.x : 0.0f;
    norm.y = (norm.y != 0.0f) ? 1.0f / norm.y : 0.0f;

    Vector2 min = { rec.x, rec.y };
    min = Vector2Multiply(Vector2Subtract(min, p1), norm);
    Vector2 max = { rec.x + rec.width, rec.y + rec.height };
    max = Vector2Multiply(Vector2Subtract(max, p1), norm);
    
    float tmin = fmaxf(fminf(min.x, max.x), fminf(min.y, max.y));
    float tmax = fminf(fmaxf(min.x, max.x), fmaxf(min.y, max.y));
    
    if ((tmax < 0.0f || tmin > tmax) )
    {
        return false;
    }
    
    float t = (tmin < 0.0f) ? tmax : tmin;
    return t > 0.0f && t * t < Vector2LengthSqr(Vector2Subtract(p2, p1));
}

bool CheckCollisionLineOrientedRec(Vector2 p1, Vector2 p2, Rectangle rec, float rotation)
{
    // rotate the point around the rectangel origin but in the opposite dir the rectangle is rotated
    // this transforms the point in to the rectangles local space
    Vector2 rotP1 = { p1.x - rec.x, p1.y - rec.y };
    rotP1 = Vector2Rotate(rotP1, DEG2RAD * -rotation);
    rotP1.x = rotP1.x + rec.x;
    rotP1.y = rotP1.y + rec.y;
    
    Vector2 rotP2 = { p2.x - rec.x, p2.y - rec.y };
    rotP2 = Vector2Rotate(rotP2, DEG2RAD * -rotation);
    rotP2.x = rotP2.x + rec.x;
    rotP2.y = rotP2.y + rec.y;

    return CheckCollisionLineRec(rotP1, rotP2, rec);
}


bool CheckCollisionCircleOrientedRec(Vector2 center, float radius, Rectangle rec, float rotation)
{
    // rotate the circle around the rectangle origin but in the opposite dir the rectangle is rotated
    // this transforms the circle center in to the rectangles local space
    Vector2 rotCenter = { center.x - rec.x, center.y - rec.y };
    rotCenter = Vector2Rotate(rotCenter, DEG2RAD * -rotation);
    rotCenter.x = rotCenter.x + rec.x;
    rotCenter.y = rotCenter.y + rec.y;

	CheckCollisionCircleRec(rotCenter, radius, rec);
}

Interval2 GetInterval(Rectangle rec, Vector2 axis)
{
    Vector2 min = { rec.x, rec.y };
    Vector2 max = { rec.x + rec.width, rec.y + rec.height };
	Vector2 verts[] = { min, max, { min.x, max.y }, { max.x, min.y } };

	// each vertex is projected to the axis
	float projection = Vector2DotProduct(axis, verts[0]);
	Interval2 result = { projection, projection };	
	for (int i = 1; i < 4; ++i)
	{
		projection = Vector2DotProduct(axis, verts[i]);
		if (projection < result.min)
		{
			result.min = projection;
		}
		if (projection > result.max)
		{
			result.max = projection;
		}
	}
	return result;
}

Interval2 GetOrientedInterval(Rectangle rec, float rotation, Vector2 axis)
{
    Vector2 min = { rec.x, rec.y };
    Vector2 max = { rec.x + rec.width, rec.y + rec.height };
	Vector2 verts[] = { min, max, { min.x, max.y }, { max.x, min.y } };

	// rotate the last 3 of the rectangles vertexes with its rotation
	// (the first one stays still when the rectangle rotates)
	for (int i = 1; i < 4; i++)
	{
		// min is rec position
		Vector2 r = Vector2Subtract(verts[i], min);
		r = Vector2Rotate(r, rotation * DEG2RAD);
		verts[i] = Vector2Add(r, min);
	}

    // each vertex is projected to the axis
	float projection = Vector2DotProduct(axis, verts[0]);
	Interval2 result = { projection, projection };	
	for (int i = 0; i < 4; i++)
	{
		projection = Vector2DotProduct(axis, verts[i]);
		if (projection < result.min)
		{
			result.min = projection;
		}
		if (projection > result.max)
		{
			result.max = projection;
		}
	}
	return result;
}

bool OverlapOnOrientedAxis(Rectangle rec1, Rectangle rec2, float rotation2,  Vector2 axis)
{
	// project the unoriented and oriented rectangles to the axis, and see if they overlap
	Interval2 a = GetInterval(rec1, axis);
 	Interval2 b = GetOrientedInterval(rec2, rotation2, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool CheckCollisionRecOrientedRec(Rectangle rec1, Rectangle rec2, float rotation2)
{
	// collision checked using the Separating Axis Theorem (SAT)
	Vector2 rotAxisA = { rec2.x, 0 };
	rotAxisA = Vector2Normalize(rotAxisA);
	rotAxisA = Vector2Rotate(rotAxisA, rotation2 * DEG2RAD);
	Vector2 rotAxisB = { 0, rec2.y };
	rotAxisB = Vector2Normalize(rotAxisB);
	rotAxisB = Vector2Rotate(rotAxisB, rotation2 * DEG2RAD);
	// x and y axis of the unorienred rectangle and the rotated axis of the oriented one
	Vector2 axisToTest[] = { { 1, 0 }, { 0, 1 }, rotAxisA, rotAxisB };

	for (int i = 0; i < 4; ++i)
	{
		// the rectangles need to overlap on every axis or else there is no collision
		if (!OverlapOnOrientedAxis(rec1, rec2, rotation2, axisToTest[i]))
		{
			return false;
		}
	}
	return true;
}

bool CheckCollisionOrientedRecs(Rectangle rec1, float rotation1, Rectangle rec2, float rotation2)
{
	// rotate rec2 the in to rec1 local space
	// the rec2 is rotated around rec1 origin, not around its own origin 
	rotation2 -= rotation1;
    Vector2 rec1Location = { rec1.x, rec1.y };
	Vector2 rec2RotatedLocation = { rec2.x, rec2.y };
	rec2RotatedLocation = Vector2Subtract(rec2RotatedLocation, rec1Location);
	rec2RotatedLocation = Vector2Rotate(rec2RotatedLocation, -rotation1 * DEG2RAD);
	rec2RotatedLocation = Vector2Add(rec2RotatedLocation, rec1Location);
	rec2.x = rec2RotatedLocation.x;
	rec2.y = rec2RotatedLocation.y;

	// once rec2 is rotated around rec1, we can forget rec1 is oriented
	return CheckCollisionRecOrientedRec(rec1, rec2, rotation2);
}
