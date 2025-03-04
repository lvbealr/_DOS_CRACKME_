<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/csedVT3.png" alt="Project logo"></a>
</p>

## 📖 Версия / Version
- [🇷🇺 RU](#RU)
- [🇺🇸 ENG](#ENG)

---

<h1 align="center">Исследование уязвимостей машинного кода ассемблера х86-64 и написание crack-программы для взлома .com файла</h1>

## 🇷🇺 RU <a name="RU"></a>

---

## 🛠 Инструменты
1) Весь код на ассемблере, приведенный в этом репозитории, был исполнен и протестирован на эмуляторе [DOSBox](https://www.dosbox.com/) - он дает много свободы в работе с памятью. Для компиляции и отладки были использованы [**TASM**](https://ru.wikipedia.org/wiki/TASM), **TLINK** и **TURBO-DEBUGGER**. Анализ бинарного кода был произведен с помощью [Ghidra](https://github.com/NationalSecurityAgency/ghidra) с параметрами **mode:** `x86:LE:16:Real Mode:default` и **base address:** `0000:0100`
2) Исходный код на **C** был собран и скомпилирован с помощью [**CMake**](https://cmake.org/) и **g++** (14.2.1) на ОС **Arch Linux**. Для создания графического интерфейса использовалась библиотека [**GTK 4.0**](https://www.gtk.org/).

> [!IMPORTANT]  
> Все необходимые CMake - файлы содержатся внутри директорий

---
## 🎯 Цель работы
Эта работа симулирует процесс взлома программы, написанной на ассемблере х86-64 для MS-DOS.

> [!NOTE]  
> Был получен исполняемый .com файл с несколькими уязвимостями. Теперь необходимо проанализировать машинный код и получить права доступа, которые выдает программа.

---

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/xSfR3BA.png" alt="Work Progress"></a>
</p> 

---

При запуске программа просит пользователя `ввести пароль`:
<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/ivTjkEg.png" alt="Access Denied" width = 640"></a>
</p>

Если введеный пароль оказывается неверным, на экран выводится сообщение `Access denied!`.

Если же пароль верный, программа сообщает об успехе: `Correct Password, Welcome!`:
<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/6XaCvOV.png" alt="Correct Password" width = 640"></a>
</p>

---

После декомпиляции, можем изучить код программы.
Взглянув на него, на адресах `0000:019b - 0000:01aa` можно увидеть нулевые байты, окруженные с двух сторон байтами `05 15` (`0000:0199 - 0000:019a` | `0000:01ab - 0000:01ac`).

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/yI8kgfu.png" alt="User Password Buffer" width = 640"></a>
</p>

С уверенностью можно предположить, что для считывания введенного пароля в памяти выделен буфер, окруженный с двух сторон двухбайтными канарейками.

К тому же, рядом с буфером для пароля в памяти можно найти сообщения, которые выводятся при завершении программы.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/MEzsDZA.png" alt="Messages" width = 640"></a>
</p>


> [!IMPORTANT]
> Возникает вопрос, что произойдет, если поломать канарейки?

Вернемся к началу программы: в глаза сразу бросаются уже знакомые нам адреса.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/v0jdEnV.png" alt="User Password Buffer" width = 640"></a>
</p>

```asm
0000:0100   mov     ax, [0x199]
0000:0103   mov     bx, [0x1ab]

0000:0107   cmp     ax, bx
0000:0109   jz      LAB_0000_010f
0000:010b   inc     byte ptr [0x1e0]
```

Эти инструкции проверяет равенство канареек, т. е. их валидность. Так как буфер для пароля защищен левой канарейкой, нельзя просто так перезаписать данные в памяти.
Всегда необходимо заботиться о валидности канареек.

Если проверка успешна, выполняется прыжок на метку `LAB_0000_010f`

Следующая часть кода считывает пароль с потока ввода в буфер размером 16 байт, расположенный в памяти по адресам `0000:019b - 0000:01aa`.

Похоже, что при вводе неправильного пароля возможны два исхода:
1) Если канарейки целые и валидные, на экран выводится стандартное сообщение о неполучении доступа `Access denied!`
2) В другом же случае, инструкция
```asm
0000:010b   inc     byte ptr [0x1e0]
```
выставляет некоторый флаг. Он отвечает за вывод другого более интересного сообщение о возможной атаке:

