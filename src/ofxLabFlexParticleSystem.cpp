//
//  ofxLabFlexParticleSystem.cpp
//  Blooms
//
//  Created by Lucas Vickers on 10/9/12.
//  Copyright (c) 2012 Lab at Rockwell. All rights reserved.
//

#include "ofxLabFlexParticleSystem.h"

// min mass for the particle, to prevent particles from having a
//  0 mass and messing up calculations
const float ofxLabFlexParticleSystem::MIN_PARTICLE_MASS        = .1;

// divider for the vec field force.  This allows users to put in vec
//  field forces of 3, 5, etc - something human readable, and lower
//  the forces within the field to make it managable.
const float ofxLabFlexParticleSystem::VEC_FIELD_FORCE_DIVIDER  = 100;


ofxLabFlexParticleSystem::ofxLabFlexParticleSystem()
{
    _options = 0;
    _nextID = 0;
    
    for(int i=0; i<SUPPORTED_WALL_CALLBACKS; ++i) {
        _wallCallbackOverride[i] = false;
        _wallCallbacks[i] = NULL;
    }

	_maxParticles = 0;
}


void ofxLabFlexParticleSystem::setupOpen( )
{
    _worldType = OPEN;
}


void ofxLabFlexParticleSystem::setupSquare( const ofVec2f& worldBox )
{
    _worldType = SQUARE;
    _worldBox = worldBox;
}

void ofxLabFlexParticleSystem::setupQuad( ofVec2f& topLeft,
                                          ofVec2f& bottomLeft,
                                          ofVec2f& topRight,
                                          ofVec2f& bottomRight )
{

    _worldType = QUAD;
    _worldQuad.tl = topLeft;
    _worldQuad.tr = topRight;
    _worldQuad.bl = bottomLeft;
    _worldQuad.br = bottomRight;
}

void ofxLabFlexParticleSystem::setWallCallback( std::tr1::function<void ( ofxLabFlexParticle* )> func,
                                         WallCallbackType type,
                                         bool override)
{
    _wallCallbacks[type] = func;
    _wallCallbackOverride[type] = override;
    
}

void ofxLabFlexParticleSystem::setOption(Options  option,
                                  bool enabled,
                                  float param)
{
    
    if( enabled ) {
        _options |= option;
    } else {
        _options &= ~option;
    }
    
    if( option == VECTOR_FIELD && enabled) {
        
        
        // zero param means the vector field will default in size
        _vectorField.setupField( _worldBox.x, 
                                 _worldBox.y,
                                 _worldBox.x * param,
                                 _worldBox.y * param);
         
    }
    
}

ofxLabFlexVectorField* ofxLabFlexParticleSystem::getVectorField()
{
    return &_vectorField;
}

ofxLabFlexParticleSystem::Container const * ofxLabFlexParticleSystem::getParticles()
{
    return &_particles;
}

ofxLabFlexParticle* ofxLabFlexParticleSystem::getParticle( unsigned long uniqueID )
{
	Poco::ScopedLock<ofMutex> scopeLock(_updateLock);
	Iterator it;
	it = _particles.find( uniqueID );
	if( it != _particles.end() ) {
		return it->second;
	}
	return NULL;
}
    

void ofxLabFlexParticleSystem::applyVectorField( const ofxLabFlexVectorField& externalVectorField )
{
    ofVec2f vecFieldForce;
    
    Iterator it = _particles.begin();
    for( it = _particles.begin(); it != _particles.end(); ++it )
    {
        ofxLabFlexParticle* p = it->second;
        vecFieldForce = externalVectorField.getForceFromPos(p->x, p->y);
    
        p->acceleration += vecFieldForce / MIN(p->mass, MIN_PARTICLE_MASS) / VEC_FIELD_FORCE_DIVIDER;
    }
}


