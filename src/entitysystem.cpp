#include "entitysystem.h"

void entitySystem::update()
{
    float maxRadius = 0;
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].alive)
        {
            if(units[i].cooldown < 0)
            {
                units[i].cooldown = 0;
                units[i].onCooldown = false;
            }else if(units[i].cooldown == 0)
            {
                units[i].onCooldown = false;
            }else if(units[i].cooldown > 0)
            {
                units[i].cooldown--;
            }
            if(units[i].radius > maxRadius)maxRadius = units[i].radius;

            //If the unit hasn't reached its waypoint
            if(!units[i].isAtWaypoint())
            {
                units[i].setWaypointBearing();
                units[i].rotateTowardsWaypoint(1);
                float distanceToWaypoint = units[i].distance(units[i].waypoint[0].x, units[i].waypoint[0].y);
                if(units[i].speed > distanceToWaypoint)
                {
                    units[i].move(distanceToWaypoint);
                } else
                units[i].move(units[i].speed);
            } else
            //If the unit has reached the waypoint
            {
                bool missileLaunched = false;
                //Check if the current waypoint index is an attack waypoint index
                //Fire a missile at the target unit's current position if it is an attack waypoint
                if(units[i].attackWaypoint.size() > 0)
                {
                    if(units[i].attackWaypoint[0] == 0)
                    {
                        if(!units[i].onCooldown)
                        {
                            missileLaunched = true;
                            addMissile(units[i].team, units[i].position,
                                (units[units[i].targetUnitIndex[0]].position - units[i].position).normalized()*3, selectedUnitIndex[i]);
                            units[i].cooldown = units[i].cooldownMax;
                            units[i].onCooldown = true;
                            if(units[i].waypoint.size()>1)units[i].waypoint.erase(units[i].waypoint.begin());
                            //units[i].attackWaypoint.erase(units[i].attackWaypoint.begin());
                            //units[i].targetUnitIndex.erase(units[i].targetUnitIndex.begin());
                            //for(int b = 0; b < units[i].attackWaypoint.size(); b++)units[i].attackWaypoint[b]--;
                        }else
                        //is on cooldown
                        {

                        }
                    }

                }else if(units[i].waypoint.size()>1)
                {
                    units[i].waypoint.erase(units[i].waypoint.begin());
                    for(int a = 0; a < units[i].attackWaypoint.size(); a++)
                    {
                        units[i].attackWaypoint[a]--;
                    }
                }
            }
        }
    }
    for(int j = 1; j < units.size(); j++)
    {
        for(int k = 0; k < units.size()-j; k++)
        {
            if(units[j-1].alive && units[j+k].alive)
            {
                if(units[j-1].distance(units[j+k].position.x,units[j+k].position.y) < maxRadius)
                {
                    ofVec2f backtrack1 = units[j-1].position.getInterpolated(units[j+k].position,-0.5);
                    ofVec2f backtrack2 = units[j+k].position.getInterpolated(units[j-1].position,-0.5);

                    units[j-1].translate(backtrack1.x,backtrack1.y);
                    units[j+k].translate(backtrack2.x,backtrack2.y);

                    float oldBearing1 = units[j-1].bearing;
                    float oldBearing2 = units[j+k].bearing;

                    //units[j-1].setWaypoint(units[j-1].position.x, units[j-1].position.y);
                    //units[j+k].setWaypoint(units[j+k].position.x, units[j+k].position.y);

                    units[j-1].bearing = oldBearing1;
                    units[j+k].bearing = oldBearing2;
                }
            }
        }
    }
    for(int l = 0; l < missiles.size(); l++)
    {
        if(missiles[l].position.x < arena.getLeft() ||
           missiles[l].position.x > arena.getRight() ||
           missiles[l].position.y < arena.getTop() ||
           missiles[l].position.y > arena.getBottom())removeMissile(l);
        missiles[l].update();
        for(int m = 0; m < units.size(); m++)
        {
            if(units[m].alive)
            {
                if(!missiles[l].team == units[m].team)
                {
                    if(units[m].isInUnitBounds(missiles[l].position.x, missiles[l].position.y))
                    {
                        ofVec2f down = ofVec2f(0,1);
                        float contactAngle = down.angle((missiles[l].position - units[m].position).normalized()*4);
                        //units[m].frequency*= 1.5;
                        //if(units[m].frequency <= 20) units[m].frequency = 20;
                        if(units[m].healthPoints <= 0)units[m].alive = false;
                        if(units[m].alive)units[m].healthPoints-=units[missiles[l].unitIndex].missileStrength;
                        removeMissile(l);
                    }
                }
            }
        }
    }
}

