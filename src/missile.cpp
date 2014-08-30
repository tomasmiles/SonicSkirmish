#include "missile.h"

void missile::draw()
{
    ofFill();
    ofSetColor(teamColour);
    ofCircle(position.x,position.y, radius);
}

void missile::update()
{
    translate(velocity);
}

void missile::translate(ofVec2f t)
{
    position = position+t;
}

//ctor
missile::missile(int t, ofVec2f pos, ofVec2f vel, int index)
{
    team = t;
    radius = 5;
    position = pos;
    velocity = vel;
    if(team == 0)teamColour = ofColor(100,100,255);
    if(team == 1)teamColour = ofColor(255,100,100);
    unitIndex = index;
}

missile::missile()
{
}

//dtor
missile::~missile()
{
}
