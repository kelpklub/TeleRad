#include "Motion.h"
void setup()
{
    Serial.begin(115200);
    initMotion();
}
void loop()
{
    updateMotion();
}