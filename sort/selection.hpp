#include <string>
#include <iostream>
#include <vector>

namespace sort {
 
    // Reference: https://www.youtube.com/watch?v=GUDLRan2DWM
    void SelectionSort( std::vector<int> &inArray ){
        int size = inArray.size();
        for ( int i = 0; i < size; ++i )
        {
            int j = i + 1;
            int least = inArray[i];
            int last = i;
            for ( ; j < size; ++j )
            {
                if ( least > inArray[j] ) {
                    least = inArray[j];
                    last = j;
                }
            }
            if (last == size)
                --last;
            
            int tmp = inArray[i];
            inArray[i] = inArray[last];
            inArray[last] = tmp;
        }
    }
}
