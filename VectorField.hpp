//
//  VectorField.hpp
//  image_processing
//
//  Created by Andrew Wallace on 12/13/20.
//

#ifndef VectorField_hpp
#define VectorField_hpp

#include "ofMain.h"
#include "GLine.hpp"

class VectorField {
    
public:
    
    // the internal dimensions of the field:    (ie, 60x40, etc)
    int fieldWidth;
    int fieldHeight;
    int fieldSize;   // total number of "pixels", ie w * h
    
    // the external dimensions of the field:   (ie, 1024x768)
    int externalWidth;
    int externalHeight;
    
    vector <vector<ofVec2f>> field;
    
    VectorField();
    virtual ~VectorField();
    
    void setupField(int innerW, int innerH, int outerW, int outerH);   // pass in internal dimensions and outer dimensions
    
    void clear();
    void fadeField(float fadeAmount);
    void randomizeField(float scale);
    void draw();
    vector<GLine> get_lines();
    
    ofVec2f    getForceFromPos(float xpos, float ypos);
    
    void addOutwardCircle(float x, float y, float radius, float strength);
    void addInwardCircle(float x, float y, float radius, float strength);
    void addClockwiseCircle(float x, float y, float radius, float strength);
    void addCounterClockwiseCircle(float x, float y, float radius, float strength);
    void addVectorCircle(float x, float y, float vx, float vy, float radius, float strength);
    
    void set_outter_oval(float center_x, float center_y, float width, float height, float strength);
    void point_unused_to_point(float center_x, float center_y, float strength);
    
protected:
private:
};

#endif /* VectorField_hpp */
