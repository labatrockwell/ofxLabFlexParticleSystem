//
//  ofxLabFlexParticle.cpp
//  Blooms
//
//  Created by rockwell on 10/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxLabFlexParticle.h"

ofxLabFlexParticle::ofxLabFlexParticle()
{
    setDefaults();
    set(0.0f, 0.0f);
}

ofxLabFlexParticle::ofxLabFlexParticle( const ofVec2f& pos )
{
    setDefaults();
    set(pos);
}

ofxLabFlexParticle::ofxLabFlexParticle( float x, float y ){
    setDefaults();
    set(x,y);
}

ofxLabFlexParticle::ofxLabFlexParticle( const ofxLabFlexParticleOptions& opts ){
    setDefaults();
	set(opts.pos);
	damping  = opts.damping;
	radius	 = opts.radius;
	rotation = opts.rotation;
	velocity = opts.velocity;
	rotateVelocity = opts.rotateVelocity;
}

void ofxLabFlexParticle::setDefaults(){
    radius = 2;
    damping = 1.0f;
    mass = 1;
    age = 0;
    startSecond = ofGetElapsedTimef();
    uniqueID = 0;
    data = NULL;
}

void ofxLabFlexParticle::update(){
    age++;
    
	// update position and rotation
    velocity += acceleration;
    acceleration *= 0;
    velocity *= damping;
    rotation += rotateVelocity;
    rotateVelocity *= damping;
	
    *this += velocity;
	//cout << (*this) << endl;
}

void ofxLabFlexParticle::draw(){

	ofSetColor(0, 0, 0);
	ofCircle(*this, radius);
    
    ofSetColor(255, 255, 255);
	ofCircle(*this, radius * .5);


}

void ofxLabFlexParticle::repel(const ofxLabFlexParticle& b)
{
    // TODO this should be explored, right now it's pretty straight forward
    // and probably doesn't even work that great
 
    float distance = ofDist(x, y, b.x, b.y);
    
    if( distance > b.radius + radius ) {
        return;
    }
    
    // ofVec2f - ofVec2f
    ofVec3f diff = ofVec3f::operator-( b );
    
    diff.normalize();
    
    // http://www.grantchronicles.com/astro09.htm
    // this is a really lose interpretation.. like not very close
    float force = b.mass * mass / MAX(1, distance);
    float accel = force / mass;
    
    acceleration += diff * accel;
}


ofxLabFlexParticle& ofxLabFlexParticle::operator=(const ofxLabFlexParticle& p)
{
    ofVec3f::operator=(p);
    damping  = p.damping;
    radius	 = p.radius;
    rotation = p.rotation;
    velocity = p.velocity;
    rotateVelocity = p.rotateVelocity;
    
    return *this;
}

void ofxLabFlexParticle::setAge( int _age ){
    age = _age;
}

int ofxLabFlexParticle::getAge(){
    return age;
}

float ofxLabFlexParticle::getStartSeconds() {
    return startSecond;
}

void * ofxLabFlexParticle::getData(){
    return data;
}

void ofxLabFlexParticle::setData( void * data ){
    this->data = data;
}

unsigned long ofxLabFlexParticle::getUniqueID()
{
    return uniqueID;
}

void ofxLabFlexParticle::setUniqueID( unsigned long id )
{
    uniqueID = id;
}



