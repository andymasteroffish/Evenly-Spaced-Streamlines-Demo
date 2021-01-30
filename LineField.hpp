//
//  LineField.hpp
//  flowlines_test
//
//  Created by Andrew Wallace on 1/30/21.
//

//based on Creating Evenly-Spaced Streamlines of Arbitrary Density by Bruno Jobard
//https://www.researchgate.net/publication/2825680_Creating_Evenly-Spaced_Streamlines_of_Arbitrary_Density

#ifndef LineField_hpp
#define LineField_hpp

#include "ofMain.h"

#include "VectorField.hpp"
#include "StreamLine.h"
#include "DistanceGrid.h"

class LineField{
public:
    
    void setup();
    void step();
    bool is_candidate_pnt_valid(ofVec2f pnt);
    StreamLine create_stream_line(ofVec2f origin);
    void grow_line(ofVec2f pos, int dir, StreamLine * line, int depth);
    
    void draw();
    
    DistanceGrid distance_grid;
    
    VectorField field;
    vector<StreamLine> stream_lines;
    
    float d_sep, d_test;
    
    int cur_line_id ;
    bool finished;
    
    int max_steps_without_moving;
    
};

#endif /* LineField_hpp */
