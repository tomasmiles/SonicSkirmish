#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include "unit.h"
#include "missile.h"
#include <vector>

class entitySystem
{
    public:
        void draw();
        void update();

        void boxStart(float x, float y);
        void boxEnd(float x, float y);
        void cpuBoxStart(float x, float y);
        void cpuBoxEnd(float x, float y);
        void pointSelect(float x, float y, int playerTeam);
        void boxSelect();
        void cpuBoxSelect();
        void drawBoxSelect();
        void drawCpuBoxSelect();
        bool boxSelectActive;
        bool cpuBoxSelectActive;
        void assignControlGroup(int key);
        void selectControlGroup(int key);
        float boxStartX, boxStartY, boxEndX, boxEndY;
        float cpuBoxStartX, cpuBoxStartY, cpuBoxEndX, cpuBoxEndY;
        void orderUnit(float x, float y, float fx, float fy, int playerTeam);
        void orderUnitQueue(float x, float y, float fx, float fy, int playerTeam);
        void addMissile(int t, ofVec2f pos, ofVec2f vel, int index);
        void removeMissile(int m);

        bool blueTeamAlive();
        bool redTeamAlive();

        void setArena(float x, float y, float w, float h);

        vector <unit> units;
        vector <missile> missiles;

        vector <int> selectedUnitIndex;
        vector <int> cpuSelectedUnitIndex;

        vector <vector <int> > controlGroups;
        vector <vector <int> > cpuControlGroups;

        int humanTeam;
        int computerTeam;

        ofRectangle arena;

        entitySystem();
        entitySystem(vector <unit> u);
        virtual ~entitySystem();
    protected:
    private:
};

#endif // ENTITYSYSTEM_H
