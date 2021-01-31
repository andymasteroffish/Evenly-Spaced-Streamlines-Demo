//
//  StreamLine.h
//  flowlines_test
//
//  Created by Andrew Wallace on 1/29/21.
//

#ifndef StreamLine_h
#define StreamLine_h

#include "ofMain.h"
#include "VectorField.hpp"

class StreamLine{
public:
    
    vector<ofVec2f> pnts;
    
    //runs along the line pulling all tangent points d_sep away from the line
    vector<ofVec2f> get_candidate_pnts(float d_sep){
        
        vector<ofVec2f> candidates;
       
        for (int i=0; i<pnts.size()-1; i++){
            float angle = atan2(pnts[i].y-pnts[i+1].y, pnts[i].x-pnts[i+1].x);
            
            
            ofVec2f p1 = ofVec2f(pnts[i]);
            p1.x += cos(angle+PI/2) * d_sep;
            p1.y += sin(angle+PI/2) * d_sep;
            
            ofVec2f p2 = ofVec2f(pnts[i]);
            p2.x += cos(angle-PI/2) * d_sep;
            p2.y += sin(angle-PI/2) * d_sep;
            
            candidates.push_back(p1);
            candidates.push_back(p2);
        }
        
        return candidates;
        
    }
    
    //demo draw
    void draw(){
        for (int i=0; i<pnts.size()-1; i++){
            ofDrawLine(pnts[i], pnts[i+1]);
        }
    }
    
    
};


#endif /* StreamLine_h */
