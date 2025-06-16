# Формат файла сигнала

1. [Описание](#описание)
2. [Информация внутри файла](#информация-о-сигнале-внутри-файла)
3. [Порядок формирования](#порядок-формирования-файла)
4. [Структура файла](#структура-файла)

---

## Описание
  Файл сигнала - это сжатый в `tar.xz` бинарный файл, который содержит информацию о захваченном сигнале.
  Такой файл имеет нечувствительное к регистру расширение расширение `.SIG`

  Пример файла можно найти [здесь](./examples/signal_v2.sig)

---

## Структура файла
  Структура файла состоит из версии и тела. Для каждой версии будет разный парсинг. Но первые два байта
  любой версии файла - это сама версия файла.

---
  
### *Версия формата файла*
  - формат: `uint16`
  - порядок байт: `LE`
  - длина (байт): 2
  ```c++
  // Пример упаковки
  uint16_t version = to_little_endian(1);
  ios.write((char*)&version, sizeof(version));

  // Пример распаковки
  ios.read((char*)&version, sizeof(version));
  version = from_little_endian(version);
  ```

---

# Структура файла v2

### Информация о сигнале внутри файла
  В теле файла содержится следующая информация

  - время начала сигнала (точность до секунды)
  - кол-во микросекунд от начала секунды
  - координаты приема сигнала
    - широта
    - долгота
    - высота над уровнем моря
  - частота сэммплирования сигнала
  - флаги:
    - была выполнена синхронизация по PPS
    - была обнаружена перегрузка при оцифровывании
  - сэмплы

---

### *Время начала сигнала*
  - формат: `ASCII`
  - данные: `время по UTC`
  - формат времени: `yyyyMMddhhmmss`
  - длина (байт): 14
  ```c++
  // Пример упаковки
  std::time_t now = std::time(nullptr);
  std::string time {"yyyyMMddhhmmss\n"};
  std::strftime(time.data(), time.size(), "%Y%m%d%H%M%S", std::gmtime(&now));
  ios.write(time.data(), time.size() - 1);
  ```

### *Микросекунд с начала сигнала*
  - формат: `uint32`
  - порядок байт: `LE`
  - длина (байт): 4
  ```c++
  // Пример упаковки
  uint32_t microseconds = to_little_endian(123);
  ios.write((char*)&microseconds, sizeof(microseconds));

  // Пример распаковки
  ios.read((char*)&microseconds, sizeof(microseconds));
  microseconds = from_little_endian(microseconds);
  ```

### *Координаты*
  - формат: `uint16|uint32|uint16|uint32|uint16|uint32`
  - порядок байт: `LE`
  - данные: `широта|долгота|высота` -> `uint16.uint32|uint16.uint32|uint16.uint32`
  - длина (байт): 18
  ```c++
  // Пример упаковки
  double longitude = 0.123456;
  double latitude = 45.123456;
  double altitude = 23.123456;

  uint16_t lon1 = to_little_endian(std::floor(longitude));
  uint16_t lat1 = to_little_endian(std::floor(latitude));
  uint16_t alt1 = to_little_endian(std::floor(altitude));

  uint32_t lon2 = to_little_endian(std::floor((longitude - lon1) * 1'000'000));
  uint32_t lat2 = to_little_endian(std::floor((latitude - lat1) * 1'000'000));
  uint32_t alt2 = to_little_endian(std::floor((altitude - alt1) * 1'000'000));

  ios.write((char*)&lon1, sizeof(lon1));
  ios.wirte((char*)&lon2, sizeof(lon2));
  ios.write((char*)&lat1, sizeof(lat1));
  ios.wirte((char*)&lat2, sizeof(lat2));
  ios.write((char*)&alt1, sizeof(alt1));
  ios.wirte((char*)&alt2, sizeof(alt2));

  // Пример распаковки
  ios.read((char*)&lon1, sizeof(lon1));
  ios.read((char*)&lon2, sizeof(lon2));
  ios.read((char*)&lat1, sizeof(lat1));
  ios.read((char*)&lat2, sizeof(lat2));
  ios.read((char*)&alt1, sizeof(alt1));
  ios.read((char*)&alt2, sizeof(alt2));

  lon1 = from_little_endian(lon1);
  lon2 = from_little_endian(lon2);
  lat1 = from_little_endian(lat1);
  lat2 = from_little_endian(lat2);
  alt1 = from_little_endian(alt1);
  alt2 = from_little_endian(alt2);

  longitude = double(lon1) + double(lon2) / 1'000'000;
  latitude = double(lat1) + double(lat2) / 1'000'000;
  altitude = double(alt1) + double(alt2) / 1'000'000;
  ```

### *Частота сэмплирования*
  - формат: `uint32`
  - данные: `частота сэмплирования в Герцах`
  - длина (байт): 4
  - порядок байт: `LE`
  ```c++
  uint32_t rate = to_little_endian(123);
  ios.write((char*)&rate, sizeof(rate));

  // Пример распаковки
  ios.read((char*)&rate, sizeof(rate));
  rate = from_little_endian(rate);
  ```

### *Флаги*
  - формат: `uint8_t`
  - длина (байт): 1
  - данные: `биты, флаги с информацией`
    - флаг: `перегрузка по АЦП зафиксирована` бит `1` (`uint8_t(0) | 1`)
    - флаг: `оцифровка с предварительной синхронизацией` бит `2` (`uint8_t(0) | 2`)
  
  ```c++
  // Пример упаковки
  enum Flags {
    overflow = 1,
    pps_sync = 2
  };

  uint8_t flags = overflow | pps_sync;
  ios.write((char*)&flags, 1);

  // Пример распаковки
  uint8_t flags;
  ios.read((char*)flags, 1);
  
  bool is_overflow = flags & overflow;
  bool is_pps_sync = flags & pps_sync;
  ```

### *Сэмплы*
  - формат: `array` -> `uint32|int16...`
  - данные: `сэплы смещенные в положительные значения`
  - порядок байт: `LE`
  - формат данных:
    - кол-во сэмплов: `uint32`
    - сэмплы: `uint16`, где каждый сэмпл смещен на `+1000`
  ```c++
  // Пример упаковки
  using sample_t = int16_t;
  
  std::vector<sample_t> samples;

  uint32_t size = to_little_endian(samples.size());
  ios.write((char*)&size, sizeof(size));

  for (sample_t s : samples) {
    int16_t sample = (s + 1000);
    sample = to_little_endian(sample);
    ios.write((char*)&sample, sizeof(sample));
  }

  // Пример распаковки
  ios.read((char*)&size, sizeof(size));
  size = from_little_endian(size);

  samples.resize(size);
  for (sample_t& s : samples) {
    int16_t sample = 0;
    ios.read((char*)&sample, sizeof(sample));
    s = from_little_endian(sample);
  }
  ```

---

# Структура файла v3

Файл упакован в формат `msgpack`

## Структура файла

> Структура файла не содержит ключей как в привычном JSON. Только поля в описанном ниже порядке.
> Если порядок нарушен, файл считается невалидным сигналом.

Порядок данных в файле сигнала (в квадратных скобках указан размер в байтах)
1. `string[14]` - время в формате `yyyymmddHHMMSS`
2. `uint` - кол-во микросекунд от начала секунды
3. `double` - долгота в градусах
4. `double` - широта в градусах
5. `double` - высота над уровнем моря в метрах
6. `uint` - частота дискретизации сигнала
7. `uint` - сопротивление нагрузки АЦП
8. `uint` - разрешение АЦП бит
9. `double` - опорное напряжение АЦП
10. `uint` - битовые флаги сигнала
    - `0` - флаги не установлены
    - `1` - сигнал захвачен с перегрузкой по АЦП
    - `2` - сигнал захвачен с синхронизацией по PPS
11. `uint` - тип антенны
    - `0` - неизвестный тип антенны
    - `1` - минивип
    - `2` - магнитная антенна
12. `array uint` - захваченный сигнал
