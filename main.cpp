/*
    условия задачи приводить не буду, что бы в поисковиках мой результат не выдавался
*/

#include <iostream>
#include <vector>

// функция разности по модулю
unsigned short uintModulusDifferenceUint( unsigned short arg1 , unsigned short arg2, unsigned short m){
    int result = arg1 - arg2;
    if (result < 0) result += m;
    return result%m;
}

// функция суммы по модулю
unsigned short uintModulusSumUint( unsigned short arg1 , unsigned short arg2, unsigned short m){
    unsigned int result = arg1 + arg2;
    return result%m;
}

// функция произведения по модулю
unsigned short uintModulusMultiplicationUint( unsigned short arg1 , unsigned short arg2, unsigned short m){
    unsigned long result = arg1 * arg2;
    return result%m;
}

// расширенный алгоритм Евклида http://e-maxx.ru/algo/export_extended_euclid_algorithm
int gcdex(int a, int b, int & x, int & y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcdex (b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

// функция нахождения обратного элемента по модулю http://e-maxx.ru/algo/export_reverse_element
unsigned short uintModulusInverse(unsigned short arg, unsigned short m)
{
    int x, y;
    int g = gcdex(arg, m, x, y);
    if (g != 1)
        return 0; // возвращенный ноль бужет означать, тчо решения нет
    else {
        x = (x % m + m) % m;
        return x;
    }
}

struct Parameter { // структура для параметров
    unsigned short m = 0; // так как по условию задачи m не более 65535 значит имеем право хранить его в переменной типа ushort ее диапазон значений [0:65535]
    unsigned short a = 0; // параметры a и c элементы поля Галуа по параметру m,
    unsigned short c = 0; // а значит не превосходят его по значению => имеем право так же хранить их значения в переменных типа ushort

    /*

    В случае если бы параметр m требовался бы больше 4 294 967 295 (максимальное значение unsigned long),
    то пользоваться обычными типами данных было бы нельзя.
    Под запись результата перемножения unsigned long (32 бита) на unsigned long (32 бита) требовалсь бы переменная типа unsigned long long (64 бита).
    В случае умножения unsigned long long (64 бита) на unsigned long long (64 бита) под результат требовалось бы 128 бит, а с типа данных,
    чья запись переменной 128 бит в нотации C++ нет. Для таких случаев требуется собственная библиотека для работы с большими числами.

    Пример такой блиотеки:                                                                ^
    Заголовочный файл https://bitbucket.org/LMNV/kuznechik-ecdh/src/master/bignum.h      /U\
    Исполнительный файл https://bitbucket.org/LMNV/kuznechik-ecdh/src/master/bignum.cpp /_o_\

    */

    unsigned short result_iteration = 0; // переменная для хранения результата итерации,
    // ее размерность в два раза больше размерности переменных под хранение параметров,
    // потому что она будет использованная как буфферная переменная при расчетах

};

unsigned short getParam(
        std::string param_name // обозначение параметра
        ){

    int param; // переменная для параметра

    for (;;)// цикл продолжается до тех пор, пока пользователь не введёт корректное значение
    {
        std::cout << "Enter \t\t\""<< param_name <<"\": "; // просьба ввести значение
        std::cin >> param; // попытка извлечения

        if ((std::cin.fail())||(param < 0)||(param > 65535)) // если предыдущее извлечение оказалось неудачным,
            // или введено отрицательное число, или чило больше максимального,
        {// то
            std::cout << "Invalid value \t\""<< param_name <<"\"" << std::endl << std::endl; // сообщение о некоректном вводе
            std::cin.clear(); // возвращаем cin в 'обычный' режим работы
            std::cin.ignore(32767,'\n'); // и удаляем значения предыдущего ввода из входного буфера
        }
        else // если всё хорошо, то возвращаем параметр
            return param;
    }
}

unsigned short generateNextValue( // функция нахождения следующего значения в генераторе
                                  Parameter param // параметры
                                  ){
    unsigned int result = 0; // переменная так же будет выступать буффером для результата перемножения двух unsigned short

    // берем по параметры по модулю параметра m, пользователь мог ввести несовсем коректные данные
    param.a = param.a%param.m;
    param.c = param.c%param.m;
    param.result_iteration = param.result_iteration%param.m;

    /*

    Разобьем рекурсивную формулу X_i+1 = (X_i * a + c) mod m на подсастовные вычисления

    при помощи справедливых формул для конечных полей

    (a+b) mod m = ( (a)mod m + (b)mod m ) mod m
    (a-b) mod m = ( (a)mod m - (b)mod m ) mod m
    (a*b) mod m = ( (a)mod m * (b)mod m ) mod m

    */

    result = param.result_iteration*param.a; // X_i * a
    result = result%param.m; // (X_i * a) mod m если это несделать,
    //то при добавлении к "X_i * a" параметра "c" значение переменной может перевалить за максимальное,
    //тогда геультат будет равен X_i+1 = ( (X_i * a + c) mod (максимальное значение переменной) ) mod m
    result = result + param.c; // (X_i * a) mod m + (c)mod m
    param.result_iteration = result%param.m; // ( (X_i * a) mod m + (c)mod m ) mod m, что равно (X_i * a + c) mod m

    return  param.result_iteration;
}

std::vector <Parameter> defineParametersByValues(std::vector <unsigned short> input_values){ // функция нахождения параметров по входному вектору

    std::vector <Parameter> result_param; // экземпляр структуры для параметров

    unsigned short m_min = 0; // значение с которого будет необходимо начать перебор

    for (unsigned int i = 0 ; i < input_values.size() ; i++) { // определим максмальное значение из входного вектора
        if (m_min < input_values[i])
            m_min = input_values[i];
    }
    m_min++; // прибавим к найденному значению 1-у, так как если у нас  есть элемент x,
    // то перебор значений модуля необходимо начать с x+1, т.к. x mod x был бы 0-м

    for (
         unsigned short param_m = m_min; // начинаем с минимально возможного значения
         param_m !=0 ; // равноценно  i < 65536
         param_m++ // следующее значение
         ) {
        unsigned short param_a = // параметр a сравним с (X_3-X_2)/(X_2-X_1) по модулю м
                uintModulusMultiplicationUint // операция умножения по модулю
                (uintModulusDifferenceUint(input_values[3], input_values[2], param_m), // тут (X_3-X_2)  по модулю м
                uintModulusInverse(uintModulusDifferenceUint(input_values[2], input_values[1], param_m),param_m), // тут (X_2-X_1)^(-1)  по модулю м
                param_m); // сам модуль
        if (param_a == 0) {continue;} // означает, что обратный элемент небыл найден
        // найдем потенциальный параметр c
        unsigned short param_c = uintModulusDifferenceUint // это X_2-aX_1 по модулю м
                (input_values[2],
                uintModulusMultiplicationUint(param_a, input_values[1], param_m ),
                param_m);

        if ( //если сравнимы по модулю
             param_c // X_2-aX_1
             == // и
             uintModulusDifferenceUint(input_values[1], uintModulusMultiplicationUint(param_a, input_values[0], param_m ), param_m) // X_1-aX_0
             ){ // то значит параметры а и с потенциально верные и надо проверить соответствуют ли они входной выборке

            bool flag = true; // флаг соответствия
            Parameter potential_param; // создадим экземпляр структуры параметров с потенциально возможными значениями
            potential_param.m=param_m;
            potential_param.a=param_a;
            potential_param.c=param_c;
            potential_param.result_iteration = input_values[0];
            for (unsigned int i = 1; i < input_values.size() ; i++) { // проверим верное ли решение
                potential_param.result_iteration = generateNextValue(potential_param);
                if (input_values[i] != potential_param.result_iteration){
                    flag = false;
                    break;
                }
            }

            if (flag){ // если решение коректно, то
                result_param.push_back(potential_param); //записываем в результирующий массив решений параметры которые мы получили
                std::cout << "Solution #"<< std::to_string(result_param.size()) <<": \t\t\"m\"="<< potential_param.m << "\t\t\"a\"="<< potential_param.a << "\t\t\"c\"="<< potential_param.c << std::endl; // и выводим новое решение в консоль

                /*

                можно просто возвращать одно правильное решение,
                но это слишком просто

                          /\
                         /  \
                        / __ \
                       / |  | \
                      /  |  |  \
                     /   |  |   \
                    /    |  |    \
                   /     |  |     \
                  /      |__|      \
                 /       ____       \
                /       (____)       \
               /______________________\


                Даже в примере задачи для входных чисел {157;  5054; 25789; 13214}
                будет два коректных решения
                "m"=32768 "a"=31 "c"=187 тут как раз таки следующим сгенерированным элементом будет 16605
                "m"=49152 "a"=16415 "c"=32955 а тут 32989

                */

            }
        }
    }

    return result_param;

}

int main()
{
    unsigned int count_iteration_results = 4;

    std::vector <unsigned short> X; // динамический массив для исходных данных по итерациям

    // получаем значения предыдущих итераций
    for (unsigned int i = 0 ; i < count_iteration_results; i++) {
        X.push_back(getParam("X_" + std::to_string(i))); // добавляем в динамический массив новое значение
    }

    std::vector <Parameter> my_param = defineParametersByValues(X); // структура в которую будут записаны параметры, которые необходимо определить

    unsigned int count_index_param = 0 ; // переменная инфекса набора параметров

    if (my_param.size()==0){ //  если решений нет

        std::cout << "No solutions" << std::endl; // сообщаем, что решений нет
        return 0; // завергаем работу программы

    }

    if (my_param.size()!=1){ //  если решение несколько, то пользователю необходиом выбрать один из них

        std::cout << "There are several correct solutions" << std::endl; // сообщаем, что решений несколько

        for (;;) {
            std::cout << "Which solution select: \t\t"; // ввод номера решения
            std::cin >> count_index_param; // попытка извлечения

            if ((std::cin.fail())||(count_index_param < 0) ||(count_index_param > my_param.size())) // если предыдущее извлечение оказалось неудачным,
                // или введено отрицательное число
                // или ведено число большее чем количество полученных вариантов
            {// то
                std::cout << "Invalid value" <<  std::endl; // сообщение о некоректном вводе
                std::cin.clear(); // возвращаем cin в 'обычный' режим работы
                std::cin.ignore(32767,'\n'); // и удаляем значения предыдущего ввода из входного буфера
            }
            else // если всё хорошо, то возвращаем параметр
            {// то
                break;
            }
        }

        count_index_param--; // Пользователь вводил число от 1-ы, а не как програмист от 0-ля

    }


    for (
         unsigned int count_step = 1 ; // переменная для количества итераций
         count_step > 0 ;
         count_step--
         ) {

        my_param[count_index_param].result_iteration = generateNextValue(my_param[count_index_param]);

        std::cout << "New value: \t\t\""<< my_param[count_index_param].result_iteration <<"\"" << std::endl; // вывод нового значения

        if (count_step == 1){
            std::cout << "All iterations completed,\nif you wish to continue, enter a new number of iterations: \t\t" << std::endl; // вывод количества иитераций
            std::cin >> count_step; // попытка извлечения

            if ((std::cin.fail())||(count_step < 0)) // если предыдущее извлечение оказалось неудачным,
                // или введено отрицательное число
            {// то
                break;
            }
            else // если всё хорошо, то возвращаем параметр
            {// то
                std::cin.clear(); // возвращаем cin в 'обычный' режим работы
                std::cin.ignore(32767,'\n'); // и удаляем значения предыдущего ввода из входного буфера
            }
        }

    }

    return 0;
}
