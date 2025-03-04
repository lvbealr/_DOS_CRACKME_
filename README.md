<p align="center">
  <a href="" rel="noopener">
 <img src="https://i.imgur.com/csedVT3.png" alt="Project logo"></a>
</p>

---

<h1 align="center">Research of vulnerabilities of binary code based on x86-64 architecture and writing a crack program for hacking a .COM file</h1>

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

| `0000:0199 - 0000:01AC` | `0000:01AD - 0000:01BA` |     `0000:01BB`     | `0000:01BC - 0000:01D5` | `0000:01D6` | `0000:01D7 - 0000:01DC` | `0000:01DD - 0000:01DF` |     `0000:01E0`      | `0000:01E1 - 0000:0000:021B` | 
|:-----------------------:|:-----------------------:|:-------------------:|:-----------------------:|:-----------------------:|:-----------------------:|:-----------------------:|:--------------------:|:-----------------------:|
|     PASSWORD BUFFER     |  DEFAULT FAIL MESSAGE   | TERMINATE SYMBOL `$` | SUCCESS MESSAGE | TERMINATE SYMBOL `$` | TRUE PASSWORD | TERMINATE SYBOLS `\r`, `\n`, `$` | SPECIAL MESSAGE FLAG | SPECIAL MESSAGE |  

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


