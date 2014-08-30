#include "unit.h"

void unit::rotate(float rotation)
{
    bearing += rotation;
    if(bearing > 180|bearing < -180)
    {
        int semiCircles = (int)bearing/180;
        bearing = bearing-(float)(semiCircles*360);
    }
}

void unit::rotateTowardsWaypoint(float maxRotation)
{
    rotate(waypointBearing - bearing);
}

void unit::translate(float positionX, float positionY)
{
    position = ofVec2f(positionX, positionY);
}

void unit::move(float speed)
{
    ofVec2f down(0,1);
    ofVec2f moveDirection = down.getRotated(bearing)*speed;
    position += moveDirection;
    translate(position.x,position.y);
}

void unit::setWaypoint(float positionX, float positionY)
{
    clearWaypoint();
    waypoint[0] = ofVec2f(positionX, positionY);
}

void unit::addWaypoint(float positionX, float positionY)
{
    waypoint.push_back(ofVec2f(positionX, positionY));
}

void unit::removeWaypoint(int index)
{
    waypoint.erase(waypoint.begin()+index);
}

void unit::clearWaypoint()
{
    waypoint.erase(waypoint.begin(), waypoint.end());
    waypoint.push_back(ofVec2f(position.x,position.y));
}

void unit::setWaypointBearing()
{
    waypointBearing = ofVec2f(0,1).angle(waypoint[0] - position);
}

void unit::select()
{
    selected = true;
}

void unit::deselect()
{
    selected = false;
}

void unit::setTeam(int t)
{
    team = t;
    if(team == 0)
    {
        teamColour = ofColor(0,0,255);
        selectedColour = ofColor(0,255,0);
    }
    if(team == 1)
    {
        teamColour = ofColor(255,0,0);
        selectedColour = ofColor(255,0,255);
    }
}

bool unit::isAtWaypoint()
{
    return(position.distance(waypoint[0]) < waypointRadius);
}

bool unit::isInUnitBounds(float x, float y)
{
    if(ofVec2f(x,y).distance(position)<radius+amplitude)
    {
        return true;
    }
    else return false;
}

void unit::draw()
{
    ofPushMatrix();
    ofTranslate(position.x,position.y);
    ofFill();
    ofSetColor(teamColour);
    ofBeginShape();
    for(int i = 0; i < 360; i++)ofVertex(cos((bearing+i+90)/360*2*4*atan(1))*(radius+wave[i]),
                                         sin((bearing+i+90)/360*2*4*atan(1))*(radius+wave[i]));

    ofEndShape(true);
    if(selected)
    {
        ofNoFill();
        ofSetColor(selectedColour);
         ofBeginShape();
    for(int i = 0; i < 360; i++)ofVertex(sin((bearing+i)/360*2*4*atan(1))*radius,
                                         cos((bearing+i)/360*2*4*atan(1))*radius);

    ofEndShape(true);
    }

    ofSetColor(ofColor(0,0,255));
    ofLine(0,0,
           sin((-bearing+90)/360*2*4*atan(1))*radius,cos((-bearing+90)/360*2*4*atan(1))*radius);
           ofSetColor(ofColor(255,0,0));
    ofLine(0,0,
           sin((-bearing-90)/360*2*4*atan(1))*radius,cos((-bearing-90)/360*2*4*atan(1))*radius);

    ofPopMatrix();

    //Draw bearing and way point
    ofSetColor(ofColor(255,255,255));
    for(int j = 0; j < waypoint.size(); j++)
    {
        if(selected)
        {
            ofNoFill();
            if(team == 0)ofSetColor(0,255,0);
            if(team == 1)ofSetColor(255,0,255);
            ofCircle(waypoint[j].x, waypoint[j].y, 10);
        }
    }
    ofSetColor(ofColor(0,255,0));
    ofLine(position.x,position.y,position.x+(ofVec2f(0,1).getRotated(bearing).x)*radius*2,position.y+(ofVec2f(0,1).getRotated(bearing).y)*radius*2);
    //ofDrawBitmapString(ofToString(healthPoints), position.x, position.y);
    ofSetColor(ofColor(255,255,255));
}

float unit::distance(float x, float y)
{
    return position.distance(ofVec2f(x,y));
}

float unit::bearingDifference(float b)
{
return b - bearing;
}

unit::unit(float posX, float posY, float b)
{
    //ctor
    alive = true;
    healthPoints = 50;
    team = 0;
    teamColour = ofColor(0,255,0);
    selectedColour = ofColor(0,255,0);
    bearing = b;
    waypointBearing = 0;
    radius = 20;
    waypointRadius = 5;
    position = ofVec2f(posX,posY);
    waypoint.resize(1);
    waypoint[0] = ofVec2f(position.x,position.y);
    selected = false;

    missileStrength = 1;
    note = 0;

    speed = 4;

    samples = 360;
    frequency = ofRandom(20,440);
    //frequency = 220;
    amplitude = 5;
    wave.resize(samples);
    //for(float i = 0; i < samples; i++){wave[i] = sin(i*frequency/360*2*4*atan(1))*amplitude;}
    for(float i = 0; i < samples; i++){wave[i] = amplitude;}

    cooldown = 0;
    cooldownMax = 50;
    onCooldown = false;
    missileTotal = 0;
}

unit::unit()
{
    //ctor
    alive = true;
    healthPoints = 50;
    team = 0;
    teamColour = ofColor(0,0,255);
    selectedColour = ofColor(0,255,0);
    bearing = 0;
    waypointBearing = 0;
    radius = 20;
    waypointRadius = 5;
    position = ofVec2f(ofRandom(0,800),ofRandom(0,600));
    waypoint.resize(1);
    waypoint[0] = ofVec2f(position.x,position.y);
    selected = false;

    missileStrength = 1;
    note = 0;

    speed = 2;

    samples = 360;
    frequency = ofRandom(60,880);
    //frequency = 220;
    amplitude = 5;
    wave.resize(samples);
    for(float i = 0; i < samples; i++){wave[i] = sin(i*frequency/360*2*4*atan(1))*amplitude;}

    cooldown = 0;
    cooldownMax = 50;
    onCooldown = false;
    missileTotal = 0;
}

unit::~unit()
{
    //dtor
}
