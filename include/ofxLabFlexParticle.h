//
//  ofxLabFlexParticle.h
//  Basic openFrameworks Particle System
//
//  Created by Brett Renfer on 10/9/12.
//

#pragma once

#include "ofMain.h"

/**
 * Helper class that makes it easy to pass in particle initiation parameters
 *
 */
class ofxLabFlexParticleOptions {
public:
    
    /**
     * ofxLabFlexParticleOptions constructor
     *
     * @param pos               The starting position of the particle in the world
     * @param velocity          The starting velocity of the particle in the world
     * @param rotation          The starting rotation of the particle in the world
     * @param rotateVelocity    The starting rotation velocity of the particle in the world
     * @param radius            The starting radius
     * @param damping           The damping.  Damping causes a particle to lose forces over time
     *                          it can be thought of as 'air friction', etc
     *
     */
    ofxLabFlexParticleOptions(ofVec3f& pos,
                              ofVec3f& velocity,
                              ofVec3f& rotation,
                              ofVec3f& rotateVelocity,
                              float radius,
                              float damping)
    : pos(pos),
      velocity(velocity),
      rotation(rotation),
      rotateVelocity(rotateVelocity),
      radius(radius),
      damping(damping)
    
    { }
    
    // these are all left public since this is simply a helper class
    ofVec3f pos;
    ofVec3f velocity;
    ofVec3f rotation;
    ofVec2f rotateVelocity;
    
    float radius;
    float damping;

};



class ofxLabFlexParticle : public ofVec3f
{
public:
    
    /**
     * ofxLabFlexParticle constructor
     *
     * Use standard defaults, see setDefaults()
     */
    ofxLabFlexParticle();
    
    /**
     * ofxLabFlexParticle constructor
     *
     * Use standard defaults, but set the starting position
     *
     * @param pos   The starting position of the particle
     *
     * @return      A particle
     */
    ofxLabFlexParticle( const ofVec2f& pos );
    
    /**
     * ofxLabFlexParticle constructor
     *
     * Use standard defaults, but set the starting position
     *
     * @param x     Starting x position
     * @param y     Starting y position
     *
     * @return      A particle
     */
    ofxLabFlexParticle( float x,
                        float y );
    
    /**
     * ofxLabFlexParticle constructor
     *
     * Pass in all parameters
     *
     * @param opts  The extensive particle options
     *
     * @return      A particle
     */
    ofxLabFlexParticle( const ofxLabFlexParticleOptions& opts );
    
    /**
     * ofxLabFlexParticle virtual deconstructor
     *
     */
    virtual ~ofxLabFlexParticle() {}
    
    /**
     * ofxLabFlexParticle equal operator
     *
     * @param p     Particle to be set
     *
     */
    ofxLabFlexParticle& operator=( const ofxLabFlexParticle& p );

    /**
     * Causes this particle to repel AWAY from particle b
     * The current algorithm is rather general and will probably want to be
     * customized given your application
     *
     * @param b     Particle we are repelling FROM
     *
     */
    virtual void repel( const ofxLabFlexParticle& b );
    
    /**
     * Update this particle through space.  Ie move this particle given it's
     * given acceleration, etc.
     *
     */
    virtual void update();
    
    /**
     * Draw this particle.  This will only show a black circle with an inner
     * white circle of size radius.
     *
     */
    virtual void draw();
    
    /**
     * Get's the uniqueID of the particle
     *
     * @return  The unique id of this particle
     */
    unsigned long getUniqueID();
    
    /**
     * Set the particle uniqueID.  WARNING you should not change this
     * once the particle has been added to ofxLabParticleSystem since the 
     * system relies on this for identification
     *
     * @param id    Particle ID
     *
     */
    void setUniqueID( unsigned long id );
    
    /**
     * The particle age can be used to keep track of how long a particle has
     * been around.  The system curently doesn't do anything with particle age
     *
     * @param age   Age to set the particle to
     *
     */
    void setAge( int age );

    /**
     * The particle age can be used to keep track of how long a particle has
     * been around.  The system curently doesn't do anything with particle age
     *
     * @return      The age of the given particle
     *
     */
    int getAge();
    
    /**
     * A void pointer can be associated with this particle.  This allows extra
     * information to be assigned to a particle, for whatever reason
     *
     * @return      void pointer to an area of memory.  NULL if never set.
     *
     */
    void * getData();
    
    /**
     * A void pointer can be associated with this particle.  This allows extra
     * information to be assigned to a particle, for whatever reason
     *
     * @param data  Pointer (void cast) to the memory you want to keep track of
     *
     */
    void setData( void * data );
    
    /**
     *
     * @return      The seconds since this particle system was created
     *
     */
    float getStartSeconds();
    

    
    // left public for easy changing
    ofVec2f velocity;
    ofVec2f acceleration;
    
    ofVec3f rotation;
    ofVec3f rotateVelocity;
    
    float   radius;
    float   damping;
    float   mass;
    
protected:
    int age;
    
    float startSecond;
    
    void * data;
    
    virtual void setDefaults();
    
    unsigned long uniqueID;
    
};