void entitySystem::draw()
{
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].alive)
        {
            units[i].draw();
            ofSetColor(255);
            ofDrawBitmapString(ofToString(i), units[i].position.x, units[i].position.y);
            ofFill();
            ofSetColor(0,255,0);
            ofRect(units[i].position.x, units[i].position.y -30, 50, 10);
            ofSetColor(255,0,0);
            ofRect(units[i].position.x + 50, units[i].position.y - 30, units[i].healthPoints-50, 10);
            ofSetColor(0,255,255);
            ofRect(units[i].position.x - 50, units[i].position.y + 30, (float(units[i].cooldown)/float(units[i].cooldownMax))*50, 10);
            ofSetColor(255);
            ofDrawBitmapString(ofToString(units[i].healthPoints)+ " HP", units[i].position.x, units[i].position.y - 20);
        }
    }
    for(int j = 0; j < missiles.size(); j++)
    {
        missiles[j].draw();
    }
    if(boxSelectActive)drawBoxSelect();
    if(cpuBoxSelectActive)drawCpuBoxSelect();
}

void entitySystem::pointSelect(float x, float y, int playerTeam)
{
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].alive)
        {
            if(units[i].isInUnitBounds(x,y) && units[i].team == playerTeam)
            {
                if(playerTeam == humanTeam)
                {
                    bool unitSelected = false;
                    for(int j = 0; j < selectedUnitIndex.size(); j++)
                    {
                        if(selectedUnitIndex[j] == i) unitSelected = true;
                    }
                    if(!unitSelected)
                    {
                        selectedUnitIndex.erase(selectedUnitIndex.begin(),selectedUnitIndex.end());
                        units[i].select();
                        selectedUnitIndex.push_back(i);
                    }
                }else if(playerTeam == computerTeam)
                {
                    bool unitSelected = false;
                    for(int j = 0; j < cpuSelectedUnitIndex.size(); j++)
                    {
                        if(cpuSelectedUnitIndex[j] == i) unitSelected = true;
                    }
                    if(!unitSelected)
                    {
                        cpuSelectedUnitIndex.erase(cpuSelectedUnitIndex.begin(),cpuSelectedUnitIndex.end());
                        units[i].select();
                        cpuSelectedUnitIndex.push_back(i);
                    }
                }
            }
        }
    }
}

void entitySystem::boxSelect()
{
    int playerTeam = 0;
    selectedUnitIndex.erase(selectedUnitIndex.begin(),selectedUnitIndex.end());
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].alive)
        {
            bool withinX = false;
            bool withinY = false;
            if((units[i].position.x > boxStartX && units[i].position.x < boxEndX) |
            (units[i].position.x < boxStartX && units[i].position.x > boxEndX))
            withinX = true;
            if((units[i].position.y > boxStartY && units[i].position.y < boxEndY) |
            (units[i].position.y < boxStartY && units[i].position.y > boxEndY))
            withinY = true;
            if(withinX && withinY && units[i].team == playerTeam)
            {
                units[i].select();
                int j = 0;
            }
            else
            {
                if(units[i].team == playerTeam)
                {
                    units[i].deselect();
                }
            }
        }
    }
    for(int l = 0; l < units.size(); l++)if(units[l].team == playerTeam && units[l].selected == true)
    {
        selectedUnitIndex.push_back(l);
    }
}

void entitySystem::cpuBoxSelect()
{
    int playerTeam = 1;
        cpuSelectedUnitIndex.erase(cpuSelectedUnitIndex.begin(),cpuSelectedUnitIndex.end());
        for(int i = 0; i < units.size(); i++)
        {
            if(units[i].alive && units[i].team == playerTeam)
            {
                bool withinX = false;
                bool withinY = false;
                if((units[i].position.x > cpuBoxStartX && units[i].position.x < cpuBoxEndX) |
                (units[i].position.x < cpuBoxStartX && units[i].position.x > cpuBoxEndX))
                withinX = true;
                if((units[i].position.y > cpuBoxStartY && units[i].position.y < cpuBoxEndY) |
                (units[i].position.y < cpuBoxStartY && units[i].position.y > cpuBoxEndY))
                withinY = true;
                if(withinX && withinY)
                {
                    units[i].select();
                    int j = 0;
                }
                else
                {
                    units[i].deselect();
                }
            }
        }
            for(int l = 0; l < units.size(); l++)if(units[l].selected == true && units[l].team == playerTeam)
            {
                cpuSelectedUnitIndex.push_back(l);
            }
}

