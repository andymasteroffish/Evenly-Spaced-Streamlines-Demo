//
//  DistanceGrid.h
//  flowlines_test
//
//  Created by Andrew Wallace on 1/30/21.
//

#ifndef DistanceGrid_h
#define DistanceGrid_h

#include "ofMain.h"

class DistanceBucket{
public:
    
    //This should be a pointer, but the vectors I'm using elsewhere make that annoying
    vector<ofVec2f> pnts;
    
    void clear(){
        pnts.clear();
    }
    
};

class DistanceGrid{
public:
    
    vector<vector<DistanceBucket>> grid;
    
    float d_sep;
    int cols;
    int rows;
    
    void setup(float field_w, float field_h, float _d_sep){
        d_sep = _d_sep;
        
        cols = field_w / d_sep + 1;
        rows = field_h / d_sep + 1;
        
        //populate the grid
        grid.clear();
        for (int c=0; c<cols; c++){
            vector<DistanceBucket> this_col;
            for (int r=0; r<rows; r++){
                DistanceBucket bucket;
                bucket.clear();
                this_col.push_back(bucket);
            }
            grid.push_back(this_col);
        }
    }
    
    void add_ptn(ofVec2f p){
        //get the cell for this location
        int c = p.x / d_sep;
        int r = p.y / d_sep;
        
        //make sure we're in range
        if (c<0 || c>=cols || r<0 || r>=rows){
            cout<<"POINT OUT OF RANGE: "<<c<<","<<r<<endl;
            return;
        }
        
        //add it
        grid[c][r].pnts.push_back(p);
    }
    
    //returns true if pnt is far enoug away
    bool is_pnt_far_enough_from_others(ofVec2f p, float check_dist){
        if (check_dist > d_sep){
            cout<<"CHECK DIST CANNOT BE GREATER THAN D_SEP!"<<endl;
        }
        
        //get the cell for this location
        int col = p.x / d_sep;
        int row = p.y / d_sep;
        
        //check that cell and the ones around it
        int start_col = MAX(0, col-1);
        int end_col = MIN(cols-1, col+1);
        int start_row = MAX(0, row-1);
        int end_row = MIN(rows-1, row+1);
        
        for (int c=start_col; c<=end_col; c++){
            for (int r=start_row; r<=end_row; r++){
                //get the pnts here
                vector<ofVec2f> others = grid[c][r].pnts;
                for (int i=0; i<others.size(); i++){
                    if (ofDistSquared(p.x,p.y, others[i].x, others[i].y) < powf(check_dist,2)){
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
    
    
    //testing
    void demo_draw(){
        for (int c=1; c<=3; c++){
            for (int r=1; r<=3; r++){
                ofSetColor(255,0,0);
                ofNoFill();
                ofDrawRectangle(c*d_sep, r*d_sep, d_sep, d_sep);
                
                for (int i=0; i<grid[c][r].pnts.size(); i++){
                    ofDrawCircle(grid[c][r].pnts[i].x,grid[c][r].pnts[i].y,2);
                }
            }
        }
    }
    
    
    
    
};


#endif /* DistanceGrid_h */
