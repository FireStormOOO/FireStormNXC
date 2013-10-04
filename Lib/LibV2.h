struct Vector2f
{
     float X;
     float Y;
};

Vector2f v2New(float xPT, float yPT)
{
	 Vector2f v;
     v.X = xPT;
     v.Y = yPT;
	 return v;
}

void v2Set(Vector2f v, float xPT, float yPT)
{
     v.X = xPT;
     v.Y = yPT;
}

float v2Length2(Vector2f v)
{
     return (v.X * v.X + v.Y * v.Y);
}


float v2Length(Vector2f v)
{
     return sqrt(v2Length2(v));
}

inline float v2Angle(Vector2f vect)
{
     //return atan2(v.Y, v.X);
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

inline Vector2f v2Mult(Vector2f v, float scalar)
{
     v.X *= scalar;
     v.Y *= scalar;
	 return v;
}

inline Vector2f v2Normalized(Vector2f v)
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