void entitySystem::assignControlGroup(int key)
{
    if(key == '1'||key == '2'||key == '3'||key == '4'||key == '5'||key == '6'||key == '7'||key == '8'||key == '9'||key == '0')
    {
        //cout << key-48 << "\n";
        controlGroups[key-48].resize(selectedUnitIndex.size());
        for(int i = 0; i < selectedUnitIndex.size(); i++)
        {
            controlGroups[key-48][i] = selectedUnitIndex[i];
        }
    }
}

void entitySystem::selectControlGroup(int key)
{
    if(key == '1'||key == '2'||key == '3'||key == '4'||key == '5'||key == '6'||key == '7'||key == '8'||key == '9'||key == '0')
    {
        //cout << key-48 << "\n";
        for(int a = 0; a < selectedUnitIndex.size(); a++)units[selectedUnitIndex[a]].deselect();
        selectedUnitIndex.resize(controlGroups[key-48].size());
        selectedUnitIndex = controlGroups[key-48];
        for(int b = 0; b < selectedUnitIndex.size(); b++)units[selectedUnitIndex[b]].select();
    }
}

void entitySystem::drawBoxSelect()
{
    ofSetColor(0,255,0);
    ofNoFill();
    ofRect(boxStartX, boxStartY, boxEndX-boxStartX, boxEndY-boxStartY);
}

void entitySystem::drawCpuBoxSelect()
{
    ofSetColor(255,0,255);
    ofNoFill();
    ofRect(cpuBoxStartX, cpuBoxStartY, cpuBoxEndX-cpuBoxStartX, cpuBoxEndY-cpuBoxStartY);
}

void entitySystem::boxStart(float x, float y)
{
    boxStartX = x;
    boxStartY = y;
}

void entitySystem::boxEnd(float x, float y)
{
    boxEndX = x;
    boxEndY = y;
}

void entitySystem::cpuBoxStart(float x, float y)
{
    cpuBoxStartX = x;
    cpuBoxStartY = y;
}

void entitySystem::cpuBoxEnd(float x, float y)
{
    cpuBoxEndX = x;
    cpuBoxEndY = y;
}

