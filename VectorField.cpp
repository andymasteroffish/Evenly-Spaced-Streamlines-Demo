//
//  VectorField.cpp
//  image_processing
//
//  Created by Andrew Wallace on 12/13/20.
//

#include "VectorField.hpp"

//------------------------------------------------------------------------------------
VectorField::VectorField(){
}

//------------------------------------------------------------------------------------
void VectorField::setupField(int innerW, int innerH, int outerW, int outerH){
    
    fieldWidth            = innerW;
    fieldHeight           = innerH;
    externalWidth         = outerW;
    externalHeight        = outerH;
    
    field.clear();
    
    fieldSize = fieldWidth * fieldHeight;
    for (int i = 0; i < fieldSize; i++){
        ofVec2f pt;
        pt.set(0,0);
        
        
        
        field.push_back(pt);
    }
    
    
    //testing
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
            
            // pos in array
            int pos = j * fieldWidth + i;
            
            //test angle
            float angle = ofNoise(i*0.03, j*0.01);
            field[pos].x = cos(angle);
            field[pos].y = sin(angle);
            
        }
    }
}


//------------------------------------------------------------------------------------
void VectorField::clear(){
    for (int i = 0; i < fieldSize; i++){
        field[i].set(0,0);
    }
}


//------------------------------------------------------------------------------------
void VectorField::fadeField(float fadeAmount){
    for (int i = 0; i < fieldSize; i++){
        field[i].set(field[i].x*fadeAmount,field[i].y*fadeAmount);
    }
}

//------------------------------------------------------------------------------------
void VectorField::randomizeField(float scale){
    for (int i = 0; i < fieldSize; i++){
        // random between -1 and 1
        float x = (float)(ofRandom(-1,1)) * scale;
        float y = (float)(ofRandom(-1,1)) * scale;
        field[i].set(x,y);
    }
}

//------------------------------------------------------------------------------------
void VectorField::draw(){
    
    float scalex = (float)externalWidth / (float)fieldWidth;
    float scaley = (float)externalHeight / (float)fieldHeight;
    
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
            
            // pos in array
            int pos = j * fieldWidth + i;
            // pos externally
            float px =     i * scalex;
            float py =     j * scaley;
            float px2 = px + field[pos].x * 2;
            float py2 = py + field[pos].y * 2;
            
            ofLine(px,py, px2, py2);
            
            
            // draw an baseline to show direction
            // get the line as vector, calculate length, then normalize.
            // rotate and draw based on length
            ofVec2f line;
            line.set(px2-px, py2-py);
            float length = line.length();
            line.normalize();
            line.rotate(90);  // these are angles in degrees
            ofLine(px - line.x*length*0.2, py - line.y*length*0.2, px + line.x*length*0.2, py + line.y*length*0.2);
            //this little bit above is not super important. Just the little base line at the bottom of each vector
            
        }
    }
}

vector<GLine> VectorField::get_lines(){
    vector<GLine> lines;
    
    float scalex = (float)externalWidth / (float)fieldWidth;
    float scaley = (float)externalHeight / (float)fieldHeight;
    
    for (int i = 0; i < fieldWidth; i++){
        for (int j = 0; j < fieldHeight; j++){
            
            // pos in array
            int pos = j * fieldWidth + i;
            // pos externally
            float px =     i * scalex;
            float py =     j * scaley;
            float px2 = px + field[pos].x * 4;
            float py2 = py + field[pos].y * 4;
            
            GLine line = GLine(px,py, px2, py2);
            lines.push_back(line);
            
            
        }
    }
    
    return lines;
}


//------------------------------------------------------------------------------------
ofVec2f VectorField::getForceFromPos(float xpos, float ypos){
    
    ofVec2f frc;
    frc.set(0,0);
    
    // convert xpos and ypos into pcts =
    float xPct = xpos / (float)externalWidth;
    float yPct = ypos / (float)externalHeight;
    
    // if we are less then 0 or greater then 1 in x or y, return no force.
    if ((xPct < 0 || xPct > 1) || (yPct < 0 || yPct > 1)){
        return frc;
    }
    
    // where are we in the array
    int fieldPosX = (int)(xPct * fieldWidth);
    int fieldPosY = (int)(yPct * fieldHeight);
    
    // saftey :)
    fieldPosX = MAX(0, MIN(fieldPosX, fieldWidth-1));
    fieldPosY = MAX(0, MIN(fieldPosY, fieldHeight-1));
    
    // pos in array
    int pos = fieldPosY * fieldWidth + fieldPosX;
    
    frc.set(field[pos].x * 0.1, field[pos].y * 0.1 );  // scale here as values are pretty large.
    return frc;
}

