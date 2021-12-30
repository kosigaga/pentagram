#pragma once
#include <FastLED.h>

#include "base.h"
#include "debug.h"

struct CData
{
    CData()
    {
        rWeight = 0;
        c = CHSV(0, 0, 0);
    }
    CData(int rw, CHSV col)
    {
        rWeight = rw;
        c = CHSV(col);
    }
    int rWeight = 0; /*render weight*/
    CHSV c;          /* _data */
};

class Effect
{
protected:
    CHSV _c;
    unsigned long _start_time;
    unsigned long _last_time; // last run time to calculate effect amount

    void ResetData()
    {
        for (size_t i = 0; i < NUM_DEG; i++)
        {
            _data[i].rWeight = 0;
            _data[i].c = _c;
        }
    }

    void UpdatePart(CHSV c, double deg, double width, double ww = 0)
    {
        if (ww == 0)
        {
            ww = _rWeight;
        }; // 0 nem lesz jó

        for (int i = deg - width / 2; i < deg + width / 2; i++)
        {
            if (i < 0)
            {
                _data[360 + i] = CData(ww, c);
            }

            if (i >= 0 && i < 360)
            {
                _data[i] = CData(ww, c);
            }

            if (i >= 360)
            {
                _data[i - 360] = CData(ww, c);
            }
        }
    }

public:
    CData _data[NUM_DEG];
    int _rWeight;

    virtual void DoStuff()
    {
        _last_time = millis();
    }

    virtual void SetColor(CHSV c)
    {
        _c = c;
    }

    explicit Effect(CHSV c, int rw = 0)
    {
        _start_time = millis();
        _c = c;
        _rWeight = rw;
        _last_time = millis();

        for (size_t i = 0; i < NUM_DEG; i++)
        {
            _data[i] = CData(_rWeight, _c);
        }
    }
};

class SymbolSimpleFade : public Effect
{
protected:
    double _deg;
    double _width;
    long _duration;

public:
    void DoStuff() override
    {
        if (millis() < _duration + _start_time)
        {
            // DLED::Blink(3, 10);
            long deltaW = (_rWeight / _duration) * (millis() - _start_time);

            // auto c = CHSV(_c.h + deltaC, _c.s + deltaC, _c.v + deltaC);

            ResetData();

            UpdatePart(_c, _deg, _width, deltaW);
            _last_time = millis();
        }
    }

    /// should be symbol numba
    SymbolSimpleFade(CHSV c, double deg, double width, long dur, int rw = 128) : Effect(c, 0)
    {
        _c = c;
        _deg = deg;
        _width = width;
        _duration = dur;
        _rWeight = rw;
        _last_time = millis();
        _start_time = millis();
    }
};

class DeepLightEffect : public Effect
{
public:
    void DoStuff() override
    {
        _last_time = millis();
    }

    DeepLightEffect(CHSV c, int deg = 30, int rw = 128) : Effect(c, 0)
    {
        _rWeight = rw;
        _last_time = millis();

        ResetData();

        for (int i = 0; i < NUM_DEG; i += 30)
        {
            _data[i] = CData(_rWeight, c);
        }
    }
};

class LightSection : public Effect
{
protected:
    double _deg;
    double _width;
    bool _dir;
    double _speed;

public:
    void DoStuff() override
    {
        _last_time = millis();
        if (_dir)
        {
            _deg += _speed;
        }
        else
        {
            _deg -= _speed;
        }

        if (_deg >= 360)
        {
            _deg = 0;
        }

        if (_deg < 0)
        {
            _deg = 360;
        }

        ResetData();

        UpdatePart(_c, _deg, _width);
    }

    LightSection(double deg, double w, double s, bool forward, CHSV c, int rw = 128) : Effect(c, 0)
    {
        _c = c;
        _width = w;
        _deg = deg;
        _speed = s;
        _dir = forward;
        _rWeight = rw;
    }
};

// class Wobble : public Effect
// {
// protected:
//     double _w_freq; //w/second
//     double _w_amount;
//     int _max_wobble;
//     int _med_wobble;
//     float _current_wobble;
//     bool rise = true;

//     double getWobbleAmount()
//     {
//         Blink(OP0, 10);
//         auto w = (256 / _w_amount) * (1000 / ((1000 / _w_freq) / (millis() - _last_time)));
//         int derp = w;
//         //Serial.write(derp);
//         Serial.println(derp, DEC);
//         return w;
//     }

//     int Cap255(int n)
//     {
//         if (n < 0 || n > 255)
//             return 128;
//         return n;
//     }

// public:
//     void DoStuff(int time)
//     {

//         // if (millis() - _last_time > (1000 / _w_freq))
//         // {

//         Blink(OP3, 10);
//         if (rise)
//         {
//             Blink(OP1, 10);
//             if (_current_wobble + getWobbleAmount() >= _med_wobble + _max_wobble)
//             {

//                 rise = false;
//             }

//             _current_wobble += getWobbleAmount();
//         }
//         else
//         {
//             Blink(OP2, 10);
//             if (_current_wobble - getWobbleAmount() <= _med_wobble - _max_wobble)
//             {

//                 rise = true;
//             }
//             _current_wobble -= getWobbleAmount();
//         }

//         for (int i = 0; i < NUM_DEG; ++i)
//         {
//             //_data[i].fadeLightBy(5);
//             //_data[i] = CRGB::Green;
//             _data[i].fadeLightBy(Cap255(_current_wobble));
//         }
//         _last_time = millis();
//     }

//     explicit Wobble(CRGB c, double wtime = 1, double wamount = 1, int wmax = 50, int wmed = 128) : Effect(c)
//     {
//         _last_time = millis();
//         _current_wobble = _med_wobble;
//     }
// };