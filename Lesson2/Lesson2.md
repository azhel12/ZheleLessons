# Урок 1. Виртуальные порты ввода-вывода.

## Вступление
На данном уроке предлагаю рассмотреть одно из главных преимуществ применения шаблонов языка C++ при
программировании микроконтроллеров: удобство создания виртуальных портов.

Выводы микроконтроллера есть константа, определяющая возможные вариации печатной платы.
Разработчики устройств нередко вынуждены искать компромисс между сложностью разводки платы и сложностью
написания программы. Последняя заключается в необходимости реализации виртуального порта, собранного
из различных линии различных портов.

Представим ситуацию, когда необходимо получить N-битный порт, но использовать целый
физический порт (или его часть) невозможно (часть линий занята периферией или крайней неудобно разводить
печатную плату). Например, пусть в нашем случае N = 4 и при разработке печатной платы наиболее удобно
под задачу выделить линии **A0**, **A1**, **B1**, **C15**.

Пин ВП | Реальный пин
:---:|:---:
0 | Pa0
1 | Pa1
2 | Pb1
3 | Pc15

Библиотека содержит специальный шаблонный класс `PinList`, позволяющий легко объявлять и использовать такие
виртуальные порты. Очень подробно механизм работы описан в оригинальной статье Константина Чижова 
[Работа с портами ввода-вывода микроконтроллеров на Си++](http://easyelectronics.ru/rabota-s-portami-vvoda-vyvoda-mikrokontrollerov-na-si.html) (снова очень рекомендую с ней ознакомиться,
чтобы понимать происходящее). Виртуальный порт предоставляет те же методы, что и обычный, инкапсулируя
распространение воздействия на каждый реальный порт.

## Пример

Обозначим примитивную задачу управления четырями светодиодами как единым портом. Будем считать, что
печатная плата разработана таким образом, что светодиоды расположены на линиях **Pa0, Pa1, Pb1, Pc15**.

Объявить виртуальный порт можно следующим образом:
```c++
#include <iopins.h>
#include <pinlist.h>

using namespace Zhele::IO;

using LedPort = PinList<Pa0, Pa1, Pb1, Pc15>;
```

Собственно, на этом сложность и ограничена! С типом `LedPort` можно работать как с обычным портом.

Чтобы включить тактирование виртуального порта, необходимо вызвать метод `Enable`:
```c++
LedPort::Enable();
```
Реализация этого метода вызовет метод `Enable` для каждого реального используемого порта. Поскольку
определение используемых портов ввода-вывода осуществляется еще на этапе компиляции, причем в этом
списке уничтожаются дубликаты, то в Runtime для каждого реального порта ввода-вывода метод влючения
тактирования будет вызвае единожды. Достичь такого при помощи макросов **невозможно**.

Чтобы сконфигурировать порт для задачи управления светодиодами,
необходимо задать конфигурацию (Out) режим (PushPull), скорость (Slow, хотя необязательно):
```c++
LedPort::SetConfiguration<LedPort::Configuration::Out>();
LedPort::SetDriverType<LedPort::DriverType::PushPull>();
LedPort::SetSpeed<LedPort::Speed::Slow>();
```

Как можно заметить из примера, методы вызываются необычным способом: параметры передаются в виде аргументов
шаблона. Библиотека имеет задублированные варианты подобных методов. Как правило, режим работы конкретного
порта (или линии) ввода-вывода известен на момент компиляции, поэтому предпочтительным является
вызов именно шаблонных вариантов, в таком случае все значения будут вычислены еще на этапе компиляции,
а в Runtime останется просто записать значение в регистр.

> Кому интересно, то можно дизассемблировать один из вышеуказанных методов. Внутри окажется только
> записаь значений в используемые порты.

> Стремление задублировать подобным образом вообще все, что возможно, пока не реализовано до конца.
> 
> Вообще говоря, следует исследовать работу оптимизатора современных компиляторов с `constexpr`-методами.
> Допускаю, что дублирование можно опустить, если пометить все эти методы как `constexpr`, тогда вызов
> их с аргументами, известными на момент компиляции, может быть вполне оптимизирован автоматически,
> снимая необходимость писать дополнительный код.

Запись значений в порт приведет к установлению на линиях **Pa0, Pa1, Pb1, Pc15** логических 0 и 1
в соответствии со значением. Например, чтобы зажечь нулевой и второй светодиоды
(то есть установить логическую 1 на линии **Pa0, Pb1**), необходимо записать в порт число `0b101 = 5`:
```c++
LedPort::Write(0b101);
```

Осталось лишь скомпилировать программу, залить прошивку на контроллер и убедиться, что все работает
именно так, как задумано.

На это описание виртуальных портов подошло к концу. Как можно заметить, нет абсолютно ничего сложного.
Код получается понятный, поддерживаемый и достаточно оптимальный (если будет необходимость, то готов
дизассмблировать прошивку и продемонстрировать результаты).

Если когда-то возникнет необходимость перекоммутировать виртуальный порт на другие линии, то для
этого достаточно будет незначительно изменить только объявление типа `LedPort`.

Полный код программы ниже:
```c++
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
```

Наконец-то пришли светодиоды, поэтому обновляю материал, в цикле счетчик увеличивается от 0 до 15 (0b0000...0b1111),
то есть 4 светодиода с задержкой в 1 секунду отобразят все возможные комбинации.

![Демонстрация записи в виртуальный порт](img/1.gif)