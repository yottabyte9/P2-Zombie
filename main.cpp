#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <queue>
#include <getopt.h>
#include <fstream>
#include <map>
#include "P2random.h"

using namespace std;

uint32_t quiver;
int nround = 0;
int rand_seed;
int rand_distance, rand_speed, rand_health;
bool keep_going = true;

int rand_num;
int named_num;

struct Zombie{
    public:
        string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;
        uint32_t round;
        Zombie(string n, uint32_t d, uint32_t s, uint32_t h, uint32_t r){
            name = n;
            distance = d;
            speed = s;
            health = h;
            round = r;
        }
};

int eta(const Zombie* zombie);
void end_sequence();

struct Comparator{
    bool operator()(const Zombie* z2, const Zombie* z1) const{ //return true if z1 has higher priority (lower eta)
        if(eta(z1) < eta(z2)){
            return true;
        }
        else if(eta(z2) < eta(z1)){
            return false;
        }
        else{ //same eta
            if((*z1).health < (*z2).health){
                return true;
            }
            else if((*z1).health > (*z2).health){
                return false;
            }
        }
        return (*z1).name < (*z2).name;
    }
};

priority_queue<Zombie*, vector<Zombie*>, Comparator> pq; 
deque<Zombie*> master;
deque<Zombie*> dead;


int eta(const Zombie* zombie){
    return (*zombie).distance/(*zombie).speed;
}

int shoot(Zombie *zombie, uint32_t current_quiver){
    cout << "shooting: " << zombie->name << "\n";
    if(zombie->health > current_quiver){
        zombie->health -= current_quiver;
        current_quiver = 0;
    }
    else{
        zombie->health = 0;
        //remove zombie
        cout << "killed: " << zombie->name << "\n";
        current_quiver -= zombie->health;
        dead.push_back(zombie);
        pq.pop();
    }
    return current_quiver;
}

void round(int round){
    int current_quiver = quiver;
    cout << "current round: " << round << "\n";

    while(current_quiver > 0 && !pq.empty()){
        current_quiver = shoot(pq.top(), current_quiver);
    }
}

void move_forward(){ 
    for(auto zombie: master){
        if(zombie->health == 0){
            return;
        }
        cout << "moving: " << zombie->name << "\n";
        zombie->distance = max(0, int(zombie->distance-zombie->speed));
        if(zombie->distance == 0){
            keep_going = false;
            cout << "killed by: " << zombie->name << "\n";
        }
    }
}

void cl(int argc, char** argv){
    //find round
    //put zombies into pq
    //start round
    //shoot zombies

    int gotopt;
    int option_index = 0;
    option long_opts[]={
        {"verbose", no_argument, nullptr, 'v'},
        {"statistics", required_argument, nullptr, 's'},
        {"median", no_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {0, 0, 0, 0}
    };

    uint32_t num;
    bool s, m, v = false;

    while((gotopt = getopt_long(argc, argv, "vs:mh", long_opts, &option_index)) != -1){
        switch(gotopt){
            case 'h':
                cout << "help";
                exit(0);
            case 's':
                s = true;
                num = atoi(optarg);
                break;
            case 'm':
                m = true;
                break;
            case 'v':
                v = true;
                break;
            default:
                cout << "default error\n";
                exit(1);
        }
    }
    cout << num << s << m << v << "\n";
}

void fill_initial(){
    string junk;
    string line;
    
    if(cin.peek() == '#'){
        while(cin.peek() != '\n'){
            cin >> line;
        }
    }
    cin >> line; //quiver-capacity:
    cin >> line;
    quiver = stoi(line);
    cin >> line; // random-seed:
    cin >> line;
    rand_seed = stoi(line);
    cin >> line; //max-rand-distance:
    cin >> line;
    rand_distance = stoi(line);
    cin >> line; //max-rand-speed:
    cin >> line;
    rand_speed = stoi(line);
    cin >> line; //max-rand-health:
    cin >> line;
    rand_health = stoi(line);
    cout << quiver << ", " << rand_seed << ", " << rand_distance << ", " << rand_speed << ", " << rand_health << "\n";
    P2random::initialize(rand_seed,rand_distance,rand_speed,rand_health);
} 

int fill_round(){
    string line;
    uint32_t round_temp = 1;

    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;

    uint32_t random_zombies = 0;
    uint32_t named_zombies = 0;
    

    cin >> line;
    if(empty(line)){
        keep_going = false;
        return 0;
    } 
    cin >> line; //round
    cin >> line;

    if(stoi(line) > nround+1){
        while(keep_going && stoi(line) > nround+1){
            nround++;
            round(nround);
            move_forward();
        }
        if(!keep_going){
            end_sequence();
        }
    }

    round_temp = stoi(line); 
    nround = round_temp;

    cin >> line >> line; //randomzombies
    random_zombies = stoi(line);
    cin >> line >> line; //namedzombies
    named_zombies = stoi(line);
    //cout << round << "\n" << random_zombies << "\n" << named_zombies << "\n";
    for(uint32_t i=0; i<random_zombies; i++){
        string name  = P2random::getNextZombieName();
        uint32_t distance = P2random::getNextZombieDistance();
        uint32_t speed    = P2random::getNextZombieSpeed();
        uint32_t health   = P2random::getNextZombieHealth();
        master.push_back(new Zombie(name, distance, speed, health, round_temp));
        pq.push(master.back());
    }
    for(uint32_t i=0; i<named_zombies; i++){
        cin >> name;
        cin >> line >> line;
        distance = stoi(line);
        cin >> line >> line;
        speed = stoi(line);
        cin >> line >> line;
        health = stoi(line);
        //cout << "name: " << name << " distance: " << distance << " speed: " << speed << " health: " << health << "\n";
        master.push_back(new Zombie(name, distance, speed, health, round_temp));
        pq.push(master.back());
    }
    return round_temp;
}

void end_sequence(){
    cout << "end sequence \n";
}

int main(int argc, char **argv){
    ios_base::sync_with_stdio(false);

    cl(argc, argv);
    fill_initial();
    while(keep_going){
        move_forward();
        if(!keep_going){
            end_sequence();
            break;
        }
        fill_round();
        round(nround);
    }
    
};
