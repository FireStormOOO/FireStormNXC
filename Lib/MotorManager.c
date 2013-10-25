#include "Lib/LineLib.c"
#include "Lib/LibV2.c"
#include "Lib/TachoNav.c"

void mmTurnToHeading(float newHeading)
{
    float degreeTurn = newHeading - tnFlagsReadOnly.heading;
    degreeTurn %= 360;
    if(degreeTurn > 180.0)
        degreeTurn -= 360.0;
    bool turnLeft = 1 == sign(degreeTurn);
    TextOut(0, LCD_LINE1, "Obj:" +
        NumToStr(newHeading), DRAW_OPT_CLEAR);

    while(abs(tnFlagsReadOnly.heading - newHeading) >= 2)
    {
         int delta = tnFlagsReadOnly.heading - newHeading;
         delta %= 360;
         if(delta > 180)
             delta -=360;
         if(abs(delta) > 30)
             delta = sign(delta) * 30;
         char pwr = delta * 3 + sign(delta) * 10;
         
         //TextOut(0, LCD_LINE2, "Cur:" +
         //    NumToStr(tnFlagsReadOnly.heading) +
         //    "        ", DRAW_OPT_NORMAL);
         TextOut(0, LCD_LINE3, "Del: " +
             NumToStr(delta) +
             "        ", DRAW_OPT_NORMAL);
         //OnFwd(OUT_A, pwr);
         //OnFwd(OUT_C, -pwr);
         OnFwdRegEx(OUT_A, pwr, OUT_REGMODE_SPEED, RESET_NONE);
         OnFwdRegEx(OUT_C, -pwr, OUT_REGMODE_SPEED, RESET_NONE);
         Wait(25);
    }
    Off(OUT_AC);
    Wait(100);
}



void mmMoveForwardDist(float distance)
{
    Vector2f endPos =
        v2Add(tnFlagsReadOnly.position, v2Mult(
           v2FromAngle(tnFlagsReadOnly.heading),
           distance));
    while(v2Dist(tnFlagsReadOnly.position, endPos) > 3.0)
    {
         float curHeading = tnFlagsReadOnly.heading;
         Vector2f curPos = tnFlagsReadOnly.position;
         float angularError =
            v2Angle(v2Dif(endPos, curPos)) - curHeading;
         if(abs(angularError) > 15)
            mmTurnToHeading(v2Angle(v2Dif(endPos, curPos)));
         char pwr;
         float remaining = v2Dist(curPos, endPos);
         if(remaining > 30)
            pwr = 100;
         else
            pwr = 3 * remaining + 10;
            
         //TextOut(0, LCD_LINE1, "O:" +
         //    v2AsString(endPos), DRAW_OPT_CLEAR);
         //TextOut(0, LCD_LINE2, "C:" +
         //    v2AsString(curPos), DRAW_OPT_NORMAL);
         TextOut(0, LCD_LINE2, "d:" +
             v2AsString(v2Dif(endPos, curPos)), DRAW_OPT_NORMAL);

         OnFwdSyncEx(OUT_AC, pwr, 2 * angularError, RESET_NONE);
         Wait(25);
    }
    Off(OUT_AC);
    Wait(100);
}

void mmMoveToPt(Vector2f dest)
{
     Vector2f diff = v2Dif(dest, tnFlagsReadOnly.position);
     float neededHeading = v2Angle(diff);
     mmTurnToHeading(neededHeading);
     mmMoveForwardDist(v2Length(diff));
}
