//
//  LineField.cpp
//  flowlines_test
//
//  Created by Andrew Wallace on 1/30/21.
//

#include "LineField.hpp"


void LineField::setup(){
    
    //set the values used when spacing out lines
    d_sep = 3.0;            //how far a line origin must be from any other points
    d_test = d_sep * 0.4;   //how far a line point can get to other points while drawing
    
    max_steps_without_moving = 30;  //the the line is caught in one place, kill it. This is duct-tape
    
    
    int field_size = ofGetWidth();
    
    //setup distance grid
    distance_grid.setup(field_size, field_size, d_sep);
    
    //setup the vector field
    field.setupField(field_size/5, field_size/5, field_size, field_size);
    field.randomizeField(0.5);
    
    bool make_topo_shape = true;    //just toggling demo fields
    for (int i=0; i<30; i++){
        float effect_size = field_size * 0.3;
        float effect_strength = 2;
        if (make_topo_shape){
            field.addClockwiseCircle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), effect_size, effect_strength);
            field.addCounterClockwiseCircle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), effect_size, effect_strength);
        }else{
            field.addInwardCircle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), effect_size, effect_strength);
            field.addOutwardCircle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()),  effect_size, effect_strength);
        }
    }
    
    //create the first stream line at random
    StreamLine start_line = create_stream_line(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
    stream_lines.push_back(start_line);
    
    //then go through and make more until we have out fill
    cur_line_id = 0;
    finished = false;
    while (!finished){
        step();
    }
    
    //how long did we take?
    cout<<"FINISHED"<<endl;
    cout<<"lines: "<<stream_lines.size()<<endl;
    cout<<"seconds: "<<ofGetElapsedTimef()<<endl;
    
}

//advances the process for filling out the field
void LineField::step(){
    if (finished){
        return;
    }
    
    //get candidates from the current line
    vector<ofVec2f> candidate_pnts = stream_lines[cur_line_id].get_candidate_pnts(d_sep);
    
    //go through making lines or DQing candidate pnts
    while (candidate_pnts.size() > 0){
        //grab one at random
        int rand_id = ofRandom(candidate_pnts.size());
        ofVec2f candidate = candidate_pnts[rand_id];
        candidate_pnts.erase(candidate_pnts.begin()+rand_id);
        
        //check if it is legit
        if (is_candidate_pnt_valid(candidate)){
            //if it is, create a stream line
            StreamLine line = create_stream_line(candidate);
            
            //line.setup( candidate, d_test, &field, &stream_lines);
            stream_lines.push_back(line);
            
        }
    }
    
    //once we've exhausted that line, try to move on
    cur_line_id++;
    
    //if we're out of lines then we're done!
    if (cur_line_id >= stream_lines.size()){
        finished = true;
    }
    
}

//checks if a point is a valid place to start a line
bool LineField::is_candidate_pnt_valid(ofVec2f pnt){
    //make sure it is on screen
    if (pnt.x < 0 || pnt.x >= ofGetWidth() || pnt.y < 0 || pnt.y >= ofGetHeight()){
        return false;
    }
    
    //check it against the distance grid
    return distance_grid.is_pnt_far_enough_from_others(pnt, d_sep);
}

//creates a new line starting at the given origin
StreamLine LineField::create_stream_line(ofVec2f origin){
    StreamLine line;
    line.pnts.clear();
    
    line.pnts.push_back(origin);
    
    grow_line(origin, 1, &line, 0);
    grow_line(origin, -1, &line, 0);
    
    //add the points to the distance grid
    for (int i=0; i<line.pnts.size(); i++){
        distance_grid.add_ptn(line.pnts[i]);
    }
    
    return line;
}

//expands a line. dir must be 1 or -1 and tells us if we're going forward or backward
void LineField::grow_line(ofVec2f pos, int dir, StreamLine * line, int depth){
    //cout<<"my depth: "<<depth<<endl;
    
    ofVec2f force = field.getForceFromPos(pos.x, pos.y);
    
    pos += force * dir;
    
    //are we out of bounds?
    if (pos.x < 0 || pos.x > field.externalWidth || pos.y < 0 || pos.y > field.externalHeight){
        return;
    }
    
    //has this been going on too long? This should not hapen. Might need to bump this number for larger canvasses
    if (depth > 5000){
        cout<<"depth too high"<<endl;
        return;
    }
    
    //too close ot other lines?
    if (!distance_grid.is_pnt_far_enough_from_others(pos, d_test)){
        return;
    }
    
    //too cose to ourselves?
    bool can_start_checking = false;    //need to go far enough back that at leats one point was out of range
    
    if (dir == 1){
        for (int i=0; i<line->pnts.size(); i++){
            float dist_sq = ofDistSquared(pos.x, pos.y, line->pnts[i].x, line->pnts[i].y);
            if (!can_start_checking){
                if (dist_sq > powf(d_test,2)){
                    can_start_checking = true;
                }
                //make sure we are still moving and haven't stalled out
                if (i>max_steps_without_moving){
                    //cout<<"too still too long A"<<endl;
                    return;
                }
            }
            else{
                if (dist_sq < powf(d_test,2)){
                    //cout<<"too close to ourselves"<<endl;
                    return;
                }
            }
        }
    }
    if (dir == -1){
        for (int i=line->pnts.size()-1; i>=0; i--){
            float dist_sq = ofDistSquared(pos.x, pos.y, line->pnts[i].x, line->pnts[i].y);
            if (!can_start_checking){
                if (dist_sq > powf(d_test,2)){
                    can_start_checking = true;
                }
                //make sure we are still moving and haven't stalled out
                if (i< ((int)line->pnts.size())-max_steps_without_moving){
                    //cout<<"too still too long B"<<endl;
                    return;
                }
            }
            else{
                if (dist_sq < powf(d_test,2)){
                    //cout<<"too close to ourselves"<<endl;
                    return;
                }
            }
        }
    }
    
    //if we're still here, add the point and keep going
    if (dir == 1){
        line->pnts.insert(line->pnts.begin(), pos);
    }else{
        line->pnts.push_back(pos);
    }
    grow_line(pos, dir, line, depth+1);
}


//draws out what we have
void LineField::draw(){
    ofSetColor(230,230,255);
    ofSetLineWidth(1);
    field.draw();
    
    //ofSetLineWidth(2);
    
    for (int i=0; i<stream_lines.size(); i++){
        ofSetColor(0);
        if (i == cur_line_id)   ofSetColor(0,255,0);
        if (i == cur_line_id-1) ofSetColor(255,0,0);
        stream_lines[i].draw();
    }
    
    distance_grid.demo_draw();
}
