#ifndef ____LibV2Load____
#define ____LibV2Load____
#define v2Zero v2New(0.0,0.0)
#define v2UnitX v2New(1.0,0.0)
#define v2UnitY v2New(0.0,1.0)
#define v2Length2(v) __v2Length2(v)
#define v2Length(v) __v2Length(v)

struct Vector2f
{
     float X;
     float Y;
};

inline Vector2f v2New(float xPT, float yPT)
{
	 Vector2f v;
     v.X = xPT;
     v.Y = yPT;
	 return v;
}

inline Vector2f v2Set(Vector2f v, float xPT, float yPT)
{
     v.X = xPT;
     v.Y = yPT;
     return v;
}

float __v2Length2(Vector2f v)
{
     return v.X * v.X + v.Y * v.Y;
}


float __v2Length(Vector2f v)
{
     return sqrt((v2Length2(v)));
}

inline float v2Angle(Vector2f vect)
{
     float  FLTRETVAL;
     asm
     {
         atan2d FLTRETVAL, vect.Y, vect.X
     }
     return FLTRETVAL;
}

inline float v2AngleBetween(Vector2f v1, Vector2f v2)
{
     float a = ((v2Angle(v1) - v2Angle(v2)) % 360);
     if(a > 180)
          return a - 360;
     return a;
}

inline string v2AsString(Vector2f v)
{
     return "<"+NumToStr(v.X)+","+NumToStr(v.Y)+">";
}

inline float v2DotProd(Vector2f v1, Vector2f v2)
{
     return v1.X * v2.X + v1.Y * v2.Y;
}

inline Vector2f v2Add(Vector2f v1, Vector2f v2)
{
     Vector2f v;
     v.X = v1.X + v2.X;
     v.Y = v1.Y + v2.Y;
     return v;
}

inline Vector2f v2Dif(Vector2f v1, Vector2f v2)
{
     Vector2f v;
     v.X = v1.X - v2.X;
     v.Y = v1.Y - v2.Y;
     return v;

}

inline float v2Dist(Vector2f v1, Vector2f v2)
{
     return sqrt(v2Length2(v2Dif(v1,v2)));
}

Vector2f v2Mult(Vector2f v, float scalar)
{
     v.X *= scalar;
     v.Y *= scalar;
	 return v;
}

Vector2f v2Normalized(Vector2f v)
{
     return v2Mult(v, 1 / v2Length(v));
}

inline Vector2f v2FromAngle(float angleDeg)
{
     Vector2f v;
     asm
     {
          sind v.X, angleDeg
          cosd v.Y, angleDeg
     }
     return v;
}

inline Vector2f v2Opposite(Vector2f v)
{
     return v2New(-v.X,-v.Y);
}

inline Vector2f v2Reflect(Vector2f v, Vector2f across)
{
     return v2Add(
               v2Mult(
                  (2 * v2DotProd(v,across) /
                     v2Length2(across)),
                  across),
               v);
}

inline Vector2f v2ReflectX(Vector2f v)
{
     return v2New(v.X, -v.Y);
}

inline Vector2f v2ReflectY(Vector2f v)
{
     return v2New(-v.X,v.Y);
}

inline Vector2f v2RotateAbout(Vector2f pivot, Vector2f pt, float angle)
{
     Vector2f delta = v2Dif(pt, pivot);
     float dist = v2Length(delta);
     float angle0 = v2Angle(delta);
     float angleF = angle0 + angle;
     return v2Add(pivot, v2Mult(v2FromAngle(angleF),dist));
     
}

inline Vector2f v2Midpoint(Vector2f pt1, Vector2f pt2)
{
     return v2Add(pt1, v2Mult(v2Dif(pt1, pt2), 0.5));
}
#endif
