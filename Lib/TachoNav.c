#ifndef ____TachoNavLoad____
#define ____TachoNavLoad____
#include "Lib/LibV2.c"
#include "Lib/LineLib.c"

struct tnFlags
{
     float WheelBaseWidth;
     float WheelDiameter;
     float heading;
     char leftMotor;
     char rightMotor;
     Vector2f position;
     long tacL;
     long tacR;
     char posLine;
     char headLine;
     bool convergenceFailure;
};

tnFlags tnFlagsReadOnly;

void tnSetFlags(float wheelbaseWidth, float wheelDiameter,
     char outLeft, char outRight, char posOutLine, char headingOutLine)
{
     tnFlagsReadOnly.leftMotor = outLeft;
     tnFlagsReadOnly.rightMotor = outRight;
     tnFlagsReadOnly.WheelBaseWidth = wheelbaseWidth;
     tnFlagsReadOnly.WheelDiameter = wheelDiameter;
     tnFlagsReadOnly.heading = 90.0;
     tnFlagsReadOnly.position =  v2New(0,0);
     tnFlagsReadOnly.posLine = posOutLine;
     tnFlagsReadOnly.headLine = headingOutLine;
     tnFlagsReadOnly.convergenceFailure = false;
}

void tnSetDefaults()
{
     tnFlagsReadOnly.WheelBaseWidth = 0.135;
     tnFlagsReadOnly.WheelDiameter = 0.057;
     tnFlagsReadOnly.leftMotor = OUT_A;
     tnFlagsReadOnly.rightMotor = OUT_C;
     tnFlagsReadOnly.heading = 90.0;
     tnFlagsReadOnly.position =  v2New(0,0);
     tnFlagsReadOnly.posLine = LCD_LINE1;
     tnFlagsReadOnly.headLine = LCD_LINE2;
     tnFlagsReadOnly.convergenceFailure = false;
}

