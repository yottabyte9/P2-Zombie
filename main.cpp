// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

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
string last_name;
bool file_end = false;
bool victory = false;

uint32_t num;
bool s, m, v = false;

struct Zombie{
    public:
        string name;
        uint32_t distance;
        uint32_t speed;
        uint32_t health;
        uint32_t round;
        uint32_t active = 1;
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

struct Comparator_s1{
    bool operator()(const Zombie* z1, const Zombie* z2) const{
        int z1_time = z1->active;
        int z2_time = z2->active;
        if(z1_time < z2_time){
            return true;
        }
        else if(z1_time > z2_time){
            return false;
        }
        return (*z1).name > (*z2).name;
    }
};

struct Comparator_s2{
    bool operator()(const Zombie* z2, const Zombie* z1) const{
        int z1_time = z1->active;
        int z2_time = z2->active;
        if(z1_time < z2_time){
            return true;
        }
        else if(z1_time > z2_time){
            return false;
        }
        return (*z1).name < (*z2).name;
    }
};

priority_queue<Zombie*, vector<Zombie*>, Comparator> pq; 
deque<Zombie*> master;
deque<Zombie*> dead;
priority_queue<Zombie*, vector<Zombie*>, Comparator_s1> most_active; 
priority_queue<Zombie*, vector<Zombie*>, Comparator_s2> least_active; 

priority_queue<size_t, vector<size_t>, greater<size_t>> m1;
priority_queue<size_t, vector<size_t>, less<size_t>> m2;


int eta(const Zombie* zombie){
    return (*zombie).distance/(*zombie).speed;
}

int shoot(Zombie *zombie, uint32_t current_quiver){
    //cout << "shooting: " << zombie->name << "\n";
    if(zombie->health > current_quiver){
        zombie->health -= current_quiver;
        current_quiver = 0;
    }
    else{
        current_quiver -= zombie->health;
        zombie->health = 0;
        //remove zombie
        if(v){
            cout << "Destroyed: " << zombie->name << " (distance: " << zombie->distance << ", speed: " << zombie->speed << ", health: "<< zombie->health << ")\n";
        }
        if(m){
            uint32_t lifetime = uint32_t(nround) - zombie->round +1;
            if(m1.empty() || lifetime > m1.top()){
                m1.push(lifetime);
            }
            else if(m2.empty() || lifetime < m2.top()){
                m2.push(lifetime);
            }
            else{
                if(m1.size() > m2.size()){
                    m2.push(lifetime);
                }
                else{
                    m1.push(lifetime);
                }
            }

            if(m1.size()-m2.size()>1 && !m1.empty()){
                m2.push(m1.top());
                m1.pop();
            }
            else if(m2.size()- m1.size() > 1 && !m2.empty()){
                m1.push(m2.top());
                m2.pop();
            }
        }
        dead.push_back(zombie);
        pq.pop();
    }
    return current_quiver;
}

double median_output(){
    if(m2.size() > m1.size()){
        return int(m2.top());
    }
    else if(m1.size() > m2.size()){
        return int(m1.top());
    }
    return int(m1.top()+m2.top() + 1)/2;
}

void round(){
    int current_quiver = quiver;
    while(current_quiver > 0 && !pq.empty()){
        if(pq.size() < 2){
            last_name = pq.top()->name;
        }
        current_quiver = shoot(pq.top(), current_quiver);
    }
    if(pq.empty()){
        victory = true;
    }
    if(m){
        double temp = 0;
        temp = median_output();
        cout << "At the end of round " << nround << ", the median zombie lifetime is "<< temp << "\n";
    }
}

void move_forward(){ 
    bool death_flag = false;
    string killer;
    if(v){
        cout << "Round: " << nround+1 << "\n";
    }
    for(auto zombie: master){
        if(zombie->health == 0){
            continue;
        }
        zombie->active++;
        zombie->distance = max(0, int(zombie->distance-zombie->speed));
        if(v){
            cout << "Moved: " << zombie->name << " (distance: " << zombie->distance << ", speed: " << zombie->speed << ", health: "<< zombie->health << ")\n";
        }
        if(death_flag){
            continue;
        }
        if(zombie->distance == 0){
            keep_going = false;
            killer = zombie->name;
            death_flag = true;
        }
    }
    if(death_flag){
        cout << "DEFEAT IN ROUND " << nround+1 << "! " << killer << " ate your brains!\n";
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
        nround ++;
        file_end = true;
        return 0;
    }
    cin >> line; //round
    cin >> line;

    if(stoi(line) > nround+1){
        while(keep_going && stoi(line) > nround+1){
            nround++;
            round();
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
        if(v){
            cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: "<< health << ")\n";
        }
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
        if(v){
            cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: "<< health << ")\n";
        }
        master.push_back(new Zombie(name, distance, speed, health, round_temp));
        pq.push(master.back());
    }
    return round_temp;
}

void end_sequence(){
    if(s){
        cout << "Zombies still active: " << master.size() - dead.size() << "\n";
        cout << "First zombies killed:\n";
        for(uint32_t i=0; i<num; i++){
            cout << dead[i]->name << " " << i+1 << "\n"; 
        }
        cout << "Last zombies killed:\n";
        for(uint32_t i=0; i<num; i++){
            cout << dead[dead.size()-i-1]->name << " " << num-i << "\n"; 
        }
        cout << "Most active zombies:\n";
        for(uint32_t i=0; i<master.size(); i++){
            most_active.push(master[i]);
        }
        for(uint32_t i=0; i<num; i++){
            cout << most_active.top()->name;
            if(most_active.top()->health > 0){
                cout << " " <<  most_active.top()->active << "\n";
            }
            else{
                cout << " " << most_active.top()->active << "\n";
            }
            most_active.pop();
        }
        cout << "Least active zombies:\n";
        for(uint32_t i=0; i<master.size(); i++){
            least_active.push(master[i]);
        }
        for(uint32_t i=0; i<num; i++){
            cout << least_active.top()->name;
            if(least_active.top()->health > 0){
                cout << " " <<  least_active.top()->active << "\n";
            }
            else{
                cout << " " << least_active.top()->active << "\n";
            }
            least_active.pop();
        }
    }
}

int main(int argc, char **argv){
    ios_base::sync_with_stdio(false);

    cl(argc, argv);
    fill_initial();
    while(keep_going && !victory){
        move_forward();
        if(!keep_going){
            end_sequence();
            victory = false;
            return 0;
        }
        if(file_end){
            nround ++;
            round();
        }
        else{
            fill_round();
            round();
        }
    }
    if(victory){
        cout << "VICTORY IN ROUND " << nround << "! " << last_name << " was the last zombie.\n";
        end_sequence();
    }
    for (size_t i = 0; i < master.size(); i++) {
        delete master[i];
    }
};
