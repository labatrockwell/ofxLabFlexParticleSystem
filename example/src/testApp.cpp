#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    // just used to draw the wireframe
    squareMesh.tl = ofVec2f(0, 0);
    squareMesh.tr = ofVec2f(400, 0);
    squareMesh.bl = ofVec2f(0, 400);
    squareMesh.br = ofVec2f(400, 400);
    
    ofVec2f squareBounds( 400, 400 );
    squareWorld.setupSquare( squareBounds );
    squareWorld.setOption( ofxLabFlexParticleSystem::HORIZONTAL_WRAP, true );
    
    ofVec2f tl(0, 0);
    ofVec2f bl(50, 400);
    ofVec2f tr(500, 0);
    ofVec2f br(400, 400);

    quadWorld.setupQuad( tl, bl, tr, br );
    
    
    // print callbacks
    squareWorld.setWallCallback( tr1::bind(&testApp::wallCallbackSquare, this, tr1::placeholders::_1),
                                ofxLabFlexParticleSystem::TOP_WALL,
                                false ); // don't override
    squareWorld.setWallCallback( tr1::bind(&testApp::wallCallbackSquare, this, tr1::placeholders::_1),
                                ofxLabFlexParticleSystem::BOTTOM_WALL,
                                false ); // don't override
    squareWorld.setWallCallback( tr1::bind(&testApp::wallCallbackSquare, this, tr1::placeholders::_1),
                                ofxLabFlexParticleSystem::LEFT_WALL,
                                false ); // don't override
    squareWorld.setWallCallback( tr1::bind(&testApp::wallCallbackSquare, this, tr1::placeholders::_1),
                                ofxLabFlexParticleSystem::RIGHT_WALL,
                                false ); // don't override
    
    
    quadWorld.setWallCallback( tr1::bind(&testApp::wallCallbackQuad, this, tr1::placeholders::_1),
                               ofxLabFlexParticleSystem::TOP_WALL,
                               false ); // don't override
    quadWorld.setWallCallback( tr1::bind(&testApp::wallCallbackQuad, this, tr1::placeholders::_1),
                               ofxLabFlexParticleSystem::BOTTOM_WALL,
                               false ); // don't override
    quadWorld.setWallCallback( tr1::bind(&testApp::wallCallbackQuad, this, tr1::placeholders::_1),
                               ofxLabFlexParticleSystem::LEFT_WALL,
                               false ); // don't override
    quadWorld.setWallCallback( tr1::bind(&testApp::wallCallbackQuad, this, tr1::placeholders::_1),
                               ofxLabFlexParticleSystem::RIGHT_WALL,
                               false ); // don't override
    
    
    
    ofVec3f pos(200, 200);
    ofVec3f velocity( ofRandom(-5, 5),
                      ofRandom(-5, 5),
                      0 );
    ofVec3f rotation( 0, 0, 0 );
    ofVec3f rotationVelocity( 0, 0, 0 );
    float radius = 10;
    float damping = 1.0f;
    
    
    ofxLabFlexParticleOptions pOpts( pos,
                                     velocity,
                                     rotation,
                                     rotationVelocity,
                                     radius,
                                     damping);
    
    // make two particles
    particles.push_back( ofxLabFlexParticle( pOpts ) );
    particles.push_back( ofxLabFlexParticle( pOpts ) );
    
    // add particles to diff systems
    
    squareWorld.addParticle( &particles[0] );
    quadWorld.addParticle( &particles[1] );
    
    ofSetWindowShape(1200, 600);
}

//--------------------------------------------------------------
void testApp::update(){
    squareWorld.update();
    quadWorld.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    stringstream ss;
    ss << "FPS: " << ofGetFrameRate();
    ofDrawBitmapString(ss.str(), ofPoint(50,50));

    ofRectangle stencil( 0, 0,
                         500,500 );
    
    ofPushMatrix(); {
        ofTranslate(100, 100);
        
        squareMesh.draw();
        
        squareWorld.draw(stencil);
        
        
        ofPushMatrix(); {
            ofTranslate(500, 0);
            
            quadWorld.getWorldQuad().draw();
            quadWorld.draw(stencil);
            
        } ofPopMatrix();

    } ofPopMatrix();
}

void testApp::wallCallbackSquare( ofxLabFlexParticle* p )
{
    stringstream ss;
    
    ss << "Square World Particle " << p->getUniqueID() << " has hit a wall.";
    ofLog( OF_LOG_NOTICE, ss.str() );
}

void testApp::wallCallbackQuad( ofxLabFlexParticle* p )
{
    stringstream ss;
    
    ss << "Quad World Particle " << p->getUniqueID() << " has hit a wall.";
    ofLog( OF_LOG_NOTICE, ss.str() );
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}