<p align="center" style="font-weight: 1000; font-size: 16px">IF YOU'LL TRY TO FOOL ME AGAIN, I'll SLAP YOUR LITTLE ASS! 🤪</p>

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/XS6vnoD.png" alt="Broken Canaries" width = 640"></a>
</p>

> Что за [забавный парень](https://github.com/livlavr) написал эту программу?.. 🙀

---

Нам до сих пор неизвестно, где в памяти располагается настоящий пароль. Попробуем найти в коде инструкции, которые сравнивают некоторые байты в памяти
(скорее всего, они укажут нам на адреса буфера ввода и настоящего пароля).

Посмотрим на адреса `0000:015e - 0000:017f`. Заметим, что в регистр **SI** положили некоторое значение `0x1d7`, и `адрес, указывающий на буфер введенного пароля` был сохранен в регистре **DI**. Смело можно предположить, что **настоящий пароль расположен по адресу** `0000:01d7`, и он ограничен `тремя терминальными символами`: `\r` (0x0d), `\n` (0x0a), `$` (0x24).

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/SDwXoNY.png" alt="Passwords Comparison" width = 640"></a>
</p>

| `0000:0199 - 0000:01AC` |     `0000:01AD - 0000:01BA`     |      `0000:01BB`       | `0000:01BC - 0000:01D5` |       `0000:01D6`       | `0000:01D7 - 0000:01DC` |       `0000:01DD - 0000:01DF`        |              `0000:01E0`              |     `0000:01E1 - 0000:021B`     | 
|:-----------------------:|:-------------------------------:|:----------------------:|:-----------------------:|:-----------------------:|:-----------------------:|:------------------------------------:|:-------------------------------------:|:-------------------------------:|
|    Буфер для пароля     | Стандартное сообщение о неудаче | Терминальный символ `$` |   Сообщение об успехе   | Терминальный символ `$` |    Настоящий пароль     | Терминальные символы `\r`, `\n`, `$` | Флаг специального сообщения о неудаче | Специальное сообщение о неудаче |  

Возможно ли просто перезаписать правильный пароль, затерев все сообщения? `Не все так просто`.
Оказывается, если мы затрем некоторые сообщения случайными символами, взлом не получится: проверка в функции `FUN_0000_0181()` сообщит о неудаче.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/N0lzNjJ.png" alt="ASCII-codes sum" width = 640"></a>
</p>

> [!IMPORTANT]
> Регистр BX накапливает сумму ASCII-кодов стандартного сообщение о неудаче

Попробуем аккуратно заменить пароль.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/7VhzQUG.png" alt="Change Password" width = 640"></a>
</p>


Получим сообщение об успехе!

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/kSZX9RX.png" alt="Success" width = 640"></a>
</p>

---


После того, как я нашел первую уязвимость, мой товарищ выпустил новую версию программы и мне предоставилась возможность переполнить буфер для ввода и достигнуть стека, в котором в некоторый момент времени лежит адрес возврата из стэка.
Для этого я создал файл `stack.txt` и заполнил символами до размера ~64KB, затем перенаправил ввод с файла в программу и подменил адрес возврата на функцию, которая разрешает доступ.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/9kBcEdu.png" alt="Success" width = 640"></a>
</p>

---

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/vQZN2NJ.png" alt="Binary Patch" width = 640"></a>
</p>

---

Так как ручной анализ не является рациональным способом взлома программы, мною был создан бинарный патч с графическим интерфейсом, построенным на базе фреймворка GTK4.0. Он заменяет настоящий пароль на любой другой.
Чтобы запустить программу, необходимо выполнить следующие команды:
```bash
$ git clone git@github.com:lvbealr/_DOS_CRACKME_.git
$ cd gtk
$ cmake .
$ make
$ ./gtkTest
```

После запуска программы, нужно ввести путь до файла (см. замечание) и нажать кнопку `CRACK`. Если искомый файл доступен, программа произведет взлом программы и выдаст пользователю подходящий пароль.

> [!IMPORTANT]
> Любой файл для взлома должен располагаться в папке gtk/files/

<p align="center">
  <a href="" rel="noopener">
 <img src="https://githubealr/_DOS_CRACKME_/blob/main/gtk/img/example.gif" alt="Example Gif"></a>
</p>


---

<h1 align="center">Research of vulnerabilities of binary code based on x86-64 architecture and writing a crack program for hacking a .COM file</h1>

## 🇺🇸 ENG <a name="ENG"></a>

---

## 🛠 Tools
1) All x86-64 asm code provided in this repository was executed and tested on the emulator [DOSBox](https://www.dosbox.com/). For compilation and debugging [**TASM**](https://ru.wikipedia.org/wiki/TASM), **TLINK**, **TURBO-DEBUGGER** were used. Binary code analysis was performed using [Ghidra](https://github.com/NationalSecurityAgency/ghidra) in the **mode** `x86:LE:16:Real Mode:default` with the parameter **base address** `0000:0100`
2) The **C** source code was compiled using [**CMake**](https://cmake.org/) and the **g++** (14.2.1) compiler on **Arch Linux**. The [**GTK 4.0**](https://www.gtk.org/) library is used for rendering graphics.

> [!IMPORTANT]  
> All necessary CMake files are contained in the directory

---
## 🎯 Goal
This work simulates the process of hacking a program on **x86-64 assembly** written for **MS-DOS**.

> [!NOTE]  
> A binary .com file with several vulnerabilities was received. It is necessary to analyze the machine code and obtain permission.

---

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/xSfR3BA.png" alt="Work Progress"></a>
</p> 

---

When you run the executable file, the console immediately asks you to `enter a password`:
<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/ivTjkEg.png" alt="Access Denied" width = 640"></a>
</p>

If you enter an incorrect password, the message: `Access denied!` is displayed.

If you managed to enter the correct password, the program displays a success message: `Correct Password, Welcome!`:
<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/6XaCvOV.png" alt="Correct Password" width = 640"></a>
</p>

---

After decompiling, we obtain the program code.
Looking at the code, you can see that at the addresses `0000:019b - 0000:01aa` there are zero bytes surrounded on both sides by 05 15 bytes (`0000:0199 - 0000:019a` | `0000:01ab - 0000:01ac`).

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/yI8kgfu.png" alt="User Password Buffer" width = 640"></a>
</p>

It is safe to assume that a buffer surrounded on both sides by two-byte canaries is allocated in memory for reading the entered password.

By the way, next to the password buffer you can find output messages on error/success.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/MEzsDZA.png" alt="Messages" width = 640"></a>
</p>


> [!IMPORTANT]
> The question arises, what happens if you break the canaries?

Let's look at the beginning of the code and we will immediately see familiar addresses.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/v0jdEnV.png" alt="User Password Buffer" width = 640"></a>
</p>

```asm
0000:0100   mov     ax, [0x199]
0000:0103   mov     bx, [0x1ab]

0000:0107   cmp     ax, bx
0000:0109   jz      LAB_0000_010f
0000:010b   inc     byte ptr [0x1e0]
```

This code checks for equality of canaries. Since the password buffer is protected by the left canary, you cannot swap both. You always have to worry about the validity of the right canary.
If the check is <span style="color:green">**successful**</span>, a jump is performed to the `LAB_0000_010f` label.

This piece of code reads the password into a 16-byte buffer located at addresses `0000:019b - 0000:01aa`. 

There seem to be two outcomes when entering an incorrect password.
1) If the canaries are not broken, the usual `Access denied!` message is displayed.
2) Otherwise, the instruction
```asm
0000:010b   inc     byte ptr [0x1e0]
```
sets a flag to display another more interesting message indicating an attempted hack: 

