#ifndef OFX_VECTORFIELD_H
#define OFX_VECTORFIELD_H


#include "ofMain.h"

class ofxLabFlexVectorField {
	
public:
    
    // these constants are for display
	static const float FORCE_DISPLAY_SCALE;
	static const float BASELINE_SCALE;
    
    // default internal to external scale.  Ie if the external world
    // is 500x500, and our scale is .1, then our vector field has 50x50 points
    static const float DEFAULT_SCALE;
	
    /**
     * default constructor
     */
	ofxLabFlexVectorField();
    
    /**
     * default virtual deconstructor
     */
	virtual ~ofxLabFlexVectorField();
	
    /**
     * Set up the vector field.
     *
     * @param externalWidth     The width of the world size this vector field will be 
     *                          applied to (ie a window size)
     * @param externalHeight    The height of the world size this vector field will be 
     *                          applied to (ie a window size)
     * @param fieldWidth        The width of the internal representation.  Ie how many 
     *                          force points internally?  Leave blank to default to a field based
     *                          on the DEFAULT_SCALE
     * @param fieldHeight       The height of the internal representation. Ie how many
     *                          force points internally?  Leave blank to default to a field based
     *                          on the DEFAULT_SCALE
     *
     */
	void setupField( int externalWidth, 
                     int externalHeight, 
                     int fieldWidth = 0, 
                     int fieldHeight = 0);
	
    
    /**
     * This will set the force for all vector points to 0.  
     * This will not change the field size, only set all values to 0 so they have no affect
     */
	void zeroField();
	
    /**
     * This will let you lower the field values over time.  All field forces will
     * be set to oldForce * fadeAmount.  ie pass in .99 or so
     *
     * @param fadeAmount    The percentage of the force to be LEFT.  NewForce = oldForce * fadeAmount
     */
	void fadeField(float fadeAmount);
	
    /**
     * Randomly sets all field values to a range between -range and range
     */
	void randomizeField(float range);
    
    
    /**
     * Visually displays a section of the field.  This is very valuable for debugging.
     * Only the specified subsection of the field will be shown if a cropSection is passed
     * otherwise the entire field will be drawn.
     * 
     * Field is drawn starting at 0,0
     *
     * @param cropSection   If passed only this section of the vector field will be drawn
     */
	void draw( const ofRectangle& cropSection = ofRectangle(0, 0, 0, 0) );
    
    /**
     * Pulls the value of the closest point in the vector field.
     * This takes into account the fieldOffset, fieldShift, and scale if any of them are set
     *
     * @param x     The x coordinate (external world coordinate)
     * @param y     The y coordinate (external world coordinate)
     *
     * @return      The 2 dimensional force at this location
     */
	ofVec2f getForceFromPos( float posX,
                             float posY) const;
	
    /**
     * Adds a force in a circular pattern that directs away from the center.
     *
     * @param x         The x center of the location of force. (external world coordinate).
     * @param y         The y center of the location of force. (external world coordinate).
     * @param radius    The radius that the force is applied to (how big is the circle).
     * @param strength  The strength of the force.
     */
	void addOutwardCircle( float x,
                           float y, 
						   float radius, 
                           float strength);

    /**
     * Adds a force in a circular pattern that directs towards the center.
     *
     * @param x         The x center of the location of force. (external world coordinate).
     * @param y         The y center of the location of force. (external world coordinate).
     * @param radius    The radius that the force is applied to (how big is the circle).
     * @param strength  The strength of the force.
     */
	void addInwardCircle( float x, 
                          float y, 
                          float radius, 
                          float strength);
	
    /**
     * Adds a force in a circular pattern that rotates around the center in clockwise fashion
     *
     * @param x         The x center of the location of force. (external world coordinate).
     * @param y         The y center of the location of force. (external world coordinate).
     * @param radius    The radius that the force is applied to (how big is the circle).
     * @param strength  The strength of the force.
     */
	void addClockwiseCircle( float x,
                             float y, 
							 float radius, 
							 float strength);
	
    /**
     * Adds a force in a circular pattern that rotates around the center in counter-clockwise fashion
     *
     * @param x         The x center of the location of force. (external world coordinate).
     * @param y         The y center of the location of force. (external world coordinate).
     * @param radius    The radius that the force is applied to (how big is the circle).
     * @param strength  The strength of the force.
     */
	void addCounterClockwiseCircle( float x,
                                    float y, 
									float radius, 
									float strength);
    
