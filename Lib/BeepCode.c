struct beep
{
     unsigned int Freq;
     unsigned int Durr;
     unsigned int Delay;
};

beep newBeep(int freq, int durr, int delay)
{
     beep myBeep;
     myBeep.Freq = freq;
     myBeep.Durr = durr;
     myBeep.Delay = delay;
     return myBeep;
}

beep beeps[];

task playBeeps()
{
     for(int count = 0; count < ArrayLen(beeps); count++)
     {
          PlayTone(beeps[count].Freq, beeps[count].Durr);
          Wait(beeps[count].Delay);
     }
}