void ofxLabFlexParticleSystem::update()
{
    
    // create a scoped lock
    Poco::ScopedLock<ofMutex> scopeLock(_updateLock);
    
    ofVec2f vecFieldForce;
    
    Iterator it = _particles.begin();
    for( it = _particles.begin(); it != _particles.end(); ++it )
    {
        ofxLabFlexParticle* p = it->second;
        p->update();
        
        // check collision
        Iterator inner_it = it;
        for(++inner_it; inner_it != _particles.end(); ++inner_it )
        {
            
            ofxLabFlexParticle* inner_p = inner_it->second;
            
            //if( (*it)->distance(*(*inner_it)) <= (*it)->radius + (*inner_it)->radius ) {
            //    //cout << "collision!" << endl;
            //
            //}
             
            p->repel( *inner_p );
            inner_p->repel( *p );

        }
        
        if( _options & VECTOR_FIELD ) {
            vecFieldForce = _vectorField.getForceFromPos(p->x, p->y);
            
            p->acceleration += vecFieldForce / MIN(p->mass, MIN_PARTICLE_MASS) / VEC_FIELD_FORCE_DIVIDER;
        }
        
        
        // if we are an open world don't do any edge detection
        if( _worldType == OPEN ) {
            continue;
        }
        
        // NOTE
        //  These wall routines are repeatative, but since we want to hit each wall one by one
        //  we have all 4.  TODO figure out how to combine them
        
        /* LOGIC
            The logic for each is as follows
         
            - check to see if we hit given wall
              * if we have a callback and override
                - call callback
              * else
                - if we have a wrap set
                  * do the wrap
                - else
                  * bounce the particle
                - if we have a callback
                  * call the callback
        */
        
        if ( _worldType == SQUARE ){
            
            // top wall
            if( p->y <= 0 ) {
                // if callback and override, only call callback
                if( _wallCallbacks[TOP_WALL] && _wallCallbackOverride[TOP_WALL] ) {
                
                    _wallCallbacks[TOP_WALL](it->second);

                } else {
                    // otherwise do our internal logic, and hit the callback if one exists

                    if( _options & VERTICAL_WRAP ) {
                        p->y += p->velocity.y + _worldBox.y;
                    } else {
                        // put it in the right direction
                        p->velocity.y = (p->velocity.y < 0 ? p->velocity.y * -1 : p->velocity.y);
                        p->y += p->velocity.y;
                    }
                    
                    if( _wallCallbacks[TOP_WALL] ) {
                        _wallCallbacks[TOP_WALL](it->second);
                    }
                }
            }
            
            // right wall
            if( p->x - p->radius >= _worldBox.x ) {
                if( _wallCallbacks[RIGHT_WALL] && _wallCallbackOverride[RIGHT_WALL] ) {
           
                    _wallCallbacks[RIGHT_WALL](it->second);
            
                } else {
                    
                    if( _options & HORIZONTAL_WRAP ) {
                        p->x += p->velocity.x - _worldBox.x;
                        p->x = 0;
                    } else {
                        p->velocity.x = (p->velocity.x > 0 ? p->velocity.x * -1 : p->velocity.x);
                        p->x += p->velocity.x;
                    }
                    
                    if( _wallCallbacks[RIGHT_WALL] ) {
                        _wallCallbacks[RIGHT_WALL](it->second);
                    }
                }
            }
            
            // bottom wall
            if( p->y >= _worldBox.y ) {
                if( _wallCallbacks[BOTTOM_WALL] && _wallCallbackOverride[BOTTOM_WALL] ) {
             
                    _wallCallbacks[BOTTOM_WALL](it->second);
             
                } else {
                    
                    if( _options & VERTICAL_WRAP ) {
                        p->y += p->velocity.y - _worldBox.y;
                    } else {
                        p->velocity.y = (p->velocity.y > 0 ? p->velocity.y * -1 : p->velocity.y);
                        p->y += p->velocity.y;
                    }
                    
                    if( _wallCallbacks[BOTTOM_WALL] ) {
                        _wallCallbacks[BOTTOM_WALL](it->second);
                    }
                }
            }
            
            // left wall
            if( p->x + p->radius <= 0 ) {
                if( _wallCallbacks[LEFT_WALL] && _wallCallbackOverride[LEFT_WALL] ) {
              
                    _wallCallbacks[LEFT_WALL](it->second);
            
                } else {
                    
                    if( _options & HORIZONTAL_WRAP ) {
                        p->x += p->velocity.x + _worldBox.x;
                        p->x = _worldBox.x;
                    } else {
                        p->velocity.x = (p->velocity.x < 0 ? p->velocity.x * -1 : p->velocity.x);
                        p->x += p->velocity.x;
                    }
                    
                    if( _wallCallbacks[LEFT_WALL] ) {
                        _wallCallbacks[LEFT_WALL](it->second);
                    }
                }
            
            }
        } else if ( _worldType == QUAD ){
            
            
            // top wall
            if( _worldQuad.checkTopBounds(*p) ) {
                // if callback and override, only call callback
                if( _wallCallbacks[TOP_WALL] && _wallCallbackOverride[TOP_WALL] ) {
                    
                    _wallCallbacks[TOP_WALL](it->second);
                    
                } else {
                    // otherwise do our internal logic, and hit the callback if one exists
                    
                    if( _options & VERTICAL_WRAP ) {
                        p->y += p->velocity.y + _worldBox.y;
                    } else {
                        // put it in the right direction
                        p->velocity.y = (p->velocity.y < 0 ? p->velocity.y * -1 : p->velocity.y);
                        p->y += p->velocity.y;
                    }
                    
                    if( _wallCallbacks[TOP_WALL] ) {
                        _wallCallbacks[TOP_WALL](it->second);
                    }
                }
            }
            
            // right wall
            if ( _worldQuad.checkRightBounds(ofVec2f(p->x - p->radius, p->y)) && p->velocity.x > 0){
            //if( p->x - p->radius >= _worldBox.x ) {
                if( _wallCallbacks[RIGHT_WALL] && _wallCallbackOverride[RIGHT_WALL] ) {
                    
                    _wallCallbacks[RIGHT_WALL](it->second);
                    
                } else {
                    
                    if( _options & HORIZONTAL_WRAP ) {
                        p->x = ofLerp(_worldQuad.tl.x, _worldQuad.bl.x, p->y/(_worldQuad.tl.y-_worldQuad.bl.y)) - p->radius;
                    } else {
                        p->velocity.x = (p->velocity.x > 0 ? p->velocity.x * -1 : p->velocity.x);
                        p->x += p->velocity.x;
                    }
                    
                    if( _wallCallbacks[RIGHT_WALL] ) {
                        _wallCallbacks[RIGHT_WALL](it->second);
                    }
                }
            }
            
            // bottom wall
            if ( _worldQuad.checkBottomBounds(*p) ){
                if( _wallCallbacks[BOTTOM_WALL] && _wallCallbackOverride[BOTTOM_WALL] ) {
                    
                    _wallCallbacks[BOTTOM_WALL](it->second);
                    
                } else {
                    
                    if( _options & VERTICAL_WRAP ) {
                        p->y += p->velocity.y - _worldBox.y;
                    } else {
                        p->velocity.y = (p->velocity.y > 0 ? p->velocity.y * -1 : p->velocity.y);
                        p->y += p->velocity.y;
                    }
                    
                    if( _wallCallbacks[BOTTOM_WALL] ) {
                        _wallCallbacks[BOTTOM_WALL](it->second);
                    }
                }
            }
            
            // left wall
            if ( _worldQuad.checkLeftBounds(ofVec2f(p->x + p->radius, p->y)) && p->velocity.x < 0 ){
                if( _wallCallbacks[LEFT_WALL] && _wallCallbackOverride[LEFT_WALL] ) {
                    
                    _wallCallbacks[LEFT_WALL](it->second);
                    
                } else {
                    
                    if( _options & HORIZONTAL_WRAP ) {
                        p->x = p-> radius + ofLerp(_worldQuad.tr.x, _worldQuad.br.x, fabs(p->y/(_worldQuad.tl.y-_worldQuad.bl.y)));
                    } else {
                        p->velocity.x = (p->velocity.x < 0 ? p->velocity.x * -1 : p->velocity.x);
                        p->x += p->velocity.x;
                    }
                    
                    if( _wallCallbacks[LEFT_WALL] ) {
                        _wallCallbacks[LEFT_WALL](it->second);
                    }
                }
                
            }
        }
    }
    
    //cout << "end update" << endl;
}