    /**
     * Sets a uniform/constant force to a given section of the vector field.  The area
     * is external world coordinates.
     *
     * @param area      Section of the vector field this force is applied to
     * @param force     The value this section of the vector field will be applied to
     */
    void setUniformForce( const ofRectangle& area,
                          const ofVec2f& force );
    
    
    /**
     * Consider you are modeling a whirlpool inside of a larger lake.  You want to
     * have this whirlpool move around the lake, so you have two options.  You either
     * clear out the vector field and re-generate the whirlpool each time it moves,
     * or you call this and you can shift the entire vector field.  Currently this
     * only works on the horizontal (can't do horizontal and vertical shifts) but the
     * vertical could be extended relatively easily.
     *
     * Internall this keeps all values of the vector field the same, it just calculates
     * values given a certain perfentage of shift.
     *
     *
     * @param area      Section of the vector field this force is applied to
     * @param force     The value this section of the vector field will be applied to
     */
    void setHorizontalShift( float shiftPct );
    
    

    /**
     * Scale the vector field, without actually affecting internal values.  You can
     * set this to 0 to temporarily make all forces equal (0,0), then set it to 1
     * later on to get all those forces back to the way they were.
     *
     * @param scale     The scale to set the vector field to
     */
    void setScale( float scale );
    
    /**
     * You have a large screen, and you have 4 different force areas.  You can either
     * create one vector field and set the forces in 4 different areas, or you can have
     * 4 different vector fields and set each one to have an offset so that calculations
     * are based on the new location of the vector field.
     *
     * @param offset    New location for the top left corner of the vector field.  If you set to
     *                  (100, 100) then the top left corner of the vector field will be at point
     *                  (100, 100).
     */
    void setExternalOffset( ofVec2f offset );
    
    
    // this maps a sin wave onto the grid, causing the values of the field to fluctuate
    // based on the sin map.
    
    /**
     * This applies a sin wave to the values of the field.  This sin wave can be repeated
     * a given number of times over the horizontal and vertical.  This is an easy way to continually
     * and predictably fluctuate the field.  This will be visible when you call draw()
     *
     * @param xRepeat
     * @param yRepeat
     * @param xPhase
     * @param yPhase
     * @param sinPowerValue
     * @param positiveClamp
     *
     */
    void applySinMap( float xRepeat,                // repeat the sin wave X times on horizontal field
                      float yRepeat,                // repeat the sin wave X times on vertical field
                      float xPhase,                 // offset the x phase
                      float yPhase,                 // offset the y phase
                      float sinPowerValue = 1,      // pow(sin(), sinPowerValu) lets you adjust the sin wav
                      // sinPowerValue will be ignored if < 1 and we are working with a negative value
                      bool positiveClamp = false);  // lets you force the use of only positive values

    /**
     * Clears the sin map so it will no longer be used for calculations
     *
     */
    void clearSinMap();
    
    
    ///////////// advanced usage functions

    /**
     * Returns the internalSize of the field.  Remember externalSize * scale = internalSize
     *
     * @return  The x,y internal size of the field
     */
    ofVec2f getInternalSize();

    /**
     * Returns the externalSize of the field.  Remember externalSize * scale = internalSize
     *
     * @return  The x,y external size of the field
     */
    ofVec2f getExternalSize();
    
    /**
     * Returns a pointer to the vector that represents the internal field.
     * The size of the vector is internalSize.x * internalSize.y
     * Use this to do advanced adjustments to the field.  
     * DO NOT CHANGE THE SIZE OF THE VECTOR
     *
     * @return  Pointer to the internal vector that holds the field representation
     */
    vector<ofVec2f>* getField();
    
    
    
protected:
	
	// the internal dimensions of the field:    (ie, 60x40, etc)
	int _fieldWidth;
	int _fieldHeight;
	int _fieldSize;   // total number of "pixels", ie w * h
	
	// the external dimensions of the field:   (ie, 1024x768)
    //  ie what it scales to
	int _externalWidth;
	int _externalHeight;
    
    // offset external calculations by a given amount
    ofVec2f _externalOffset;
    
    float _scale;
    float _horShiftPct;
    
    float _sinXRepeat;
    float _sinYRepeat;
    float _sinXPhase;
    float _sinYPhase;
    float _sinPowerValue;
    
    bool _bUseSinMap;
    bool _bClampSinPositive;
    
	
	vector <ofVec2f> _field;
	
	enum ForceType {
        OUT_CIRCLE, 
        IN_CIRCLE, 
        CLOCK_CIRCLE, 
        COUNTER_CLOCK_CIRCLE 
    };
	
	void addForce(float x,
                  float y,
                  ForceType type,
                  float radius,
                  float strength);
    
};

#endif // OFX_VECTORFIELD_H
