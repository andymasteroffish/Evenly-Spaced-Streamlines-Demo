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
    
//    float d_test;
//    float force_bump;
//    int max_steps_without_moving;
//    
//    
//    void setup(ofVec2f origin, float _d_test, VectorField * field, vector<StreamLine> * existing_lines){
//        
//        d_test = _d_test;
//        force_bump = 10;
//        max_steps_without_moving = 50;
//        
//        pnts.clear();
//        pnts.push_back(origin);
//        
//        //explore in both directions
//        trace(origin, 1, field, existing_lines, 0);
//        trace(origin, -1, field, existing_lines, 0);
//        
//        cout<<"new line size: "<<pnts.size()<<endl;
//    }
//    
//    void trace(ofVec2f pos, int dir, VectorField * field, vector<StreamLine> * existing_lines, int depth){
//        //cout<<"my depth: "<<depth<<endl;
//        
//        ofVec2f force = field->getForceFromPos(pos.x, pos.y) * force_bump;
//        //force.normalize();
//        //ofVec2f normalized = force.normalized();
//        
//        //cout<<"force: "<<normalized<<endl;
//        
//        pos += force * dir;
//        
//        //cout<<"pos "<<pos<<endl;
//        
//        //are we out of bounds?
//        if (pos.x < 0 || pos.x > field->externalWidth || pos.y < 0 || pos.y > field->externalHeight){
//            return;
//        }
//        
//        if (depth > 1000){
//            cout<<"depth too high"<<endl;
//            return;
//        }
//        
//        
//        //too close to other lines?
//        for (int i=0; i<existing_lines->size(); i++){
//            for (int k=0; k<existing_lines->at(i).pnts.size(); k++){
//                if (ofDistSquared(pos.x,pos.y, existing_lines->at(i).pnts[k].x,existing_lines->at(i).pnts[k].y) < powf(d_test,2)){
//                    return;
//                }
//            }
//        }
//        
//        //too cose to ourselves?
//        bool can_start_checking = false;    //need to go far enough back that at leats one point was out of range
//        
//        if (dir == 1){
//            for (int i=0; i<pnts.size(); i++){
//                float dist_sq = ofDistSquared(pos.x, pos.y, pnts[i].x, pnts[i].y);
//                if (!can_start_checking){
//                    if (dist_sq > powf(d_test,2)){
//                        can_start_checking = true;
//                    }
//                    //make sure we are still moving and haven't stalled out
//                    if (i>max_steps_without_moving){
//                        cout<<"too still too long A"<<endl;
//                        return;
//                    }
//                }
//                else{
//                    if (dist_sq < powf(d_test,2)){
//                        cout<<"too close to ourselves"<<endl;
//                        return;
//                    }
//                }
//            }
//        }
//        if (dir == -1){
//            for (int i=pnts.size()-1; i>=0; i--){
//                float dist_sq = ofDistSquared(pos.x, pos.y, pnts[i].x, pnts[i].y);
//                if (!can_start_checking){
//                    if (dist_sq > powf(d_test,2)){
//                        can_start_checking = true;
//                    }
//                    //make sure we are still moving and haven't stalled out
//                    if (i< ((int)pnts.size())-max_steps_without_moving){
//                        cout<<"too still too long B"<<endl;
//                        return;
//                    }
//                }
//                else{
//                    if (dist_sq < powf(d_test,2)){
//                        cout<<"too close to ourselves"<<endl;
//                        return;
//                    }
//                }
//            }
//        }
//        
//        //if we're still here, add the point and keep going
//        if (dir == 1){
//            pnts.insert(pnts.begin(), pos);
//        }else{
//            pnts.push_back(pos);
//        }
//        trace(pos, dir, field, existing_lines, depth+1);
//    }
    
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
    
    void draw(){
        //cout<<"pnts: "<<pnts.size()<<endl;
        for (int i=0; i<pnts.size()-1; i++){
            ofDrawLine(pnts[i], pnts[i+1]);
        }
    }
    
    
};


#endif /* StreamLine_h */
