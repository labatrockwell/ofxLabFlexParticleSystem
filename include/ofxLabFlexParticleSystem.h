//
//  ofxLabFlexParticleSystem.h
//  Blooms
//
//  Created by Lucas Vickers on 10/9/12.
//  Copyright (c) 2012 Lab at Rockwell. All rights reserved.
//

/*
 
 This is the Rockwell ofxLabFlexParticleSystem.  The general idea behind this
 class is simple particle system that can provide needed flexibility
 such as callbacks and viewpoint stencil.  As we get into more advanced
 projects this class will run through bigger and badder particle systems
 such as bullit, box2d, or GPU particle systems - all the while keeping
 the interface stable for Rockwell projects.
 
 This class easily plugs into our ofxLabFlexVectorField
 
 Currently this is only 2d, although it supports using the z
 dimension for external purposes
 
 */


#pragma once

#include "ofxLabFlexParticle.h"
#include "ofxLabFlexVectorField.h"
#include "ofxLabFlexQuad.h"

#if defined _WIN64 || defined _WIN32
#include <functional>
#else
#include <tr1/functional>
#endif


class ofxLabFlexParticleSystem
{
public:
 
    
    // just makes things a little easier than typing this out every time
    
    typedef map<unsigned long, ofxLabFlexParticle*>                         Container;
    
    typedef map<unsigned long, ofxLabFlexParticle*>::iterator               Iterator;
    typedef map<unsigned long, ofxLabFlexParticle*>::reverse_iterator       RIterator;
    
    typedef map<unsigned long, ofxLabFlexParticle*>::const_iterator         const_Iterator;
    typedef map<unsigned long, ofxLabFlexParticle*>::const_reverse_iterator const_RIterator;
    
    
    // min mass for the particle, to prevent particles from having a
    //  0 mass and messing up calculations
    static const float MIN_PARTICLE_MASS;

    // divider for the vec field force.  This allows users to put in vec
    //  field forces of 3, 5, etc - something human readable, and lower
    //  the forces within the field to make it managable.
    static const float VEC_FIELD_FORCE_DIVIDER;
    
    
    // three world types are allowed so far, open and square.
    // Open is as you guess boundless
    // Square has 4 walls and by default the particles bounce off the walls
    // Quad is a quadrilateral, ie a lopsided square
    // this can be
    enum WorldType {
        OPEN = 0, 
        SQUARE,
        QUAD
    };
    
    
    // callbacks start at top and go in clockwise order for walls
    enum WallCallbackType { 
        TOP_WALL,
        RIGHT_WALL, 
        BOTTOM_WALL, 
        LEFT_WALL 
    };
    
    // should always be equal to the number of enums above
    static const int SUPPORTED_WALL_CALLBACKS = 4;

    
    /* sets internal options
        VERTICAL_WRAP = particles infinitely wrap around top and bottom of screen
                        (ie particle that goes off top instantly shows on bottom)
        HORIZONTAL_WRAP = particles infiinitely wrap around sides of screen
        VECTOR_FIELD = use the ofxLabFlexVectorField for calculations
        VECTOR_FIELD_DRAW = draw the ofxLabFlexVectorField forces (visual reference tool)
     */
    enum Options { 
        VERTICAL_WRAP       = (1u << 0),
        HORIZONTAL_WRAP     = (1u << 1),
        VECTOR_FIELD        = (1u << 2),
        VECTOR_FIELD_DRAW   = (1u << 3),
        DETECT_COLLISIONS   = (1u << 4)
    };
    
    /**
     * Virtual deconstructor
     */
    virtual ~ofxLabFlexParticleSystem() {}
    
    /**
     * Default constructor
     */
    ofxLabFlexParticleSystem();
    
    /**
     * Configure the particle system.  You should only call this once.
     *  This sets the particle system to the OPEN type so no parameters are needed
     */
    void setupOpen( );
    
    /**
     * Configure the particle system.  You should only call this once.
     *  This sets the particle system to the SQUARE type
     *
     * @param worldBox      For bound worlds this box defines the size.
     */
    void setupSquare( const ofVec2f& worldBox );
    
    /**
     * Configure the particle system.  You should only call this once.
     *
     * Configure the particle system as a QUAD type.  Provide the corners of the quad
     *
     * @param topLeft corner of Quad
     * @param bottomLeft corner of Quad
     * @param topRight corner of Quad
     * @param bottomRight corner of Quad
     */
    void setupQuad( ofVec2f& topLeft,
                    ofVec2f& bottomLeft,
                    ofVec2f& topRight,
                    ofVec2f& bottomRight);

    /**
     * Updates all particles in the system, applies vector fields if option is enabled
     * calls callbacks that are eneabled, etc.
     */
    virtual void update();
    
    /**
     * Draws the particles inside the windowStencil
     *
     * @param windowStencil     visual square you want drawn
     * @param rotate            what degree the windowStencil should be rotated
     */
    virtual void draw( const ofRectangle& windowStencil,
                       float rotate = 0.0f);
    
    /**
     * Inserts an ofxLabFlexParticle into the system.
     * NOTE: no memory management is done by this system
     *
     * @param particle      pointer to the ofxLabFlexParticle you want added
     */
    virtual void addParticle( ofxLabFlexParticle* particle );
    
    
    
