#ifndef MISSILE_H
#define MISSILE_H

#include "ofMain.h"
#include "math.h"
#include <vector>

class missile
{
    public:
        //Members
        int unitIndex;
        int team;
        ofColor teamColour;
        float radius;
        ofVec2f position;
        ofVec2f velocity;

        //Methods
        void draw();
        void update();

        void translate(ofVec2f t);

        //ctor
        missile(int t, ofVec2f pos, ofVec2f vel, int index);
        missile();
        //dtor
        virtual ~missile();
    protected:
    private:
};

#endif // MISSILE_H