tnFlags __updateHeadingPosition(tnFlags __flags__, short timeout)
{
     //calculate positions and distances in local coordinates
     float degL =
           (MotorTachoCount(__flags__.leftMotor) -
           __flags__.tacL);
     float degR =
           (MotorTachoCount(__flags__.rightMotor) -
            __flags__.tacR);
     float distL = PI * __flags__.WheelDiameter * degL / 360.0;
     float distR = PI * __flags__.WheelDiameter * degR / 360.0;
     
     TextOut(0, LCD_LINE4, NumToStr(distL) + "        " , DRAW_OPT_NORMAL);
     TextOut(0, LCD_LINE5, NumToStr(distR) + "        ", DRAW_OPT_NORMAL);

     //determine the current wheel position in local coors
     Vector2f TCur, TPrev, L0, L1, R0, R1, C0;
     L0 = v2New(__flags__.WheelBaseWidth / -2.0, 0);
     R0 = v2New(__flags__.WheelBaseWidth / 2.0, 0);
     L1 = v2New(L0.X, distL);
     R1 = v2New(R0.X, distR);
     C0 = __flags__.position;

     //In the special case where wheels barely move,
     //or the wheels move in sync so there is minimal turning,
     //we're simply going to add the distance and convert back to globals
     bool leftNearZero = abs(degL) <= 2.0;
     bool rightNearZero = abs(degR) <= 2.0;
     bool isTurning = (distL - distR) > (distL + distR) / 256.0;
     if((leftNearZero && rightNearZero) || !isTurning)
     {
          Vector2f C1 = v2Midpoint(L1,R1);
          __flags__.heading += v2AngleBetween(
                            v2Dif(L1,R1),
                            v2Dif(L0,R0));
          __flags__.position += v2RotateAbout(
                             v2Zero, C1,__flags__.heading);
          __flags__.tacL = MotorTachoCount(__flags__.leftMotor);
          __flags__.tacR = MotorTachoCount(__flags__.rightMotor);
          return __flags__;

     }

     //special case where one wheel is stationary
     //we only need one itteration, with the stationary wheel as pivot
     if(leftNearZero || rightNearZero)
     {
         if(leftNearZero)
         {
              TCur = L0;
              L1 = L0;
              float RadR = v2Dist(R0, TCur);
              R1 = v2Mult(v2FromAngle(180.0 * distR / (PI * RadR)), RadR);
              if(TCur.X >= R0.X)
                  R1 = v2ReflectY(R1);
              R1 = v2Dif(TCur, R1);
         }
         if(rightNearZero)
         {
              TCur = R0;
              R1 = R0;
              float RadL = v2Dist(L0, TCur);
              L1 = v2Mult(v2FromAngle(180.0 * distL / (PI * RadL)), RadL);
              if(TCur.X >= L0.X)
                  L1 = v2ReflectY(L1);
              L1 = v2Dif(TCur, L1);
         }

         Vector2f C1 = v2Midpoint(L1,R1);
         __flags__.position += v2RotateAbout(v2Zero,
                            C1, __flags__.heading);
         __flags__.heading += v2AngleBetween(
                            v2Dif(L1,R1),
                            v2Dif(L0,R0));

         __flags__.tacL = MotorTachoCount(__flags__.leftMotor);
         __flags__.tacR = MotorTachoCount(__flags__.rightMotor);
         return __flags__;
     }


     //Primary case where both wheels move a different non-zero amount
     //T is the turn pivot
     TCur = l2Intercept(l2NewPtPt(L0,R0), l2NewPtPt(L1,R1));
     TPrev = v2New(0,100);
     
     //Itterate until our turn point is accurate to within a small margin
     while(v2Dist(TCur,TPrev) > 1)
     {
           if(CurrentTick() > timeout)
           {
                __flags__.convergenceFailure = true;
                return __flags__;
           }

           TPrev = TCur;

          //circle radius to project distance onto
          float RadL = v2Dist(L0, TCur);
          float RadR = v2Dist(R0, TCur);

          //new endpoints from distance over arc,
          //rather than straight line
          L1 = v2Mult(v2FromAngle(180.0 * distL / (PI * RadL)), RadL);
          R1 = v2Mult(v2FromAngle(180.0 * distR / (PI * RadR)), RadR);

          if(TCur.X >= L0.X)
              L1 = v2ReflectY(L1);
          if(TCur.X >= R0.X)
              R1 = v2ReflectY(R1);

          L1.X += TCur.X;
          R1.X += TCur.X;

          TCur = l2Intercept(l2NewPtPt(L0,R0), l2NewPtPt(L1,R1));
     }
     //take the last itteration's end points as final
     //and update our flags
     Vector2f C1 = v2Midpoint(L1,R1);
     __flags__.position += v2RotateAbout(v2Zero,
                        C1, __flags__.heading);
     __flags__.heading += v2AngleBetween(
                       v2Dif(L1,R1), v2Dif(L0,R0));
     __flags__.tacL = MotorTachoCount(__flags__.leftMotor);
     __flags__.tacR = MotorTachoCount(__flags__.rightMotor);
     return __flags__;
}

task NavigateByTachometer()
{
     tnFlags flags = tnFlagsReadOnly;
     ResetAllTachoCounts(flags.leftMotor);
     ResetAllTachoCounts(flags.rightMotor);
     unsigned int time;
     unsigned int delta;
     while(true)
     {
          TextOut(0, flags.posLine,
              "P: " + v2AsString(flags.position) +
              "          " , DRAW_OPT_NORMAL);
          TextOut(0, flags.headLine, "H: " +
              NumToStr(flags.heading) +
              "          " , DRAW_OPT_NORMAL);
          time = CurrentTick();
          tnFlags temp = __updateHeadingPosition(flags, time + 50);
          if(temp.convergenceFailure)
              PlayTone(2000, 100);
          else
              flags = temp;
          delta = CurrentTick() - time;
          tnFlagsReadOnly = flags;
          if(delta < 25)
              Wait(25 - delta);

     }
}

void tnStop()
{
     StopTask(NavigateByTachometer);
}
#endif
