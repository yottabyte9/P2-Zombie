#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
using namespace std;

int quiver;


class Zombie{
    public:
        string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;
        int rounds;
};

bool operator< (Zombie& z1, Zombie& z2){ //return true if z1 has higher priority (lower eta)
    if(eta(z1) < eta(z2)){
        return true;
    }
    else if(eta(z2) < eta(z1)){
        return false;
    }
    else{ //same eta
        if(z1.health < z2.health){
            return true;
        }
        else if(z1.health > z2.health){
            return false;
        }
    }
    return z1.name < z2.name;
}

void updateDist(Zombie &zombie){
    zombie.distance = min(0, int(zombie.distance-zombie.speed));
    if(zombie.distance == 0){
        //end game;
    }
}

int eta(Zombie &zombie){
    return zombie.distance/zombie.speed;
}

void shoot(Zombie &zombie, int current_quiver){
    if(zombie.health > current_quiver){
        zombie.health -= current_quiver;
    }
    else{
        zombie.health = 0;
        //remove zombie
        current_quiver -= zombie.health;
    }
}

main(){
    
}