//------------------------------------------------------------------------------------
void VectorField::addInwardCircle(float x, float y, float radius, float strength){
    
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    
    float pctx            = x / (float)externalWidth;
    float pcty            = y / (float)externalHeight;
    float radiusPct        = radius / (float)externalWidth;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float fieldRadius    = (float)(radiusPct * fieldWidth);
    
    // we used to do this search through every pixel, ie:
    //    for (int i = 0; i < fieldWidth; i++){
    //    for (int j = 0; j < fieldHeight; j++){
    // but we can be smarter :)
    // now, as we search, we can reduce the "pixels" we look at by
    // using the x y +/- radius.
    // use min and max to make sure we don't look over the edges
    
    int startx    = MAX(fieldPosX - fieldRadius, 0);
    int starty    = MAX(fieldPosY - fieldRadius, 0);
    int endx    = MIN(fieldPosX + fieldRadius, fieldWidth);
    int endy    = MIN(fieldPosY + fieldRadius, fieldHeight);
    
    
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
            
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
            if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
            
            if (distance < fieldRadius){
                
                float pct = 1.0f - (distance / fieldRadius);
                
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x += unit_px * strongness;
                field[pos].y += unit_py * strongness;
            }
        }
    }
}


//------------------------------------------------------------------------------------
void VectorField::addOutwardCircle(float x, float y, float radius, float strength){
    
    
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    
    float pctx            = x / (float)externalWidth;
    float pcty            = y / (float)externalHeight;
    float radiusPct        = radius / (float)externalWidth;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float fieldRadius    = (float)(radiusPct * fieldWidth);
    
    // we used to do this search through every pixel, ie:
    //    for (int i = 0; i < fieldWidth; i++){
    //    for (int j = 0; j < fieldHeight; j++){
    // but we can be smarter :)
    // now, as we search, we can reduce the "pixels" we look at by
    // using the x y +/- radius.
    // use min and max to make sure we don't look over the edges
    
    int startx    = MAX(fieldPosX - fieldRadius, 0);
    int starty    = MAX(fieldPosY - fieldRadius, 0);
    int endx    = MIN(fieldPosX + fieldRadius, fieldWidth);
    int endy    = MIN(fieldPosY + fieldRadius, fieldHeight);
    
    
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
            
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
            if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
            
            if (distance < fieldRadius){
                
                float pct = 1.0f - (distance / fieldRadius);
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                
                //printf(" overall length = %f \n", sqrt(unit_px* unit_px + unit_py* unit_py));
                
                field[pos].x -= unit_px * strongness;
                field[pos].y -= unit_py * strongness;
            }
        }
    }
}

//------------------------------------------------------------------------------------
void VectorField::addClockwiseCircle(float x, float y, float radius, float strength){
    
    
    
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    
    float pctx            = x / (float)externalWidth;
    float pcty            = y / (float)externalHeight;
    float radiusPct        = radius / (float)externalWidth;    //raidus ignores height, but in most cases this won't look weird
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float fieldRadius    = (float)(radiusPct * fieldWidth);
    
    // we used to do this search through every pixel, ie:
    //    for (int i = 0; i < fieldWidth; i++){
    //    for (int j = 0; j < fieldHeight; j++){
    // but we can be smarter :)
    // now, as we search, we can reduce the "pixels" we look at by
    // using the x y +/- radius.
    // use min and max to make sure we don't look over the edges
    
    int startx    = MAX(fieldPosX - fieldRadius, 0);
    int starty    = MAX(fieldPosY - fieldRadius, 0);
    int endx    = MIN(fieldPosX + fieldRadius, fieldWidth);
    int endy    = MIN(fieldPosY + fieldRadius, fieldHeight);
    
    
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
            
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
            if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
            
            if (distance < fieldRadius){
                
                float pct = 1.0f - (distance / fieldRadius);
                
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x += unit_py * strongness;   /// Note: px and py switched, for perpendicular
                field[pos].y -= unit_px * strongness;
            }
        }
    }
}



