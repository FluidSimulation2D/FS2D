## Моделирование движения несжимаемой жидкости
Уравнение Навье-Стокса - система дифференциальных уравнений в частных производных, описавающая движение вязкой несжимаемой ( $\rho = const$ ) жидкости.

$${\partial{\widetilde{\bf{u}}}\over{\partial{t}}}=-{(\widetilde{\bf{u}}\cdot\nabla)\widetilde{\bf{u}}} - {1\over{\rho}}{\nabla\bf{p}}+{\nu\nabla^2\widetilde{\bf{u}}} + \widetilde{\bf{F}}$$

где $\nu$ - коэффициент вязкости,
    $\rho$ - плотность жидкости.

Данное уравнение можно разделить на 5 частей:
-  ${\partial{\widetilde{\bf{u}}} \over{\partial{t}}}$ — обозначает скорость изменения скорости жидкости в точке 
-  $-{(\widetilde{\bf{u}}\cdot\nabla)\widetilde{\bf{u}}}$ — перемещение жидкости в пространстве
-  $-{1\over{\rho}}{\nabla\bf{p}}$ — давление, оказываемое на частицу 
-  ${\nu\nabla^2\widetilde{\bf{u}}}$ — описывает вязкость среды (чем она больше, тем сильнее жидкость сопротивляется силе применяемой к ее части)
-  ${\widetilde{\bf{F}}}$ — внешние силы, применяемые к жидкости (отражает действия пользователя)

#### Требования по сборке:
Окружение | Версия
-----------|-------
CMake|3.14
MSVC compiler|v142
CUDA Toolkit| 11.8

#### Внешние зависимости
Библиотека|Версия
----------|------
SFML|2.5.1
TGUI|1.0
