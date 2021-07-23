#include <iopins.h>
#include <pinlist.h>

using namespace Zhele::IO;

using LedPort = PinList<Pa0, Pa1, Pb1, Pc15>;

int main()
{
    // Включение тактирования виртуального порта.
    LedPort::Enable();

    // Конфигурирование порта
    LedPort::SetConfiguration<LedPort::Configuration::Out>();
    LedPort::SetDriverType<LedPort::DriverType::PushPull>();
    LedPort::SetSpeed<LedPort::Speed::Slow>();

    // Запись значения в порт
    LedPort::Write(0b101);

    for(;;)
    {
    }
}