void entitySystem::orderUnit(float x, float y, float fx, float fy, int playerTeam)
{
    if(playerTeam == 0)
    {
        int target;
        bool moving = true;
        for(int j = 0; j < units.size(); j++)
        {
            if(units[j].alive)
            {
                if(units[j].isInUnitBounds(x, y) && !units[j].team == humanTeam)
                {
                    moving = false;
                    target = j;
                    break;
                }
            }
        }
        if(!moving)
        {
            for(int i = 0; i < selectedUnitIndex.size(); i++)
            {
                //Assume the missile won't be launched
                bool missileLaunched = false;
                //Set the waypoint to the current position, and clear attack waypoints and target units
                units[selectedUnitIndex[i]].setWaypoint(units[selectedUnitIndex[i]].position.x,units[selectedUnitIndex[i]].position.y);
                units[selectedUnitIndex[i]].attackWaypoint.erase(units[selectedUnitIndex[i]].attackWaypoint.begin(),units[selectedUnitIndex[i]].attackWaypoint.end());
                units[selectedUnitIndex[i]].targetUnitIndex.erase(units[selectedUnitIndex[i]].targetUnitIndex.begin(),units[selectedUnitIndex[i]].targetUnitIndex.end());

                if(!units[selectedUnitIndex[i]].onCooldown)
                {
                    //Fire the missile
                    missileLaunched = true;
                    addMissile(units[selectedUnitIndex[i]].team,
                        units[selectedUnitIndex[i]].position,
                        (units[target].position - units[selectedUnitIndex[i]].position).normalized()*3, selectedUnitIndex[i]);
                    units[selectedUnitIndex[i]].cooldown = units[cpuSelectedUnitIndex[i]].cooldownMax;
                    units[selectedUnitIndex[i]].onCooldown = true;
                }else
                {
                    if(units[selectedUnitIndex[i]].attackWaypoint.size()>0)units[selectedUnitIndex[i]].attackWaypoint.push_back(0);
                    if(units[selectedUnitIndex[i]].targetUnitIndex.size()>0)units[selectedUnitIndex[i]].targetUnitIndex.push_back(target);
                }
            }
        }else if(moving)
        {
            float averageBearing = 0;
            vector <ofVec2f> formation(selectedUnitIndex.size());
            int offset = formation.size()/2;
            ofVec2f formationLine;
            for(int k = 0; k < selectedUnitIndex.size(); k++)averageBearing += (units[selectedUnitIndex[k]].waypointBearing);
            averageBearing = averageBearing/selectedUnitIndex.size();
            if(fx == x && fy == y) formationLine = ofVec2f(0,1).getRotated(averageBearing).getPerpendicular();
            else formationLine = ofVec2f(fx-x,fy-y).getPerpendicular();
            ofVec2f formationSpacing = formationLine*(units[0].radius*2);
            for(int l = 0; l < formation.size(); l++)
            {
                formation[l] = ofVec2f(x+(formationSpacing.x*(l-offset)),y+(formationSpacing.y*(l-offset)));
                if(formation[l].x < arena.getLeft())formation[l].x = arena.getLeft();
                else if(formation[l].x > arena.getRight())formation[l].x = arena.getRight();
                if(formation[l].y < arena.getTop())formation[l].y = arena.getTop();
                else if(formation[l].y > arena.getBottom())formation[l].y = arena.getBottom();
                units[selectedUnitIndex[l]].setWaypoint(formation[l].x,formation[l].y);
                units[selectedUnitIndex[l]].attackWaypoint.erase(units[selectedUnitIndex[l]].attackWaypoint.begin(),units[selectedUnitIndex[l]].attackWaypoint.end());
                units[selectedUnitIndex[l]].targetUnitIndex.erase(units[selectedUnitIndex[l]].targetUnitIndex.begin(),units[selectedUnitIndex[l]].targetUnitIndex.end());
            }
        }
    }

        if(playerTeam == 1)
    {
        bool missileLaunched = false;
        bool moving = true;
        for(int i = 0; i < cpuSelectedUnitIndex.size(); i++)
        {
            if(units[cpuSelectedUnitIndex[i]].alive)
            {
                for(int j = 0; j < units.size(); j++)
                {
                    if(units[j].alive)
                    {
                        if(units[j].isInUnitBounds(x, y) && !units[j].team == units[cpuSelectedUnitIndex[i]].team)
                        {
                            moving = false;
                            if(!units[cpuSelectedUnitIndex[i]].onCooldown)
                            {
                                missileLaunched = true;
                                addMissile(units[cpuSelectedUnitIndex[i]].team,
                                    units[cpuSelectedUnitIndex[i]].position,
                                    (units[j].position - units[cpuSelectedUnitIndex[i]].position).normalized()*3, cpuSelectedUnitIndex[i]);
                                units[cpuSelectedUnitIndex[i]].cooldown = units[cpuSelectedUnitIndex[i]].cooldownMax;
                                units[cpuSelectedUnitIndex[i]].onCooldown = true;
                            }
                        }
                    }
                }
            }
        }
        if(moving)
        {
            float averageBearing = 0;
            vector <ofVec2f> formation(cpuSelectedUnitIndex.size());
            int offset = formation.size()/2;
            for(int k = 0; k < cpuSelectedUnitIndex.size(); k++)averageBearing += (units[cpuSelectedUnitIndex[k]].waypointBearing);
            averageBearing = averageBearing/cpuSelectedUnitIndex.size();
            ofVec2f formationLine = ofVec2f(0,1).getRotated(averageBearing).getPerpendicular();
            ofVec2f formationSpacing = formationLine*(units[0].radius*2);
            for(int l = 0; l < formation.size(); l++)
            {
                formation[l] = ofVec2f(x+(formationSpacing.x*(l-offset)),y+(formationSpacing.y*(l-offset)));
                if(formation[l].x < arena.getLeft())formation[l].x = arena.getLeft();
                else if(formation[l].x > arena.getRight())formation[l].x = arena.getRight();
                if(formation[l].y < arena.getTop())formation[l].y = arena.getTop();
                else if(formation[l].y > arena.getBottom())formation[l].y = arena.getBottom();
                units[cpuSelectedUnitIndex[l]].setWaypoint(formation[l].x,formation[l].y);
            }
        }else if(!moving)
        {
            for(int n = 0; n < cpuSelectedUnitIndex.size(); n++)
            {
                units[cpuSelectedUnitIndex[n]].setWaypoint(units[cpuSelectedUnitIndex[n]].position.x,units[cpuSelectedUnitIndex[n]].position.y);
            }
        }
    }
}

