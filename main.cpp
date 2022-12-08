#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>
#include <cmath>
#include<fstream>
#include<sstream>
#include<iomanip>

using namespace std;
//3D MODEL

class Bacteria {
public:
    int x;
    int y;
    int z;
    double radius;
    int nn_av;
    int type;
    int swims;
};

class av_point{
public:
    int x;
    int y;
    int z;
};

class Array3D {
    size_t m_width, m_height;
    std::vector<int> m_data;
public:
    Array3D(size_t x, size_t y, size_t z, int init = 0):
            m_width(x), m_height(y), m_data(x*y*z, init)
    {}
    int& operator()(size_t x, size_t y, size_t z) {
        return m_data.at(x + y * m_width + z * m_width * m_height);
    }
};


int check_if_neighbours(Bacteria &b1, Bacteria &b2) {
    for (int i=-1; i < 2; i += 2) {
        if ((b1.x+i == b2.x) && (b1.y == b2.y) && (b1.z == b2.z)) return 1;
        if ((b1.x == b2.x) && (b1.y+i == b2.y) && (b1.z == b2.z)) return 1;
        if ((b1.x == b2.x) && (b1.y == b2.y) && (b1.z+i == b2.z)) return 1;
    }
    return 0;
}

double find_radius(int x1,int y1,int z1,int x2,int y2,int z2) {
    double r = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
    return r;
}


