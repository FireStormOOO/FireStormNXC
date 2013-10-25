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

     //theta is in RADIANS
     float theta = -(distL - distR) / __flags__.WheelBaseWidth;

     if((theta < PI / 18.0)//2.5 degrees
     {
          Vector2f C1 = v2Midpoint(L1,R1);
          __flags__.position += v2RotateAbout(
                             v2Zero, C1, 90 * theta / PI);
          __flags__.heading += 180 * theta / PI;
          __flags__.tacL = MotorTachoCount(__flags__.leftMotor);
          __flags__.tacR = MotorTachoCount(__flags__.rightMotor);
          return __flags__;

     }


     //Primary case where both wheels move a different non-zero amount
     //T is the turn pivot
     TCur = l2Intercept(l2NewPtPt(L0,R0), l2NewPtPt(L1,R1));
     TPrev = v2New(0,100);
     
     float radL, radR;
     radL = distL * __flags__.WheelBaseWidth / (distL - distR);
     radR = distR * __flags__.WheelBaseWidth / (distL - distR);


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
