#include "Pokitto.h"
#include "PokittoCookie.h"
#include "fix16.h"
#include "ship.h"

CAnimValue::CAnimValue()
{
    Reset();
}

void CAnimValue::Reset()
{
    m_isActive = false;
    m_startTimeInMs = 0;
    m_endTimeInMs = 0;
    m_fxStartValue = 0;
    m_fxEndValue = 0;
    m_fxValue = 0;
    m_pfinishedCB = NULL;
    m_finishedCBParam = 0;
}

void CAnimValue::Start( uint32_t durationInMs,  fix16_t fxStartValue, fix16_t fxEndValue, CAnimValueCallback* cb, int32_t callbackParam)
{
    m_isActive = true;
    m_startTimeInMs = g_currentFrameTimeInMs;
    m_endTimeInMs = m_startTimeInMs + durationInMs;
    m_fxStartValue = fxStartValue;
    m_fxEndValue = fxEndValue;
    m_fxValue = fxStartValue;
    m_pfinishedCB = cb;
    m_finishedCBParam  = callbackParam;
}

void CAnimValue::Run()
{
    if( g_currentFrameTimeInMs > m_endTimeInMs )
    {
        m_isActive = false;
        if( m_pfinishedCB )
            m_pfinishedCB->Finished( m_finishedCBParam );

        // Note: the m_isActive can be set to true in the Finished() callback if a new animation is started.
        if( ! m_isActive )
            Reset();
    }
    else
    {
        uint32_t deltaTimeInMs = m_endTimeInMs - m_startTimeInMs;
        uint32_t currentDeltaTimeInMs = g_currentFrameTimeInMs - m_startTimeInMs;
        fix16_t fxFactor = 0;
        if( deltaTimeInMs != 0 )
            fxFactor = fix16_div( currentDeltaTimeInMs, deltaTimeInMs );
        fix16_t fxDeltaValue = m_fxEndValue - m_fxStartValue;
        m_fxValue = m_fxStartValue + fix16_mul( fxFactor, fxDeltaValue );
    }
}

CAnimValue* CAnimValue::GetFreeElement()
{
    for( int32_t i=0; i<g_animValueArrayCount; i++)
        if( g_animValueArray[i].m_isActive == false )
            return &( g_animValueArray[ i ] );
    return NULL;
}

void CAnimValue::RunAll()
{
    for( int32_t i=0; i<g_animValueArrayCount; i++)
        if( g_animValueArray[i].m_isActive == true )
            g_animValueArray[ i ].Run();
}

CAnimBitmap::CAnimBitmap() :
    m_bitmap( NULL ),
    m_animValue( NULL ),
    m_x1( 0 ),
    m_y1( 0 ),
    m_x2( 0 ),
    m_y2( 0 ),
    m_pfinishedCB( NULL )
{}

void CAnimBitmap::Start( uint32_t durationInMs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t* bitmap, CAnimValueCallback* cb, int32_t callbackParam )
{

    m_pfinishedCB = cb;

    if(m_animValue)
        m_animValue->Reset();
    else
        m_animValue = CAnimValue::GetFreeElement();

    if( m_animValue )
        m_animValue->Start( durationInMs,  fix16_from_int( 0 ), fix16_from_int( 1 ), this, callbackParam );
    m_bitmap = bitmap;
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
}

void CAnimBitmap::Stop()
{
    if( m_animValue )
        m_animValue->Reset();
    m_animValue = NULL; // Must be done!
}

void CAnimBitmap::Draw()
{
    if( m_animValue )
    {
        fix16_t fxFactor = m_animValue->m_fxValue;
        int32_t relX = fix16_to_int( fxFactor * (m_x2 - m_x1) );
        int32_t currX = m_x1 + relX;

        int32_t relY = fix16_to_int( fxFactor * (m_y2 - m_y1) );
        int32_t currY = m_y1 + relY;

        // Draw
        DrawBitmap8bit( currX, currY, &(m_bitmap[2]), m_bitmap[0], m_bitmap[1] );
    }
}

// When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
// set a pointer to the CAnimValue object to NULL, or start another animation on the object.
// The parameter can be used e.g. for chained animations to indicate the state.
void CAnimBitmap::Finished( int32_t par )
{
    if( m_pfinishedCB )
        m_pfinishedCB->Finished( par );

    if( m_animValue && ! m_animValue->m_isActive )
        m_animValue = NULL; // Must be done!
}

