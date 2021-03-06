# Урок 2. Таймер.

## Вступление
> Прошу прощения у читателей (если такие ещё есть) за долгий перерыв как в работе над самой библиотекой,
> так и над уроками, сначала отдыхал в отпуске, потом, как это обычно бывает, навалились дела.

На данном уроке будет начато рассмотрение такого важного и часто используемого элемента
любого микроконтроллера, как **Таймер**.

В контроллерах Stm32 выделяют 3 вида таймеров:
1. **Basic timers** (базовые таймеры). Преоставляют следующий функционал:
   1. 16-битный счётчик с автоперезагрузкой.
   2. 16-битный программируемый делитель частоты: с 1 по 65535.
   3. Схема синхронизации для запуска *ЦАП*.
   4. Генерация прерывания и/или запроса *DMA* по переполнению.
2. **General-purpose timers** (таймеры общего назначения). Предоставляют те же функции, что и базовые таймеры, а также дополнительные возможности (будут рассмотрены в следуюющих уроках).
3. **Advanced timers** (продвинутые таймеры). Самые функциональные таймеры, хотя лично я до сих пор не до конца понял, в чем отличия от GP-таймеров.

Библиотека **Zhele** содержит набор классов, реализующих таймеры. Они объявлены в заголовочном файле *<timer.h>*

Для класса базового таймера (и, соответственно, для всех других тоже)
доступны следующие методы:

1. `unsigned GetClockFreq();` - возвращает частоту шины таймера.
2. `void Enable();` - включает тактирование таймера.
3. `void Disable();` - отключает тактирование таймера.
4. `void SetCounterValue(Counter counter);` - устанавливает значение счетчика таймера.
5. `Counter GetCounterValue();` - возвращает текущее значение счетчика таймера.
6. `void ResetCounterValue();` - сбрасывает значение счетчика до 0.
7. `void SetPrescaler(Prescaler prescaler);` - устанавливает предделитель таймера.
8. `Counter GetPrescaler();` - возвращает текущий предделитель таймера.
9. `void SetPeriod(Counter period);` - устанавливает период (значение перезагрузки) таймера.
10. `void SetPeriodAndUpdate(Counter value);` - устанавливает период с немедленным обновлением и сбросом счетчика до 0.
11. `Counter GetPeriod();` - возвращает текущее значение периода.
12. `void Stop();` - останавливает таймер.
13. `void Start();` - запускает таймер.
14. `void EnableInterrupt();` - разрешает прерывание от таймера.
15. `void DisableInterrupt();` - запрещает прерывание от таймера.
16. `bool IsInterrupt();` - возвращает true, если в регистре флагов установлен флаг прерывания по переполнению.
17. `void ClearInterruptFlag();` - сбрасывает флаг прерывания.
18. `void DmaRequestEnable();` - разрешает запрос DMA по переполнению.
19. `void DmaRequestDisable();` - запрещает запрос DMA по переполнению.

На текущем уроке предлагается осуществить управление светодиодом с применением таймера, а именно:
1. Настроить таймер для прерывания каждую секунду.
2. В обработчике прерывания изменять состояние светодиода.

Проще говоря, получившаяся программа - это "моргалка" светодиодом, однако не на задержках, а кошерно, на таймере.

## Пример

Разрабатываемый код предназачен для популярной платы BluePill с контроллером *Stm32F103c8t6*, которая содержит светодиод на выходе **C13**, поэтому сконфигурируем его на выход.

Сначала зададим alias, чтобы в случае необходимости было легко перенести диод на другую ногу:
`using Led = Pc13Inv;`

В функции **main** осуществим некоторые настройки (см. Урок 0.):
```c++
Led::Port::Enable();
Led::SetConfiguration(Led::Configuration::Out);
Led::SetDriverType(Led::DriverType::PushPull);
Led::Clear();
```

Далее настроим таймер. Аналогично объвяим alias, чтобы в случае необходимости было проще изменить на использование другого таймера:
`using LedTimer = Timer2;`

В функции **main** необходимо произвести начальные настройки, которые зависят от частоты шины, от которой тактируется таймер и от необходимого периода прерываний. В нашем случае частота системной шины, от которой тактируется таймер 2, составляет 8 Мгц, соответственно для получения периода в 1 секунду необходимо разделить ее на 8.000.000 путем установки значений предделителя и регистра перезагрузки. Можно предложить несколько вариантов, например такой:
1. Значение предделителя 4000 (точнее 3999, так как значение регистра предделителя, равное 0, на самом деле задает делитель, равный 1).
2. Значение перезагрузки 2000 (аналогично в регистр надо записать 1999).

```c++
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
```

После вызова последней функции таймер начнет отсчет с генерацией прерывания, которое нужно обработать. К сожалению, на данный момент не удалось обеспечить удобную (= написанную в общем стиле, на шаблонах) обработку прерываний, поэтому придется определить обработчик вручную:
```c++
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
```
В общем-то, программа написана, осталось только прошить микроконтроллер и наблюдать за поведением светодиода.

Полный код приведен ниже:

```c++
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
    // Установка периода. Если частота CPU = 8Мгц, то таймер будет переполняться каждую секунду (8МГц/4000/2000).
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
```