<p align="center" style="font-weight: 1000; font-size: 16px">IF YOU'LL TRY TO FOOL ME AGAIN, I'll SLAP YOUR LITTLE ASS! 🤪</p>

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/XS6vnoD.png" alt="Broken Canaries" width = 640"></a>
</p>

> What kind of [funny guy](https://github.com/livlavr) wrote this program?.. 🙀

---

We still don't know where the correct password is stored. Let's try to find the instructions in the code that perform the byte comparison.

Let's look at the addresses `0000:015e - 0000:017f`. We see that some value `0x1d7` was put into the **SI** register, and the `address of the buffer` where the password was saved is in the **DI** register. We can safely say that the **password is located at the address** `0000:01d7`, and its end can be determined by `three terminal symbols`: `\r` (0x0d), `\n` (0x0a), `$` (0x24).

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/SDwXoNY.png" alt="Passwords Comparison" width = 640"></a>
</p>

| `0000:0199 - 0000:01AC` | `0000:01AD - 0000:01BA` |     `0000:01BB`     | `0000:01BC - 0000:01D5` | `0000:01D6` | `0000:01D7 - 0000:01DC` | `0000:01DD - 0000:01DF` |     `0000:01E0`      | `0000:01E1 - 0000:021B` | 
|:-----------------------:|:-----------------------:|:-------------------:|:-----------------------:|:-----------------------:|:-----------------------:|:-----------------------:|:--------------------:|:-----------------------:|
|     PASSWORD BUFFER     |  DEFAULT FAIL MESSAGE   | TERMINATE SYMBOL `$` | SUCCESS MESSAGE | TERMINATE SYMBOL `$` | TRUE PASSWORD | TERMINATE SYBOLS `\r`, `\n`, `$` | SPECIAL MESSAGE FLAG |     SPECIAL MESSAGE     |  

Is it possible to simply rewrite the correct password to your own, erasing all messages? **It's not that simple.**
It turns out that memory is erased with random symbols. The protection is implemented in the function `FUN_0000_0181()`

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/N0lzNjJ.png" alt="ASCII-codes sum" width = 640"></a>
</p>

> [!IMPORTANT]
> The **BX** register accumulates the **sum of the ASCII codes** of the `DEFAULT FAIL MESSAGE`.

Let's try to carefully change the password:

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/7VhzQUG.png" alt="Change Password" width = 640"></a>
</p>

We will receive the coveted message about success!

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/kSZX9RX.png" alt="Success" width = 640"></a>
</p>

---

There is another more interesting vulnerability. It is possible to reach the stack by overflowing the entire .code segment (in the tiny model, the code and data segments coincide).
After finding the first vulnerability, my friend slightly modified the code, replacing individual labels (for example, entering a password and printing a message about access).
To do this, I created a stack.txt file and filled it with about 64KB of characters, then replaced the return address with the successful exit function.

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/9kBcEdu.png" alt="Success" width = 640"></a>
</p>

---

<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/vQZN2NJ.png" alt="Binary Patch" width = 640"></a>
</p>

---

Since manual file editing is not a rational way to hack the program, a binary patch with a graphical interface based on the GTK 4.0 framework was written. It replaces the real password with another one and gives it to the user.
To run the program, you must do the following:
```bash
$ git clone git@github.com:lvbealr/_DOS_CRACKME_.git
$ cd gtk
$ cmake .
$ make
$ ./gtkTest
```

After launching, you must specify the path to the file (see note) and press the `CRACK` button! If the required file is found, the program will crack and give the password to the user.

> [!IMPORTANT]
> Any file for hacking **must be** placed in the `gtk/files/` folder

<p align="center">
  <a href="" rel="noopener">
 <img src="https://github.com/lvbealr/_DOS_CRACKME_/blob/main/gtk/img/example.gif" alt="Example Gif"></a>
</p>


