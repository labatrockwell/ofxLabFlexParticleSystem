#include "ofxLabFlexVectorField.h"


// these constants are for display
const float ofxLabFlexVectorField::FORCE_DISPLAY_SCALE     = 5.0f;
const float ofxLabFlexVectorField::BASELINE_SCALE          = 0.2f;

// default internal to external scale, pixels to internal mapping
const float ofxLabFlexVectorField::DEFAULT_SCALE           = 0.1f;


//------------------------------------------------------------------------------------
ofxLabFlexVectorField::ofxLabFlexVectorField() :
_fieldWidth(0),
_fieldHeight(0),
_fieldSize(0),
_externalWidth(0),
_externalHeight(0),
_horShiftPct(0),
_scale(1),
_field(),
_sinPowerValue(1),
_bUseSinMap(false),
_bClampSinPositive(false)

{
	
}


//------------------------------------------------------------------------------------
ofxLabFlexVectorField::~ofxLabFlexVectorField(){
    
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::setupField( int externalWidth, 
                                        int externalHeight,
                                        int fieldWidth, 
                                        int fieldHeight ) 
{
	
    if( fieldWidth == 0 || fieldHeight == 0 ) {
        fieldWidth  = externalWidth * DEFAULT_SCALE;
        fieldHeight = externalHeight * DEFAULT_SCALE;
    }
    
	_fieldWidth		= fieldWidth;
	_fieldHeight	= fieldHeight;
	_externalWidth	= externalWidth;
	_externalHeight = externalHeight;
	
	_field.clear();
	
	_fieldSize = _fieldWidth * _fieldHeight;

    for(int i=0; i<_fieldSize; ++i) {
        _field.push_back( ofVec2f(0,0) );
    }
    
    _horShiftPct = 0.0f;
    _scale = 1.0f;
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::zeroField()
{
    
	vector<ofVec2f>::iterator it;
	for(it = _field.begin(); it != _field.end(); ++it) {
		it->set(0, 0);
	}
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::fadeField(float fadeAmount)
{
    
	vector<ofVec2f>::iterator it;
	for(it = _field.begin(); it != _field.end(); ++it) {
		it->set(it->x * fadeAmount, it->y * fadeAmount);
	}
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::randomizeField(float range)
{
	vector<ofVec2f>::iterator it;
	for(it = _field.begin(); it != _field.end(); ++it) {
		// random between -1 and 1
		float x = (float)(ofRandom(-1,1)) * range;
		float y = (float)(ofRandom(-1,1)) * range;
		it->set(x,y);
	}
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::setHorizontalShift( float shiftPct ) {
    
    if( shiftPct > 1.0f ) {
        shiftPct -= floor(shiftPct);
    }
    _horShiftPct = shiftPct;
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::setScale( float scale ) {
    _scale = scale;
}

void ofxLabFlexVectorField::setExternalOffset( ofVec2f offset ) {
    _externalOffset = offset;
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::draw( const ofRectangle& cropSection )
{
    ofSetColor(0, 0, 0);
    
	float scaledX = _externalWidth / _fieldWidth;
	float scaledY = _externalHeight / _fieldHeight;
	
    int startXX;
    int startYY;
    int endXX;
    int endYY;
    
    if( cropSection.width == 0 || cropSection.height == 0 ) {
        
        startXX = _externalOffset.x / scaledX;
        startYY = _externalOffset.y / scaledY;
        
        endXX = _fieldWidth / scaledX;
        endYY = _fieldHeight / scaledY;

    } else {
        
        startXX = (cropSection.x + _externalOffset.x) / scaledX;
        startYY = (cropSection.y + _externalOffset.y) / scaledY;
        
        endXX = startXX + cropSection.width / scaledX;
        endYY = startYY + cropSection.height / scaledY;
        
    }
    
    startXX = MAX(0, startXX);
    startYY = MAX(0, startYY);
    
    endXX = MIN(_fieldWidth, endXX);
    endYY = MIN(_fieldHeight, endYY);
    
	// loop y to x to optimize reading of our vector in cache
	for(int yy=startYY; yy < endYY; ++yy) {
		for(int xx=startXX; xx < endXX; ++xx) {
			
            // shift index as needed
            int shiftXX;
            shiftXX = ((int) (xx + _horShiftPct * _fieldWidth )) % _fieldWidth;
			int vecPos = yy * (endXX - startXX) + shiftXX;
		
			// get our main force line
			ofPoint forceOrigin(xx * scaledX - _externalOffset.x,
                                yy * scaledY - _externalOffset.y);
			
			ofPoint forceEnd(forceOrigin);
            
            
            if( _bUseSinMap ) {
                
                float sinX = sin(xx / _sinXRepeat + _sinXPhase);
                float sinY = sin(yy / _sinYRepeat + _sinYPhase);
                
                
                // watch for the like of (-2 ^ .5)
                if( sinX >= 0 || _sinPowerValue >= 1 ) {
                    sinX = pow(sinX, _sinPowerValue);
                }
                
                if( sinY >= 0 || _sinPowerValue >= 1 ) {
                    sinY = pow(sinY, _sinPowerValue);
                }
                
                if( _bClampSinPositive ) {
                    sinX = MAX(0, sinX);
                    sinY = MAX(0, sinY);
                }
                
                forceEnd.x += _field[vecPos].x * FORCE_DISPLAY_SCALE * _scale * sinX;
                forceEnd.y += _field[vecPos].y * FORCE_DISPLAY_SCALE * _scale * sinY;
                
            } else {
                forceEnd.x += _field[vecPos].x * FORCE_DISPLAY_SCALE * _scale;
                forceEnd.y += _field[vecPos].y * FORCE_DISPLAY_SCALE * _scale;
            }
            
            ofEllipse(forceOrigin.x, forceOrigin.y, 2, 2);
			
			ofLine(forceOrigin.x, forceOrigin.y,
                   forceEnd.x, forceEnd.y);
			
			// draw peprendicular base line
			ofVec2f baseLine;
			baseLine.set(forceEnd.x - forceOrigin.x,
                         forceEnd.y - forceOrigin.y);
            
			float forceLength = baseLine.length();
			baseLine.normalize();
			baseLine.rotate(90);
			
			ofLine(forceOrigin.x - (baseLine.x * forceLength * BASELINE_SCALE),
                   forceOrigin.y - (baseLine.y * forceLength * BASELINE_SCALE),
                   forceOrigin.x + (baseLine.x * forceLength * BASELINE_SCALE),
                   forceOrigin.y + (baseLine.y * forceLength * BASELINE_SCALE));
			
		}
	}
}

//------------------------------------------------------------------------------------
ofVec2f ofxLabFlexVectorField::getForceFromPos( float posX,
                                                float posY)
const
{
	
	ofVec2f force(0,0);
    
    posX += _externalOffset.x;
    posY += _externalOffset.y;
	
	// convert posX and posY into percentage
	float percentX = posX / _externalWidth;
	float percentY = posY / _externalHeight;
	
	// range check
	if(percentX < 0 || percentX > 1 || percentY < 0 || percentY > 1) {
		return force;
	}
    
    // shift it as needed
    percentX += _horShiftPct;
    percentX -= int(percentX);
	
	int fieldPosX = (int)(percentX * _fieldWidth);
	int fieldPosY = (int)(percentY * _fieldHeight);
	
    // range check
	fieldPosX = MAX(0, MIN(fieldPosX, _fieldWidth-1));
	fieldPosY = MAX(0, MIN(fieldPosY, _fieldHeight-1));

	// pos in vector
	int vecPos = fieldPosY * _fieldWidth + fieldPosX;
	
    force.set(_field[vecPos].x,
              _field[vecPos].y);  // scale here as values are pretty large.
    
    if( _bUseSinMap ) {
        //force *= sin(posX * _sinXRepeat + _sinXPhase) * sin(posY * _sinYRepeat + _sinYPhase);
        
        float sinX = sin(fieldPosX / _sinXRepeat + _sinXPhase);
        float sinY = sin(fieldPosY / _sinYRepeat + _sinYPhase);
        
        
        // watch for the like of (-2 ^ .5)
        if( sinX >= 0 || _sinPowerValue >= 1 ) {
            sinX = pow(sinX, _sinPowerValue);
        }
        
        if( sinY >= 0 || _sinPowerValue >= 1 ) {
            sinY = pow(sinY, _sinPowerValue);
        }
        
        if( _bClampSinPositive ) {
            sinX = MAX(0, sinX);
            sinY = MAX(0, sinY);
        }
        
        force.x *= sinX;
        force.y *= sinY;
    }
	
    // scale it as needed
    force *= _scale;
    
	return force;
}


//------------------------------------------------------------------------------------
ofVec2f ofxLabFlexVectorField::getInternalSize()
{
    return ofVec2f(_fieldWidth,
                   _fieldHeight);
}


//------------------------------------------------------------------------------------
ofVec2f ofxLabFlexVectorField::getExternalSize()
{
    return ofVec2f(_externalWidth,
                   _externalHeight);
}


//------------------------------------------------------------------------------------
vector<ofVec2f>* ofxLabFlexVectorField::getField()
{
    return &_field;
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addForce( float x, 
                               float y,
							   ForceType type,
							   float radius, 
							   float strength)
{
	
    x += _externalOffset.x;
    y += _externalOffset.y;
    
	float percentX = x / _externalWidth;
	float percentY = y / _externalHeight;
	float radiusPercent = radius / _externalWidth; // kind of an arbitrary selection
	
    
    // range check
	if(percentX < 0 || percentX > 1 || percentY < 0 || percentY > 1) {
		return;
	}
    
    percentX += _horShiftPct;
    percentX -= int(percentX);
    
	int fieldPosX = (int)(percentX * _fieldWidth);
	int fieldPosY = (int)(percentY * _fieldHeight);
	float fieldRadius = radiusPercent * _fieldWidth;
	
	//cout << "adding force to [" << _fieldWidth << "] " << fieldPosX << ", [" << _fieldHeight << "] " << fieldPosY << endl;
	
	int startX	= MAX(fieldPosX - fieldRadius, 0);    
	int startY	= MAX(fieldPosY - fieldRadius, 0);
	int endX	= MIN(fieldPosX + fieldRadius, _fieldWidth);
	int endY	= MIN(fieldPosY + fieldRadius, _fieldHeight);
	
	
	// loop yy then xx to optimize read in cache
	for(int yy = startY; yy < endY; ++yy) {
		for(int xx = startX; xx < endX; ++xx) {
			int vecPos = yy * _fieldWidth + xx;
			float distance = sqrt(static_cast<double> ((fieldPosX - xx) * (fieldPosX - xx) +
													  (fieldPosY - yy) * (fieldPosY - yy)));
	
			// 0 distance is problematic
			if(distance < 0.0001f) { distance = 0.0001f; }
			
			if(distance < fieldRadius) {
				float percent = 1 - (distance / fieldRadius);
				float scaledStrength = strength * percent;
				ofVec2f unitDirection((fieldPosX - xx),
															 (fieldPosY - yy));
				unitDirection.normalize();
				
				switch(type) {
					default:
					case OUT_CIRCLE:
						_field[vecPos].x -= unitDirection.x * scaledStrength;
						_field[vecPos].y -= unitDirection.y * scaledStrength;
						break;
						
					case IN_CIRCLE:
						_field[vecPos].x += unitDirection.x * scaledStrength;
						_field[vecPos].y += unitDirection.y * scaledStrength;	
						break;
						
					case CLOCK_CIRCLE:
						// noticed flipped x, y
						_field[vecPos].x += unitDirection.y * scaledStrength;
						_field[vecPos].y -= unitDirection.x * scaledStrength;
						break;
						
					case COUNTER_CLOCK_CIRCLE:
						// noticed flipped x, y
						_field[vecPos].x -= unitDirection.y * scaledStrength;
						_field[vecPos].y += unitDirection.x * scaledStrength;
						break;
				}
				
			}
		}
	}
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addInwardCircle( float x, 
                                      float y, 
                                      float radius, 
                                      float strength)
{
	addForce(x, y, IN_CIRCLE, radius, strength);
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addOutwardCircle( float x, 
                                       float y, 
                                       float radius, 
                                       float strength)
{
	addForce(x, y, OUT_CIRCLE, radius, strength);
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addClockwiseCircle( float x, 
                                         float y, 
                                         float radius, 
                                         float strength)
{
	addForce(x, y, CLOCK_CIRCLE, radius, strength);
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addCounterClockwiseCircle( float x, 
                                                float y, 
                                                float radius, 
                                                float strength)
{
	addForce(x, y, COUNTER_CLOCK_CIRCLE, radius, strength);
}


//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::setUniformForce( const ofRectangle& area,
                                      const ofVec2f& force)
{
    
    float startX = area.x / _externalWidth * _fieldWidth;
    float endX   = (area.x + area.width) / _externalWidth * _fieldWidth;
    
    float startY = area.y / _externalHeight * _fieldHeight;
    float endY   = (area.y + area.height) / _externalHeight * _fieldHeight;
    
    // cap the range
    startX = MAX( 0, startX );
    endX = MIN( _fieldWidth, endX );
    
    startY = MAX( 0, startY );
    endY = MIN( _fieldHeight, endY );


    for( int yy = startY; yy < endY; ++yy ) {
        for( int xx = startX; xx < endX; ++xx ) {
            int index = yy * _fieldWidth + xx;
            
            _field[index] = force;
        }
    }
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::applySinMap( float xRepeat,
                                  float yRepeat,
                                  float xPhase,
                                  float yPhase,
                                  float sinPowerValue,
                                  bool positiveClamp)
{
    _bUseSinMap = true;
    
    _sinXRepeat = _fieldWidth / TWO_PI / xRepeat;
    _sinYRepeat = _fieldHeight / TWO_PI / yRepeat;
    
    _sinXPhase = xPhase;
    _sinYPhase = yPhase;
    
    _bClampSinPositive = positiveClamp;
    
    _sinPowerValue = sinPowerValue;
}

//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::clearSinMap()
{
    _bUseSinMap = false;
}

/*
//------------------------------------------------------------------------------------
void ofxLabFlexVectorField::addVectorCircle(float x, float y, float vx, float vy, float radius, float strength){
	// LV I am not sure what to make of this....
	
	
	// x y and radius are in external dimensions.  Let's put them into internal dimensions:
	// first convert to pct:
	
	float pctx			= x / (float)externalWidth;
	float pcty			= y / (float)externalHeight;
	float radiusPct		= radius / (float)externalWidth;   
	
	// then, use them here: 
	int fieldPosX		= (int)(pctx * (float)fieldWidth);
	int fieldPosY		= (int)(pcty * (float)fieldHeight);
	float fieldRadius	= (float)(radiusPct * fieldWidth);
	
	// we used to do this search through every pixel, ie: 
	//    for (int i = 0; i < fieldWidth; i++){
	//    for (int j = 0; j < fieldHeight; j++){
	// but we can be smarter :)
	// now, as we search, we can reduce the "pixels" we look at by 
	// using the x y +/- radius.
	// use min and max to make sure we don't look over the edges 
	
	int startx	= MAX(fieldPosX - fieldRadius, 0);    
	int starty	= MAX(fieldPosY - fieldRadius, 0);
	int endx	= MIN(fieldPosX + fieldRadius, fieldWidth);
	int endy	= MIN(fieldPosY + fieldRadius, fieldHeight);
	
	
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
*/