/*
void ofxLabFlexParticleSystem::draw()
{
    cerr << "The regular draw is not yet tested." << endl;
    
    return;
    
    // TODO this needs to be fixed
    pContainerIterator it;
    for( it = _particles.begin(); it != _particles.end(); ++it )
    {
        ofxLabFlexParticle* p = (it->second);
        p->draw();

        if( _worldType == SQUARE) {
            // do wrap checks
        
            if( _options & HORIZONTAL_WRAP ) {
                if( p->x + p->radius > _worldBox.x || p->x - p->radius < 0 ) {
                    // mirror horizontal
                }
            }
            if( _options & VERTICAL_WRAP ) {
                if( p->y + p->radius > _worldBox.y || p->y - p->radius < 0 ) {
                    // mirror vertical
                }
            }
        }
    
    }
}
 */

bool ofxLabFlexParticleSystem::shouldDraw( ofxLabFlexParticle* p, 
                                    const ofRectangle& ws,
                                    float rotation)
{
	//return true;

    //particle rotated
    ofVec2f pr(p->x, p->y);
 
    pr.rotate( -rotation,
               ws.getCenter() );
    
    
    // TODO figure out how to not increase this buffer by 2x and still not lose corners
    // on hard rotations
    
	if( p->x + p->radius >= ws.getMinX() - ws.getWidth() &&
        p->x - p->radius <= ws.getMaxX() + ws.getWidth() &&
        p->y + p->radius >= ws.getMinY() - ws.getHeight() &&
        p->y - p->radius <= ws.getMaxY() + ws.getHeight() )
    {
        return true;
    }
    
    return false;
}

