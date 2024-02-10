# digital signature
Реализация тестового задания 
> Приложение для Windows.
> 1.	Предложить свой алгоритм формирования цифровой подписи для данных .
> 2.	Подпись должна быть индивидуальна  с достаточно высокой вероятностью. Как минимум возможность формирования 1024 варианта подписи.
> 
> ----------------------------------------------------------------------------------
> 1.	Приложение читает данные из файла, указанного при запуске или запрошенного от пользователя .
> 2.	Проверяет наличие цифровой подписи этого файла данных.
> 3.	О наличии/ отсутствии подписи сообщает.
> 4.	Предлагает убрать/добавить подпись.
> 5.	Результат писать в другой файл. Имя его получать при запуске  или формировать автоматически.'''
>
оценка снизу на количество уникальных подписей > 1024
(./signature.cpp -> void addDigitalSignature(const std::string &filename) -> std::vector<int> numbers ) numbers для генерации ключей содержит 46 простых чисел, из которых можно получить С(46,2) = 1035 возможных пар(<p,q> и <q,p>, в контексте предложенного алгоритма являются одной парой)
      
## build
### Linux
> mkdir build
> cd build
> cmake ..
> make
### Windows
> mkdir build
> cd build
> cmake .. 
> msbuild sig_auth.sln /p:Configuration=Release
or 
> mkdir build
> cd build
> cmake -G "Visual Studio 16 2019" ..
> msbuild sig_auth.sln /p:Configuration=Release