void entitySystem::orderUnitQueue(float x, float y, float fx, float fy, int playerTeam)
{

    //Go through all the selected units
    for(int i = 0; i < selectedUnitIndex.size(); i++)
    {
        //Assume the order to queue is a move order for now
        bool missileLaunched = false;
      //If the order is an attack, we need the index of the target
        int target;
        for(int j = 0; j < units.size(); j++)
        {
            //If the order is in the bounds of an enemy unit, use that unit's index as the target index
            //and flag an attack
            if(units[j].alive)
            {
                if(units[j].isInUnitBounds(x, y) && !units[j].team == units[selectedUnitIndex[i]].team)
                {
                    missileLaunched = true;
                    target = j;
                }
            }
        }
        //If there is no attack order, simply add a movement waypoint onto the order queue
        if(!missileLaunched)
            {
                float averageBearing = 0;
                ofVec2f formation;
                int offset = selectedUnitIndex.size()/2;
                ofVec2f formationLine;
                ofVec2f lastWaypoint = units[selectedUnitIndex[i]].waypoint[units[selectedUnitIndex[i]].waypoint.size()-1];
                if(fx == x && fy == y) formationLine = (ofVec2f(x,y)-units[selectedUnitIndex[i]].position).getPerpendicular();
                else formationLine = ofVec2f(fx-x,fy-y).getPerpendicular();
                ofVec2f formationSpacing = formationLine*(units[0].radius*2);
                formation = ofVec2f(x+(formationSpacing.x*(i-offset)),y+(formationSpacing.y*(i-offset)));

                if(formation.x < arena.getLeft())formation.x = arena.getLeft();
                else if(formation.x > arena.getRight())formation.x = arena.getRight();
                if(formation.y < arena.getTop())formation.y = arena.getTop();
                else if(formation.y > arena.getBottom())formation.y = arena.getBottom();

                units[selectedUnitIndex[i]].addWaypoint(formation.x,formation.y);
            }else
            //If there an attack order:
            if(missileLaunched)
            {
            //Add another waypoint at the position of the previous one
            //units[selectedUnitIndex[l]].addWaypoint(
                //units[selectedUnitIndex[l]].waypoint[units[selectedUnitIndex[l]].waypoint.size()-1].x,
                //units[selectedUnitIndex[l]].waypoint[units[selectedUnitIndex[l]].waypoint.size()-1].y);

            units[selectedUnitIndex[i]].attackWaypoint.push_back(units[selectedUnitIndex[i]].waypoint.size()-1);
            units[selectedUnitIndex[i]].targetUnitIndex.push_back(target);

            //units[selectedUnitIndex[l]].addWaypoint(
                //units[selectedUnitIndex[l]].waypoint[units[selectedUnitIndex[l]].waypoint.size()-1].x,
                //units[selectedUnitIndex[l]].waypoint[units[selectedUnitIndex[l]].waypoint.size()-1].y);
        }
    }
}

void entitySystem::addMissile(int t, ofVec2f pos, ofVec2f vel, int index)
{
    missiles.push_back(missile(t, pos, vel, index));
    units[index].missileTotal++;
}

void entitySystem::removeMissile(int m)
{
    units[missiles[m].unitIndex].missileTotal--;
    missiles.erase(missiles.begin()+m);
}

bool entitySystem::blueTeamAlive()
{
    int totalBlue = 0;
    int totalDead = 0;
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].team == 0)
        {
            totalBlue++;
            if(!units[i].alive)totalDead++;
        }
    }
    if(totalBlue - totalDead > 0)
    {
        return true;
    } else
    {
        return false;
    }
}

bool entitySystem::redTeamAlive()
{
       int totalRed = 0;
    int totalDead = 0;
    for(int i = 0; i < units.size(); i++)
    {
        if(units[i].team == 1)
        {
            totalRed++;
            if(!units[i].alive)totalDead++;
        }
    }
    if(totalRed - totalDead > 0)
    {
        return true;
    } else
    {
        return false;
    }

}

void entitySystem::setArena(float x, float y, float w, float h)
{
    arena = ofRectangle(x, y, w, h);
}

entitySystem::entitySystem()
{
    //ctor
}

entitySystem::entitySystem(vector <unit> u)
{
    //ctor
    units = u;
    selectedUnitIndex.resize(units.size());
    cpuSelectedUnitIndex.resize(units.size());
    boxStartX = 0;
    boxStartY = 0;
    boxEndX = 0;
    boxEndY = 0;
    boxSelectActive = false;
    cpuBoxSelectActive = false;
    humanTeam = 0;
    computerTeam = 1;

    controlGroups.resize(10);
}

entitySystem::~entitySystem()
{
    //dtor
}
