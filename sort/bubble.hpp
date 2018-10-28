#include <string>
#include <iostream>
#include <vector>

namespace sort {
 
    void BubbleSort( std::vector<int> &inArray ){
        int size = inArray.size();
        while ( size >= 1 )
        {
            for ( int i = 0; i < size; ++i )
            {
                if (inArray[i] > inArray[i+1])
                {
                    int tmp = inArray[i];
                    inArray[i] = inArray[i+1];
                    inArray[i+1] = tmp;
                }
            }
            --size;
        }
    }
}
