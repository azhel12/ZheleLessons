#define F_CPU 8000000

#include <delay.h>
#include <iopins.h>
#include <pinlist.h>

using namespace Zhele::IO;

using LedPort = PinList<Pb10, Pa6, Pa2, Pc15>;

int main()
{
    // Включение тактирования виртуального порта.
    LedPort::Enable();

    // Конфигурирование порта
    LedPort::SetConfiguration<LedPort::Configuration::Out>();
    LedPort::SetDriverType<LedPort::DriverType::PushPull>();
    LedPort::SetSpeed<LedPort::Speed::Slow>();

    // 4 диода = 16 комбинаций 
    for(int i = 0; i < 16; ++i)
    {
        // Запись значения в порт
        LedPort::Write(i);
        // Пауза
        Zhele::delay_ms<1000>();
    }

    for(;;)
    {
    }
}