void ofxLabFlexParticleSystem::draw( const ofRectangle& ws,
                              float rotation )
{


    float tempf;
    
    ofVec2f middleBox = _worldBox / 2;
    
    Iterator it;
    
    for( it = _particles.begin(); it != _particles.end(); ++it )
    {
        ofxLabFlexParticle* p = (it->second);
        
        if( shouldDraw( p, ws, rotation ) ) {
            p->draw();
        }
        
        
        // need to check if the particle wrap is inside
        if( _worldType == SQUARE ) {
            
            if( _options & HORIZONTAL_WRAP ) {
                if( p->x + p->radius > _worldBox.x ) {
                    // check right side of screen and wrap back to left if needed
                    
                    tempf = p->x;
                    p->x -= _worldBox.x;
                    if( shouldDraw( p, ws, rotation ) ) {
                        p->draw();
                    }
                    p->x = tempf;
                } else if ( p->x - p->radius < 0 ) {
                    // check right side of screen and wrap back to right if needed
                    
                    tempf = p->x;
                    tempf = p->x;
                    p->x += _worldBox.x;
                    if( shouldDraw( p, ws, rotation ) ) {
                        p->draw();
                    }
                    p->x = tempf;                
                }
            }
            
            if( _options & VERTICAL_WRAP ) {
                if( p->y + p->radius > _worldBox.y ) {
                    // check bottom side of screen and wrap back to top if needed
                    
                    tempf = p->y;
                    p->y -= _worldBox.y;
                    if( shouldDraw( p, ws, rotation ) ) {
                        p->draw();
                    }
                    p->y = tempf;
                } else if ( p->y - p->radius < 0 ) {
                    // check top side of screen and wrap back to bottom if needed
                    
                    tempf = p->y;
                    p->y += _worldBox.y;
                    if( shouldDraw( p, ws, rotation ) ) {
                        p->draw();
                    }
                    p->y = tempf;
                }
            }
        }
        
    }
    
    
    //cout << "bool is " << ( _options & VECTOR_FIELD & VECTOR_FIELD_DRAW ) << endl;
    if( (_options & VECTOR_FIELD) && (_options & VECTOR_FIELD_DRAW) )  {
        //cout << "drawing.. " << endl;
        _vectorField.draw( ws );
    }
}

void ofxLabFlexParticleSystem::addParticle( ofxLabFlexParticle* p )
{
    Poco::ScopedLock<ofMutex> scopeLock(_updateLock);
    
    p->setUniqueID(_nextID++);
    
    _particles[p->getUniqueID()] = p;

	while(_maxParticles > 0 && _particles.size() > _maxParticles) {
        cout << "add erase" << endl;
		_particles.erase( _particles.begin() );
	}

}

void ofxLabFlexParticleSystem::printIDs()
{
    Iterator it;
    for( it = _particles.begin(); it != _particles.end(); ++it ) {
        cout << it->first << ", ";
    }
    cout << endl << endl;
    
}

bool ofxLabFlexParticleSystem::removeParticle( unsigned long uniqueID )
{
    //cout << "trying to delete particle " << uniqueID << endl;
    if( !_updateLock.tryLock() ) {
        cerr << "unagle to obtain lock" << endl;
        return false;
    }
    
    Iterator it = _particles.find(uniqueID);
    
    if( it == _particles.end() ) {

        //_updateLock.unlock();
        cerr << "unable to find particle " << uniqueID << endl;
        return false;
    }

	_particles.erase( it );

    _updateLock.unlock();
    
    return true;
}


void ofxLabFlexParticleSystem::clear(){
    // TO DO: wait for lock
    //while( !_updateLock.tryLock() ){}
    if ( _particles.size() == 0 ){
        return;
    }

    _updateLock.lock();
    _particles.clear();
    _nextID = 0;
    _updateLock.unlock();
}

void ofxLabFlexParticleSystem::multForce( const ofVec3f& force )
{
    Iterator it;
    for( it = _particles.begin(); it != _particles.end(); ++it ) {
        (*it).second->velocity *= force;
    }
}

void ofxLabFlexParticleSystem::addForce( const ofVec3f& force )
{
    Iterator it;
    for( it = _particles.begin(); it != _particles.end(); ++it ) {
        (*it).second->acceleration += force;
    }
}

int ofxLabFlexParticleSystem::getNumParticles()
{
    return _particles.size();
}