    /**
     * Remove a given particle from the system.  Note that you CANNOT
     * remove a particle while the system is updating.  The system locks
     * to prevent this.
     * 
     * NOTE: no memory management is done by this system
     *
     * @param uniqueID      internal ID of the particle
     * @return              true if the particle was removed, false otherwise
     */
    virtual bool removeParticle( unsigned long uniqueID );
    
    /**
     * Clears all particles and resets uniqueID counter
     *
     * NOTE: no memory management is done by this system
     */
    virtual void clear();
	
    /**
     * Multiply the velocity of all particled by this force
     *
     * @param force     velocity multiplication force
     */
    virtual void multForce( const ofVec3f& force );
    
    /**
     * Add to the acceleration of all particled by this force
     *
     * @param force     acceleration addition force
     */
    virtual void addForce( const ofVec3f& force );
    
    /**
     * Return the number of particles currently in the system
     *
     * @return     the number of particles currently in system
     */
    int getNumParticles();
    
    
    /**
     * Set one of the given wall callbacks.  This callback will be called
     * while the particle system is in update() and a specific event occurs.
     *   An example is a wall bounce.  If the particle hits a wall we will call
     *   the appropriate wall callback.  If the override is true, then the particle
     *   system will not bounce the ball, and the callback must do this
     *   if the override is false, the ball will bunce and then the callback will be called
     *
     * @param func      function pointer that will act as the callback
     * @param type      type of callback (see WallCallbackType)
     * @param override  if true the particle system will not use internal logic
     *                  and call the callback instead.  If false the system will
     *                  do internal logic and then call the callback
     */
    void setWallCallback( std::tr1::function<void ( ofxLabFlexParticle* )> func,
                          WallCallbackType type,
                          bool override);
    
    /**
     * Enable or diable a given option.  See Options enum
     *
     *
     * @param option        option in question
     * @param enabled       should we enable or disable the option
     * @param param         optional parameter, depending on option
     */
    void setOption( Options  option,
                    bool enabled,
                    float param = 0.0f );
    
    
    /**
     * Get the internal container that holds our particles.  Be careful
     * as there is no lock associated with this container so you could easily
     * break things.
     *
     *
     * @return              The immutable particle container
     */
    Container const * getParticles();

	 /**
     * Get an individual particle out of the system based on the uniqueID.
     * Be careful not to touch this particle while the system is updating.
     *
     * @return              The particle pointer or NULL
     */
	ofxLabFlexParticle * getParticle( unsigned long uniqueID );
    
    /**
     * Print a list of the UniqueIDs inside of the particle system.
     * mainly used for debugging.
     *
     */
    void printIDs();
    
    /**
     * Return a pointer to the internal vector field that the particle
     * system is using.  This allows the user to configure the vector field
     * as they need.  This is good for any force type you want applied to all particles
     *
     * @return              ofxLabFlexVectorField pointer
     */
    ofxLabFlexVectorField * getVectorField();
    
    /**
     * Apply a given ofxLabFlexVectorField to the particles.  This is useful for advanced
     * functionality where multiple vector fields are needed.  Note that the particle
     * x,y will be plugged in directly to the vectorField.getForce(x,y), no translation
     * occurs.
     *
     * @param vectorField      ofnxVectorField that will be applied
     */
    void applyVectorField( const ofxLabFlexVectorField& vectorField );
    
    /**
     * Sets the maxinum number of particles that the system will hold.  Once we reach the limit
     * the addition of a new particle will result in the deletion of oldest particle (lowest uniqueID)
     *
     * @param maxParticles      maxinum particle number
     */
    void setMaxParticles( unsigned int maxParticles ) {
		_maxParticles = maxParticles;
	}
    
    /**
     * Checks to see if the particle falls within our cropping range
     *
     *
     * @param particle      particle to test
     * @param ws            the cropping rectangle
     * @param rotation      rotation of the cropping rectangle
     * @return              true if the particle should be drawn, false otherwise
     */
    bool shouldDraw( ofxLabFlexParticle* particle,
                     const ofRectangle& ws,
                     float rotation );
    
    ofxLabFlexQuad getWorldQuad() {
        return _worldQuad;
    }
    
protected:
    
    
    Container               _particles;    // holds the actual particles
    WorldType               _worldType;    // is it a bordered world, infinite world ?
    ofVec2f                 _worldBox;     // if square world, this is the boundaries
    ofxLabFlexQuad          _worldQuad;    // if quad world, this is the bounds
    
    
    // call back is for special interactions when particles hit wall boundaries
    // the override tells us whether or not we do standard functionality FIRST
    // before the callback or if we let the callback handle everything
    std::tr1::function<void ( ofxLabFlexParticle* )>    _wallCallbacks [SUPPORTED_WALL_CALLBACKS];
    
    // internal array of the different callbacks
    bool                    _wallCallbackOverride[SUPPORTED_WALL_CALLBACKS];

    ofMutex                 _updateLock;    // update lock, so we can protect memory
    
    unsigned int            _options;       // stores options mask
    
    ofxLabFlexVectorField   _vectorField;   // our vector field
    
    unsigned long           _nextID;        // for uniqueIDs

	unsigned int			_maxParticles;	// optional max particles
    
};