int main() {
    int length = 500;
    double k = 1; //Rate of growing
    double ks= 0.01;  //Rate of swimming
    double timepas=0;
    Array3D lattice(length, length, length, 0);


    ofstream MyFile("ResultsQ13.txt");
    ofstream File("EvolutionQ13.txt");

    Bacteria first{int (length/2), int (length/2), int (length/2),0,6};
    vector<Bacteria> colony;
    colony.push_back(first);
    lattice((length/2), (length/2), (length/2)) = 1;


    // Initialize the loop to determine how many events we want to have in our simulation.
    int events = 1000000;
    for (int time{ 0 }; time < events; ++time) {
        int N1 = 0;
        int N2 = 0;
        double jump = 0;

        vector<av_point> type1;
        vector<av_point> type2;
        vector<av_point> swim;

        // First we run the code to determine the number of cells with 1 or 2 empty neighbours or with 3-4 empty neighbours
        // Loop over nn of colony

        for (Bacteria &b : colony) {
            if (b.nn_av>0){
                b.nn_av=0;
                vector<av_point> test;
                if (lattice(b.x+1, b.y, b.z) == 0){
                    test.push_back({b.x + 1, b.y, b.z});
                    ++b.nn_av ;
                }
                if (lattice(b.x-1, b.y, b.z) == 0){
                    test.push_back({b.x -1, b.y, b.z});
                    ++b.nn_av ;
                }
                if (lattice(b.x, b.y+1, b.z) == 0){
                    test.push_back({b.x , b.y+1, b.z});
                    ++b.nn_av ;
                }
                if (lattice(b.x, b.y-1, b.z) == 0){
                    test.push_back({b.x, b.y-1, b.z});
                    ++b.nn_av ;
                }
                if (lattice(b.x, b.y, b.z+1) == 0){
                    test.push_back({b.x, b.y, b.z+1});
                    ++b.nn_av ;
                }
                if (lattice(b.x, b.y, b.z-1) == 0){
                    test.push_back({b.x, b.y, b.z-1});
                    ++b.nn_av ;
                }

                if (test.empty()) continue;
                if (3 < b.nn_av) {
                    ++N2;
                    type2.insert(std::end(type2), std::begin(test), std::end(test));
                    av_point swimmer{b.x, b.y, b.z};
                    swim.push_back(swimmer);
                } else if (0 < b.nn_av) {
                    ++N1;
                    type1.insert(std::end(type1), std::begin(test), std::end(test));
                }
            }
            else continue;
        }
        // Find the total rate:
        double T = (N1 * k) + (N2 * (k + ks));
        // Throw a random number to determine which event happens.
        std::random_device rd{};
        std::mt19937 engine{rd()};
        std::uniform_real_distribution<double> distr2(0, 1);
        double tau = -log(distr2(engine)) / T;
        timepas = timepas + tau;
        double prob = distr2(engine);
        if (prob < ((N1 * k) / T)) {
            std::uniform_int_distribution<int> dis(0, type1.size() - 1);
            int p = dis(engine);
            av_point grows = type1[p];
            double rad = find_radius(grows.x,grows.y,grows.z,int (length/2), int (length/2), int (length/2));
            Bacteria newb{grows.x, grows.y, grows.z,rad,1};
            colony.push_back(newb);
            lattice(grows.x, grows.y, grows.z) = 1;
            jump =0;
        }
        else if (prob < ((N1 + N2) * k / T)) {
            std::uniform_int_distribution<int> dis(0, type2.size() - 1);
            int p = dis(engine);
            av_point grows = type2[p];
            double rad = find_radius(grows.x,grows.y,grows.z,int (length/2), int (length/2), int (length/2));
            Bacteria newb{grows.x, grows.y, grows.z, rad,1};
            colony.push_back(newb);
            lattice(grows.x, grows.y, grows.z) = 1;
            jump = 0;
        }
        else {

            std::uniform_int_distribution<int> disn(0, swim.size() - 1);
            //std::uniform_real_distribution<double> rad(0, 1);
            std::normal_distribution<double> gaussian(0.0, 2.0);
            //std::uniform_real_distribution<double> directionT(0, M_PI);
            //std::uniform_real_distribution<double> directionP(0, 2*M_PI);

            int p = disn(engine);
            av_point aswim = swim[p];
            int sx = aswim.x;
            int sy = aswim.y;
            int sz = aswim.z;
            int no_swim=0;

            //double distance = gaussian(engine);
            //double phi = directionP(engine);
            //double theta = directionT(engine);

            //int randx = distance*sin(theta)*cos(phi);
            //int randy = distance*sin(theta)*sin(phi);
            //int randz = distance*cos(theta);
            int randx =gaussian(engine);
            int randy =gaussian(engine);
            int randz =gaussian(engine);
            std::cout << "hello" <<'\t'<<'\t'<<randx<<'\t'<<randy<<'\t'<<randz<< '\n';
            if(lattice(sx + randx, sy + randy, sz + randz) == 1){
                no_swim=1;
            };

            if (no_swim==0){
                for (Bacteria &b4 : colony) {
                    if (abs(b4.x - sx) == 0 && abs(b4.y - sy) == 0 && abs(b4.z - sz) == 0) {
                        b4.x = sx + randx;
                        b4.y = sy + randy;
                        b4.z = sz + randz;
                        b4.radius = find_radius(sx+randx,sy+randy,sz+randz,int (length/2), int (length/2), int (length/2));
                        jump = sqrt((randx*randx)+(randy*randy)+(randz*randz));
                        lattice(sx, sy, sz) = 0;
                        lattice(sx + randx, sy + randy, sz + randz) = 1;
                        //break;
                    }
                    else if (abs(b4.x - sx+1) == 0 && abs(b4.y - sy) == 0 && abs(b4.z - sz) == 0) {
                        b4.nn_av=1;}
                    else if (abs(b4.x - sx-1) == 0 && abs(b4.y - sy) == 0 && abs(b4.z - sz) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - sx) == 0 && abs(b4.y - sy+1) == 0 && abs(b4.z - sz) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - sx) == 0 && abs(b4.y - sy-1) == 0 && abs(b4.z - sz) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - sx) == 0 && abs(b4.y - sy) == 0 && abs(b4.z - sz+1) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - sx) == 0 && abs(b4.y - sy) == 0 && abs(b4.z - sz-1) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)+1) == 0 && abs(b4.y - (sy+randy)) == 0 && abs(b4.z - (sz+randz)) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)-1) == 0 && abs(b4.y - (sy+randy)) == 0 && abs(b4.z - (sz+randz)) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)) == 0 && abs(b4.y - (sy+randy)+1) == 0 && abs(b4.z - (sz+randz)) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)) == 0 && abs(b4.y - (sy+randy)-1) == 0 && abs(b4.z - (sz+randz)) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)) == 0 && abs(b4.y - (sy+randy)) == 0 && abs(b4.z - (sz+randz)+1) == 0) {
                        b4.nn_av=1;
                    }
                    else if (abs(b4.x - (sx+randx)) == 0 && abs(b4.y - (sy+randy)) == 0 && abs(b4.z - (sz+randz)-1) == 0) {
                        b4.nn_av=1;
                    }

                }
            }
        }
        double maxdis=0;
        for (Bacteria &radb: colony){
            if (radb.radius > maxdis){
                maxdis = radb.radius;
            }

        }
        File<<colony.size() <<'\t' <<timepas<<'\t'<< jump<<'\t'<<maxdis<<'\t'<<type1.size()<<'\t'<<type2.size()<<'\t'<<swim.size()<<'\n';
        cout << time << endl;
    }
    for (Bacteria &bac: colony){
        MyFile<<bac.x<<'\t'<<bac.y<<'\t'<<bac.z<<'\n';
    }


    string directory = "LatQ13//";
    string fileName = "Stack";
    string fileType = ".txt";
    const int maxDigits = 4;
    stringstream ss;

//Number of files
    const int count = length;

    ofstream writer;
    int z=0;
    while (z<length){
        ss<<directory<<fileName<<setw(maxDigits)<<setfill<char>('0')<<z<<fileType;
        writer.open(ss.str());
        int row1=0;
        while (row1<length)
        {
            int col1 = 0;
            while(col1<length)
            {
                writer<<lattice(row1,col1,z) <<'\t';
                ++col1;
            }
            writer<<endl;
            ++row1;
        }
        //Clear the string stream
        writer.close();
        ss.str("");
        ++z;
    }

    MyFile.close();
    File.close();
    return 0;
}
