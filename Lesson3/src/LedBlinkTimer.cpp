#include <iopins.h>
#include <timer.h>

using namespace Zhele::IO;
using namespace Zhele::Timers;

using Led = Pc13Inv;
using LedTimer = Timer2;


int main()
{
    Led::Port::Enable();
    Led::SetConfiguration(Led::Configuration::Out);
    Led::SetDriverType(Led::DriverType::PushPull);
    Led::Clear();

    // Включение тактирования
    LedTimer::Enable();
    // Установка предделителя
    LedTimer::SetPrescaler(3999);
    // Установка периода. Если частота CPU = 8Мгц, то таймер будет переполняться каждую секунду (8Мгц/4000/2000).
    LedTimer::SetPeriod(1999);
    // Разрешение прерывания
    LedTimer::EnableInterrupt();
    // Запуск таймера
    LedTimer::Start();

    for (;;)
    {
    }
}

extern "C"
{
    void TIM2_IRQHandler()
    {
        // Изменение состояния светодиода
        Led::Toggle();
        // Сброс флага прерывания
        LedTimer::ClearInterruptFlag();
    }
}