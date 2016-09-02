#ifndef RATIO_H
#define RATIO_H

class RatioSingleton
{
        float _ratio;
        static RatioSingleton * _instance;
    public:
        RatioSingleton();
        void SetRatio(float ratio);
        float GetRatio();
        static RatioSingleton * GetInstance();

};

//RatioSingleton * RatioSingleton::_instance = 0;

#endif
