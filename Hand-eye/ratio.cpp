#include "ratio.h"
        RatioSingleton::RatioSingleton()
        {
            RatioSingleton::SetRatio(0.0f);
        }
        void RatioSingleton::SetRatio(float ratio)
        {
            _ratio = ratio;
        }
        float RatioSingleton::GetRatio()
        {
            return _ratio;
        }
        RatioSingleton * RatioSingleton::GetInstance()
        {
            if(!_instance)
            {
                _instance = new RatioSingleton;
                return _instance;
            }
            return _instance;
        }
RatioSingleton * RatioSingleton::_instance = 0;
