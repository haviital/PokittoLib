#pragma once

#include "main.h"

class CAnimValueCallback
{
public:

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    virtual void Finished( int32_t par ) = 0;
};

class CAnimValue
{
public:
    CAnimValue();

    void Reset();

    void Start( uint32_t durationInMs,  fix16_t fxStartValue, fix16_t fxEndValue, CAnimValueCallback* cb = NULL, int32_t callbackParam = 0 );

    void Run();

    static CAnimValue* GetFreeElement();

    static void RunAll();

public:
    bool m_isActive;
    uint32_t m_startTimeInMs;
    uint32_t m_endTimeInMs;
    fix16_t m_fxStartValue;
    fix16_t m_fxEndValue;
    fix16_t m_fxValue; // Current value
    CAnimValueCallback* m_pfinishedCB;
    int32_t m_finishedCBParam;
};

class CAnimBitmap : public CAnimValueCallback
{
public:

    CAnimBitmap();

    void Start( uint32_t durationInMs, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t* bitmap, CAnimValueCallback* cb = NULL, int32_t callbackParam = 0 );

    void Draw();

    // When this is called the CAnimValue is finished and will be used for other tasks. The caller should either
    // set a pointer to the CAnimValue object to NULL, or start another animation on the object.
    // The parameter can be used e.g. for chained animations to indicate the state.
    void Finished( int32_t par );

public:
    uint8_t* m_bitmap;
    CAnimValue* m_animValue;
    int32_t m_x1;
    int32_t m_y1;
    int32_t m_x2;
    int32_t m_y2;
    CAnimValueCallback* m_pfinishedCB;
};