//------------------------------------------------------------------------------------
void VectorField::addCounterClockwiseCircle(float x, float y, float radius, float strength){
    
    
    
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    
    float pctx            = x / (float)externalWidth;
    float pcty            = y / (float)externalHeight;
    float radiusPct        = radius / (float)externalWidth;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float fieldRadius    = (float)(radiusPct * fieldWidth);
    
    // we used to do this search through every pixel, ie:
    //    for (int i = 0; i < fieldWidth; i++){
    //    for (int j = 0; j < fieldHeight; j++){
    // but we can be smarter :)
    // now, as we search, we can reduce the "pixels" we look at by
    // using the x y +/- radius.
    // use min and max to make sure we don't look over the edges
    
    int startx    = MAX(fieldPosX - fieldRadius, 0);
    int starty    = MAX(fieldPosY - fieldRadius, 0);
    int endx    = MIN(fieldPosX + fieldRadius, fieldWidth);
    int endy    = MIN(fieldPosY + fieldRadius, fieldHeight);
    
    
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
            
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
            if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
            
            if (distance < fieldRadius){
                
                float pct = 1.0f - (distance / fieldRadius);
                
                float strongness = strength * pct;
                float unit_px = ( fieldPosX - i) / distance;
                float unit_py = ( fieldPosY - j) / distance;
                field[pos].x -= unit_py * strongness;   /// Note: px and py switched, for perpendicular
                field[pos].y += unit_px * strongness;
            }
        }
    }
}


//------------------------------------------------------------------------------------
void VectorField::addVectorCircle(float x, float y, float vx, float vy, float radius, float strength){
    
    
    
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    
    float pctx            = x / (float)externalWidth;
    float pcty            = y / (float)externalHeight;
    float radiusPct        = radius / (float)externalWidth;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float fieldRadius    = (float)(radiusPct * fieldWidth);
    
    // we used to do this search through every pixel, ie:
    //    for (int i = 0; i < fieldWidth; i++){
    //    for (int j = 0; j < fieldHeight; j++){
    // but we can be smarter :)
    // now, as we search, we can reduce the "pixels" we look at by
    // using the x y +/- radius.
    // use min and max to make sure we don't look over the edges
    
    int startx    = MAX(fieldPosX - fieldRadius, 0);
    int starty    = MAX(fieldPosY - fieldRadius, 0);
    int endx    = MIN(fieldPosX + fieldRadius, fieldWidth);
    int endy    = MIN(fieldPosY + fieldRadius, fieldHeight);
    
    
    for (int i = startx; i < endx; i++){
        for (int j = starty; j < endy; j++){
            
            int pos = j * fieldWidth + i;
            float distance = (float)sqrt((fieldPosX-i)*(fieldPosX-i) +
                                         (fieldPosY-j)*(fieldPosY-j));
            
            if (distance < 0.0001) distance = 0.0001;  // since we divide by distance, do some checking here, devide by 0 is BADDDD
            
            if (distance < fieldRadius){
                
                float pct = 1.0f - (distance / fieldRadius);
                float strongness = strength * pct;
                field[pos].x += vx * strongness;
                field[pos].y += vy * strongness;
            }
        }
    }
}

//------------------------------------------------------------------------------------
void VectorField::set_outter_oval(float center_x, float center_y, float width, float height, float strength){
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    float pctx            = center_x / (float)externalWidth;
    float pcty            = center_y / (float)externalHeight;
    float widthPrc        = width / (float)externalWidth;
    float heightPrc        = height / (float)externalWidth;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    float field_dist_x    = (float)(widthPrc * fieldWidth);
    float field_dist_y    = (float)(heightPrc * fieldWidth);
    
    //go through the whole field
    for (int x=0; x<fieldWidth; x++){
        for (int y=0; y<fieldHeight; y++){
            int pos = y * fieldWidth + x;
            
            //https://www.geeksforgeeks.org/check-if-a-point-is-inside-outside-or-on-the-ellipse/
            float p = (powf((x - fieldPosX), 2) / powf(field_dist_x, 2))
            + (powf((y - fieldPosY), 2) / powf(field_dist_y, 2));
            if (p > 1){
                
                //get the vector to the center
                ofVec2f dir = ofVec2f(fieldPosX,fieldPosY) - ofVec2f(x,y);
                dir.normalize();
                
                field[pos] = dir * strength;
            }
        }
    }
}

//------------------------------------------------------------------------------------
void VectorField::point_unused_to_point(float center_x, float center_y, float strength){
    // x y and radius are in external dimensions.  Let's put them into internal dimensions:
    // first convert to pct:
    float pctx            = center_x / (float)externalWidth;
    float pcty            = center_y / (float)externalHeight;
    
    // then, use them here:
    int fieldPosX        = (int)(pctx * (float)fieldWidth);
    int fieldPosY        = (int)(pcty * (float)fieldHeight);
    
    //go through the whole field
    for (int x=0; x<fieldWidth; x++){
        for (int y=0; y<fieldHeight; y++){
            int pos = y * fieldWidth + x;
            
            if (field[pos].length() < 0.01){
                ofVec2f dir = ofVec2f(fieldPosX,fieldPosY) - ofVec2f(x,y);
                dir.normalize();
                
                field[pos] = dir * strength;
            }
            
        }
    }
}



//------------------------------------------------------------------------------------
VectorField::~VectorField(){
    
}
