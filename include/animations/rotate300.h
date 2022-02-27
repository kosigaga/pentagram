#include <FastLED.h>
#include <vector.h>

#include "effects.h"
#include "base.h"

template <class T>
using ValueChanger = std::function<T(T, int64_t)>;

template <class T>
ValueChanger<T> createLinearValueChanger(T valStart, T valEnd, int64_t duration)
{
    return [valStart, valEnd, duration](T currentValue, int64_t deltaT)
    {
        T valueChangeRange = (valEnd - valStart);

        double progress = (double)deltaT / (double)duration;
        T step = valueChangeRange * (double)progress;
        return valStart + step;
    };
}

template <class T>
ValueChanger<T> createSinusValueChanger(T valStart, T valEnd, int64_t duration)
{
    return [valStart, valEnd, duration](T currentValue, int64_t deltaT)
    {
        T valueChangeRange = (valEnd - valStart);
        double progress = (double)deltaT / (double)duration;

        T step = valueChangeRange * sin((double)progress * (M_PI));
        return valStart + step;
    };
}

template <class T>
ValueChanger<T> createBezierValueChanger(T valStart, T valEnd, int64_t duration)
{
    return [valStart, valEnd, duration](T currentValue, int64_t deltaT)
    {
        T valueChangeRange = (valEnd - valStart);
        double progress = (double)deltaT / (double)duration;

        T step = valueChangeRange * (progress * progress * (3.0f - 2.0f * progress));
        return valStart + step;
    };
}

std::vector<BaseEffect *> LightAndHold(std::vector<SData> symbols, int startTime, int endTime)
{
    std::vector<BaseEffect *> effects;

    // switch (holdType)
    // {
    // case 0:
    //     break;
    // case 1:
    //     break;
    // default:
    //     break;
    // }

    for (SData s : symbols)
    {
        int maxWeight = s.weight;
        int baseWeight = maxWeight / 4;
        int minWeight = 0;

        CHSV color = s.c;

        int64_t duration = endTime - startTime;

        double sDeg = SymbolToDeg(s.symbol);

        ValueChanger<int> weightLambda = createBezierValueChanger(maxWeight, minWeight, duration);
        BaseEffect *effect = new BaseEffect(color, nullptr, sDeg, nullptr, SYMBOL_LED_WIDTH, nullptr, maxWeight, weightLambda, startTime, endTime);
        effects.push_back(effect);
    }

    return effects;
}

std::vector<BaseEffect *> Trickle(std::vector<std::pair<SData, SData>> symbols, int startTime, int endTime)
{
    std::vector<BaseEffect *> effects;

    for (std::pair<SData, SData> s : symbols)
    {
        SData fromData = s.first;
        SData toData = s.second;
        int symbolFrom = fromData.symbol;
        int symbolTo = toData.symbol;

        int64_t duration = endTime - startTime;

        int delayTime = 1 * SEC_TO_MICRO; // duration/something maybe

        int maxWeight = s.first.weight;
        int minWeight = 0;

        CHSV color = s.first.c;
        int deg = 90;
        int width = 50; // degree

        double sDeg = SymbolToDeg(symbolFrom);
        ValueChanger<int> weightLambda = createLinearValueChanger(maxWeight, minWeight, duration - delayTime);
        BaseEffect *effect = new BaseEffect(color, nullptr, sDeg, nullptr, SYMBOL_LED_WIDTH, nullptr, maxWeight, weightLambda, startTime, endTime - delayTime);
        effects.push_back(effect);

        double sDeg2 = SymbolToDeg(symbolTo);
        ValueChanger<int> weightLambda2 = createLinearValueChanger(minWeight, maxWeight, duration - delayTime);
        BaseEffect *effect2 = new BaseEffect(color, nullptr, sDeg2, nullptr, SYMBOL_LED_WIDTH, nullptr, minWeight, weightLambda2, startTime + delayTime, endTime);
        effects.push_back(effect2);
        Serial.print("Trk HALF ");
        int rnd = 5 + rand() % 20;

        int trickleWeight = (maxWeight / rnd) + 64;
        int trickleWidth = 2;

        for (size_t i = 0; i < rnd; i++)
        {
            int dir = rand() % 2;
            int timeRange = (endTime / SEC_TO_MICRO) - 1;

            int start = (rand() % (timeRange * 5)) * SEC_TO_MICRO / 5;

            int stop = endTime - ((rand() % ((timeRange - start / SEC_TO_MICRO) * 5)) * SEC_TO_MICRO / 5);
            int duration = stop - start;

            if (dir == 0)
            {
                ValueChanger<double> degChanger = createBezierValueChanger(sDeg, sDeg2, duration);
                BaseEffect *tEffect = new BaseEffect(color, nullptr, sDeg, degChanger, trickleWidth, nullptr, trickleWeight, nullptr, start, stop);
                effects.push_back(tEffect);
            }
            else
            {

                ValueChanger<double> degChanger = createLinearValueChanger(sDeg, sDeg2 - NUM_DEG, duration);
                BaseEffect *tEffect = new BaseEffect(color, nullptr, sDeg, degChanger, trickleWidth, nullptr, trickleWeight, nullptr, start, stop);
                effects.push_back(tEffect);
            }
        }
    }
    // Serial.print("   ++++++++ AC FREEHEAP:");
    // Serial.print(ESP.getFreeHeap());
    // Serial.print("\n");

    // Serial.print("Trk Bup \n ");
    return effects;
}

std::vector<BaseEffect *> createRotate300()
{
    std::vector<BaseEffect *> effects;

    CHSV color = CHSV(190, 255, 255);
    double deg = 90;
    double width = 50; // degree
    int strength = 128;

    BaseEffect *baseColorEffect = new BaseEffect(CHSV(180, 128, 96), nullptr, 0, nullptr, 360, nullptr, 0, nullptr);
    effects.push_back(baseColorEffect);

    int64_t startTime = 1 * SEC_TO_MICRO;
    int64_t endTime = 5 * SEC_TO_MICRO;
    int64_t duration = endTime - startTime;

    BaseEffect *effect = new BaseEffect(
        color,
        nullptr,
        60,
        [duration](double D, int64_t deltaT)
        {
            double progress = (double)deltaT / (double)duration;
            double step = -300.0 * (double)progress;
            return Clamp(step + 60);
        },
        width,
        [duration](double D, int64_t deltaT)
        {
            double progress = (double)deltaT / (double)duration;
            double step = -300.0 * (double)progress;
            return Clamp(step + 60);
        },
        strength,
        nullptr,
        startTime,
        endTime);

    effects.push_back(effect);

    return effects;
}

std::vector<BaseEffect *> createEgymassalSzembeForog()
{
    std::vector<BaseEffect *> effects;

    CHSV color = CHSV(190, 150, 150);
    BaseEffect *effect = new BaseEffect(
        color, nullptr, 0, [](double D, int64_t deltaT)
        { return Clamp(D + 1); },
        5, nullptr, 255, nullptr, 5 * SEC_TO_MICRO, 25 * SEC_TO_MICRO);

    effects.push_back(effect);

    BaseEffect *effect2 = new BaseEffect(
        color, nullptr, Clamp(180.0), [](double D, int64_t deltaT)
        { return Clamp(D + 1); },
        5, nullptr, 255, nullptr, 5 * SEC_TO_MICRO, 25 * SEC_TO_MICRO);

    effects.push_back(effect2);

    return effects;
}
