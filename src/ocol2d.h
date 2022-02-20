/* ocol2d.h */
#include "raylib.h"

typedef struct Interval2 {
    float min;
    float max;
} Interval2;

extern bool CheckCollisionPointOrientedRec(Vector2 point, Rectangle rec, float rotation); // Check if point is inside oriented rectangle
extern bool CheckCollisionLineCircle(Vector2 p1, Vector2 p2, Vector2 center, float radius); // Check collision between line and circle
extern bool CheckCollisionLineRec(Vector2 p1, Vector2 p2, Rectangle rec); // Check collision between line and rectangle
extern bool CheckCollisionLineOrientedRec(Vector2 p1, Vector2 p2, Rectangle rec, float rotation); // Check collision between line and oriented rectangle
extern bool CheckCollisionCircleOrientedRec(Vector2 center, float radius, Rectangle rec, float rotation); // Check collision between circle and oriented rectangle
extern Interval2 GetInterval(Rectangle rec, Vector2 axis);
extern Interval2 GetOrientedInterval(Rectangle rec, float rotation, Vector2 axis);
extern bool OverlapOnOrientedAxis(Rectangle rec1, Rectangle rec2, float rotation2,  Vector2 axis);
extern bool CheckCollisionRecOrientedRec(Rectangle rec1, Rectangle rec2, float rotation2); // Check collision between rectangle and oriented rectangle
extern bool CheckCollisionOrientedRecs(Rectangle rec1, float rotation1, Rectangle rec2, float rotation2); // Check collision between 2 oriented rectangles
