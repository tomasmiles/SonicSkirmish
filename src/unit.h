#ifndef UNIT_H
#define UNIT_H

#include "ofMain.h"
#include "math.h"
#include <vector>

class unit
{
    public:

        //Setters
        void rotate(float rotation);
        void rotateTowardsWaypoint(float maxRotation);

        void translate(float positionX, float positionY);
        void move(float speed);

        void setWaypoint(float positionX, float positionY);
        void addWaypoint(float positionX, float positionY);
        void removeWaypoint(int index);
        void clearWaypoint();
        void setWaypointBearing();


        void select();
        void deselect();

        void setTeam(int t);

        //Boolean methods
        bool isAtWaypoint();
        bool isInUnitBounds(float x, float y);

        //General Methods
        void draw();
        void update();
        float distance(float x, float y);
        float bearingDifference(float b);
        void setTeamColour(int c);

           //Members
        bool alive;
        int healthPoints;
        int team;
        ofColor teamColour;
        ofColor selectedColour;

        float bearing;
        float waypointBearing;
        float radius;
        float waypointRadius;
        ofVec2f position;
        vector <ofVec2f> waypoint;
        vector<int> attackWaypoint;
        vector<int> targetUnitIndex;

        int samples;
        float frequency;
        float amplitude;
        vector <float> wave;

        float missileTotal;

        float speed;
        bool selected;

        int cooldown;
        int cooldownMax;
        bool onCooldown;

        int missileStrength;
        int note;
        int attackRate;

        //ctor
        unit();
        unit(float posX, float posY, float b);
        //dtor
        virtual ~unit();

    protected:
    private:
};

#endif // UNIT_H
