SSD1306_I2C

SSD1306 + SI7021 + BMP180 температура, влажность, давление. 

Исходники на https://github.com/NimaMX/STM32F103-SSD1306
Переделан на IAR

PB6 - I2C1_SCL
PB7 - I2C1_SDA

Добавлена поддержка утилиты TheDotFactory (www.eran.io/the-dot-factory-an-lcd-font-and-image-generator/)
которая генерирует массив из выбранного шрифта, установленого на ПК. 
