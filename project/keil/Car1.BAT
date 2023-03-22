SET PATH=A:\Keil_v5\ARM\ARMCC\Bin;A:\VMware\VMware Workstation\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;A:\Bandizip\;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;A:\MATLAB\R2022b\bin;C:\Program Files\Git\cmd;A:\Anaconda3\Scripts;C:\Users\Lixin\AppData\Local\Programs\Python\Python311\Scripts\;C:\Users\Lixin\AppData\Local\Programs\Python\Python311\;C:\Users\Lixin\AppData\Local\Microsoft\WindowsApps;;A:\VSCode\bin;A:\PyCharm\bin;
SET CPU_TYPE=MSP432P401R
SET CPU_VENDOR=Texas Instruments
SET UV2_TARGET=Car1
SET CPU_CLOCK=0x00B71B00
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\system_msp432p401r.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\build\startup_msp432p401r_uvision._ia"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\main.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\adc.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\key.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\key4x4.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\led.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\oled.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\tim32.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\tima.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\usart.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\sysinit.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\delay.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\baudrate_calculate.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\infrared_led.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\motor.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\follow_line.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\fsm.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\build\pid.__i"
"A:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Build\Car1.lnp"
"A:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Build\Car1.axf" --i32combined --output ".\Build\Car1.hex"
fromelf --bin --output .\Build\my_project_ROM.bin .\Build\Car1.axf
