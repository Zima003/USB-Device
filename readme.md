# USB DEVICE
#### GPIO
Для работы с интерфейсом GPIO была разработана функция Demo_GPIO. В начале создаётся структура WINUSB_PIPE_INFORMATION, которая содержит в себе: тип канала, идентификатор канала (ID), максимальный размер пакетов в байтах,передаваемых по каналу, интервал канала. Затем для настройки режима работы GPIO создается буфер szBuffer с следующими командами:  
Таблица 1. Команда установки младшего байта

|   0x80| Value |Direction|
| ------------- | ------------- |-|


Устанавливает направление младших восьми линий GPIO и формирует на выходах указанное значение. Направление определяется Direction. (Direction[0] соответствует DBUS[0], Direction[7] соответствует DBUS[7], значение бита в поле Direction указывает на направление: 1 - «выход», 0 - «вход»). Значение Value[i] устанавливается на DBUS [i], в случае если Direction[i] = 1.  
Таблица 2. Команда установки старшего байта
|   0x82| Value |Direction|
| ------------- | ------------- |-|
Устанавливает направление старших восьми линий GPIO и формирует на выходах указанное значение. Направление определяется Direction. (Direction[0] соответствует CBUS[0], Direction[7] соответствует CBUS[7], значение бита 1 в поле Direction указывает на направление «выход», 0-на направление «вход»). Значение Value[i] устанавливается на CBUS [i], в случае если Direction[i]=1.  
Таблица 3.Команда установки делителя тактового сигнала
|   0x86| ValueL |ValueH|
| ------------- | ------------- |-|
Устанавливает значение делителя частоты для режима последовательной передачи данных. Тактовый сигнал всегда имеет скважность 50 %. Расчет частоты тактового сигнала определяется по формуле (1), если делитель на пять включен и по формуле (2), если делитель на пять выключен:  
F(TCK) = 12 МГц / ((1+ ValueL+ValueH*256)*2),                        (1)

F(TCK) = 60 МГц / ((1+ ValueL+ValueH*256)*2).                        (2)

Для выбора режима работы микросхемы (UART,SPI,GPIO) была разработана функция SetSIUMode, в которой сначала проверяется валидность дескриптора интерфейса, а затем создается и отправляется пакет данных для настройки режима работы, согласно документации микросхемы:

Таблица 5. Команда установки режима работы интерфейсов
| bmRequestType  | bRequest |wValue|wIndex|wLength|
| ------------- | ------------- |-|-|-|
| 0b01000001  | 0x00  |Значения битов: [15:8] – зарезервировано;  [7:0] – режим работы: 0 –интерфейс отключен; 1 – UART; 2 – SFIFO; 3 – SPI/GPIO|Номер интерфейса|0x0000|
Далее считается размер буфера для записи настроек в канал. После выставляется режим работы GPIO функцией SetSIUMode. Следующем шагом является извлечения информации о канале для структуры WINUSB_PIPE_INFORMATION с помощью функции  WinUsb_QueryPipe. Затем все выбранные настройки загружаются в микросхему посредством функции WinUsb_WritePipe.
Создается массив ToSJG, куда записываются: адрес отправки посылки, данные для отправки  и выбор режима работы (вход/выход).
Заключительным шагом становится передачи данных с помощью функции WinUsb_WritePipe по каналу, сформированной в ToSJG посылки.
#### UART
Для настройки  интерфейса UART написана функция SetUartLineCoding, которая позволяет настраивать нужный скорость передачи и формат кадра (количество стоп битов, бит четности, количество битов данных). В начале этой функции проверяется валидность дескриптора интерфейса,если он неправильный, то программа выдаёт ошибку. Согласно документации микросхемы настройка UART производится с помощью определенной команды:  
Таблица 6. Команда установки параметров передачи
| bmRequestType  | bRequest |wValue|wIndex|wLength|
| ------------- | ------------- |-|-|-|
| 0b00100001  | 0x20  |0x0000|Номер интерфейса|0x0007|
Эти данные были записаны в пакет SetupPacket. Также, согласно спецификации, требуется посылка 7 байт с параметрами настройки интерфейса UART:
| Смещение  | Поле |Размер|Значение|Описание|
| ------------- | ------------- |-|-|-|
| 0  | dwDTERate |4|Число|Скорость передачи, бит / с|
|4|bCharFormat|1|Число|Количество стоп-бит: 0: 1 стоп-бит; 1: 1,5 стоп-бита; 2: 2 стоп-бита;|
|5|bParityType|1|Число|Бит четности: 0: нет бита четности; 1: нечетный (odd); 2: четный (even); 3: единица (mark); 4: ноль (space)|
|6|bDataBits|1|Число|Количество битов данных. Допустимые значения: 5, 6, 7, 8, 16|
Все параметры передачи записываются в пакет UartLineCoding, а затем с помощью библиотечной функции WinUsb_ControlTransfer по адрес из SetupPacket отправляется посылка UartLineCoding.
Для запуска UART используется функция Test_UART, аргументами которой являются: дескриптор интерфейса, параметры передачи данных интерфейса, а также данные для отправки Затем создаётся структура WINUSB_PIPE_INFORMATION, которая содержит в себе: тип канала, идентификатор канала (ID), максимальный размер пакетов в байтах, передаваемых по каналу, интервал канала. Выставляется работа в режиме UART с помощью описанное выше функции SetSIUMode. Следующем шагом является извлечение информации о канале для структуры WINUSB_PIPE_INFORMATION с помощью функции  WinUsb_QueryPipe. Затем, используя функцию WinUsb_WritePipe отправляются данные, вводимые пользователем по каналу, полученному из WINUSB_PIPE_INFORMATION.  
#### SPI
Следующей задачей стала разработка программного обеспечения для цифрового интерфейса SPI. Первым шагом становится создание структуры  WINUSB_PIPE_INFORMATION, а также определение других переменных. Далее в массив szBuffer записывается команда для тактирования интерфейса (таблица 4). Устанавливается работа в режиме SPI с помощью SetSIUMode, а затем отправляется команда, записанная в szBuffer. После формируется массив cmd_buf, в котором  записываем команду для создания определенного количества  периодов, а затем команда записи (идентичная команде, которая в GPIO) в SPI.  
Таблица 7. Команды формирования большого количества периодов 
|   0x86| LengthL |LengthH|
| ------------- | ------------- |-|
Формирует заданное (LengthL + 256 x LengthH + 1) x 8 количество тактовых периодов. 
Затем рассчитывается размер массива cmd_buf и с помощь функции WinUsb_WritePipe записываются данные.  




















