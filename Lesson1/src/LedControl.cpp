#include <iopins.h>

using Led = Zhele::IO::Pc13Inv;

int main()
{
    // Включение тактирования порта
    Led::Port::Enable();
    // Конфигурация - выход
    Led::SetConfiguration(Led::Configuration::Out);
    // Режим -  Push-pull
    Led::SetDriverType(Led::DriverType::PushPull);
    // Скорость - низкая
    Led::SetSpeed(Led::Speed::Slow);
    // Запись 1 (на самом деле 0, так как класс инвертированный)
    Led::Set();

    for(;;)
    {
    }
}