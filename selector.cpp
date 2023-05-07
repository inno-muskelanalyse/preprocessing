#include <vector>
#include "segmentation.cpp"
#include <exception>
#include <tuple>
#include <array>


using namespace std;

class Selector{
    public:
        Selector(int *** greenmap, int iterations, int height, int width);
        ~Selector();
        int ** map;
        

    private:
        
        int mapLength;
        int mapHeight;
        void selection(int compare, int select);
        
        
        
};


Selector::Selector(int *** greenmap, int iterations, int height, int width)
{
    
    map = greenmap[0];
    mapHeight = height;
    mapLength = width;


    int iter = (iterations <= 0) ? true : false;
    int counter = 0;
    int compare = 0;
    int select = 1;

    for(int i = 0; i < iterations; i++)
    {
        selection(compare, select);
        compare++;
        select++;
    }
    
}

void Selector::selection(int compare, int select)
{  
    for(int x = 0; x < mapLength; x++)
    {
        for(int y = 0; y < mapHeight; y++)
        {
            //only interested in "high" values
            int to_check = map[x][y];
            if(to_check != select)
            {
                continue;
            }


            if(x != 0)
            {
                //          [ ][ ][ ]
                //          [x][ ][ ]
                //          [ ][ ][ ]
                if(map[x-1][y] == compare)
                {
                    map[x][y] = select + 1;
                    continue;
                }
                if(y != 0)
                {
                    //          [x][ ][ ]
                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    if(map[x-1][y-1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }

                    //          [ ][x][ ]
                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    if(map[x][y-1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }
                }
                if(y != mapHeight-1)
                {
                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    //          [x][ ][ ]
                    if(map[x-1][y+1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }

                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    //          [ ][x][ ]
                    if(map[x][y+1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }
                }
            }
            if(x != mapLength)
            {
                if(y != 0)
                {
                    //          [ ][ ][x]
                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    if(map[x+1][y-1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }

                    //          [ ][ ][ ]
                    //          [ ][ ][x]
                    //          [ ][ ][ ]
                    if(map[x+1][y] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }
                }
                if(y != mapHeight)
                {
                    //          [ ][ ][ ]
                    //          [ ][ ][ ]
                    //          [ ][ ][x]
                    if(map[x+1][y+1] == compare)
                    {
                        map[x][y] = select + 1;
                        continue;
                    }
                }
            }
        }
    }
}





