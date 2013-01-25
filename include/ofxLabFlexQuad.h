//
//  ofxLabFlexQuad.h
//  example
//
//  Created by Lucas Vickers on 1/23/13.
//
//

#ifndef __example__ofxLabFlexQuad__
#define __example__ofxLabFlexQuad__

#include <iostream>
#include "ofMain.h"


class ofxLabFlexQuad
{
public:
    ofVec2f tl,tr,bl,br;


    ofxLabFlexQuad() {
        bBuilt = false;
    }
    
    // bounds checks return true IF triggered; e.g. if point is left of quad checkLeft returns true
    
    bool checkLeftBounds(ofVec2f test){
        return ((bl.x - tl.x)*(test.y - tl.y) - (bl.y - tl.y)*(test.x - tl.x)) > 0;
    }
    
    bool checkRightBounds(ofVec2f test){
        return ((br.x - tr.x)*(test.y - tr.y) - (br.y - tr.y)*(test.x - tr.x)) < 0;
    }
    
    bool checkTopBounds(ofVec2f test){
        return ((tr.x - tl.x)*(test.y - tl.y) - (tr.y - tl.y)*(test.x - tl.x)) < 0;
    }
    
    bool checkBottomBounds(ofVec2f test){
        return ((br.x - bl.x)*(test.y - bl.y) - (br.y - bl.y)*(test.x - bl.x)) > 0;
    }
    
    //sign( (Bx-Ax)*(Y-Ay) - (By-Ay)*(X-Ax) )
    void draw(){
        if ( !bBuilt ){
            mesh.addVertex(tl);
            mesh.addVertex(tr);
            mesh.addVertex(bl);
            mesh.addVertex(br);
            mesh.addIndex(0);
            mesh.addIndex(1);
            mesh.addIndex(2);
            mesh.addIndex(1);
            mesh.addIndex(2);
            mesh.addIndex(3);
            
            bBuilt = true;
        }
        mesh.drawWireframe();
    }
    
private:
    ofMesh mesh;
    bool bBuilt;
};

#endif /* defined(__example__ofxLabFlexQuad__) */
