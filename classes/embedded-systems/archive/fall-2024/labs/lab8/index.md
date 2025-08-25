---
layout: page
title: Lab 8
parent: Labs
nav_exclude: true
---

# Lab 8: Autonomus Vehicle

## Week 1

### Introduction

This first week is devoted to assembling and testing your autonomous robot car.
First you and your partner will modify your SoC to include the peripherals
necessary for communication with the car sensors. Then you will follow the video
instructions to assemble the shadow chassis, which is the base platform for your
Arty Bot. Your TA will be available to help you through the assembly process.
Zip ties and "engineering grade" rubber bands will be available in the lab if you
need them.

### Lab Instructions

{: .note }
> Start by downloading this [IP Repository](./assets/ip_repo.zip). It contains 
> some of the IPs we'll need for our robot project. Extract the "ip_repo" directory 
> and place it in your SoC project directory.

1. Now let us make some changes to our SoC (System on Chip). Start by opening
your project in Vivado. In the __Flow Navigator__ on the left side, navigate to
__“Open Block Design.”__
2. To add our downloaded IP to Vivado, navigate to __“Project Settings”__ to open
the __Project Settings__ dialog. In the left panel select __“IP,”__ then select the
__“Repository Manager”__ tab. Now add the IP repository folder you copied earlier.
Select __“Apply”__ and close the dialog.
3. Back in the block design of your SoC. We want to add IP to the SoC for
communicating with the external Pmod peripherals. We will add AXI GPIO,
Pmod_DHB1, and Pmod_Dual_MAXSONAR IP for communication with the two line
detecting light sensors, Pmod MAXSONAR sensor, and Pmod DHB1 motor driver.  

    {: .warning }
    > __Do not use connection automation...__  

    1. __AXI GPIO >> jA >> Pmod LS1:__
        - Using the __+ Add IP__ button and dialog. Add a new __AXI GPIO__ and
        __Pmod Bridge__ IP to your design.
        - Connect the __S_AXI__ bus port of the AXI GPIO to a port on the interconnect.
        - Connect the two GPIO ports on the AXI GPIO with the __GPIO_Top_Row__ and
        __GPIO_Bottom_Row__ on the Pmod Bridge.
        - Finally, in the “Board” window on the left, right-click the __jA__ port
        under Pmod and select “connect component.” In the dialog that appears,
        find the Pmod bridge you just added and select the __Pmod_out__ port as your
        connection. One more thing, enable interrupts for the AXI GPIO.
    2. __Pmod_Dual_MAXSONAR >> jB >> Pmod MAXSONAR:__
        - Add the __Pmod_Dual_MAXSONAR__ IP and connect it to a Pmod Bridge.
        - You can connect the __Pmod_out__ port of the Pmod Bridge to the __jB__ Pmod port.
    3. __Pmod_DHB1 >> jD >> Pmod DHB1:__
        - Add the __Pmod_DHB1_v1_0__ IP and connect it to the __jD__ Pmod port through
        a Pmod Bridge.
4. Go to "Address Editor" window of the block design and assign memory addresses to the newly added IP.
5. Validate your design and generate a bitstream.

### Project Assignment

0. Watch the assembly [instruction video](https://www.youtube.com/watch?v=aJRYTqZu5OE) or follow the [instructions given here](https://learn.sparkfun.com/tutorials/assembly-guide-for-redbot-with-shadow-chassis) to assembly the chassis of your robot.
1. Mount the two IR (infrared) Proximity sensors somewhere on the front of car. Try to mount them as close as possible to the floor. Connect the sensors to one of S1, S2, S3, or S4 pin headers on the Pmod LS1 module. Finally, connect the Pmod LS1 module to the Arty board via the Pmod port you connected the AXI GPIO in the board design.
2. Mount the two Pmod MAXSONAR sensors on the front of the car. Use the provided Pmod extension cables to connect the sensors to the Pmod port where you connected the AXI GPIO in the board design. Connect one sensor to the bottom row of the Pmod port and the other to the top row of the same Pmod port.
3. Connect the Pmod DHB1 to the Arty board via the Pmod port you selected in the board design. Now make the following connections on the Pmod DHB1 module:
    1. Connect the left and right motors to the screw terminals J5 and J6.
    2. Connect the Hall effect sensors to J7 and J8. The red wire is VCC, black is GND, and white is motor feedback. The motor feedback signal is used to count wheel revolutions using the Hall effect sensors.
    3. Connect the AA battery pack to the screw terminal at J4.

### Prelab Assignment

Take a look at the manuals listed in the references sections

1. [Tang, A. ArtyBot - A two-wheel robotics platform built on the Arty development board. Hackster.io](https://www.hackster.io/atang08/artybot-551478)
2. [Pmod Interface Specification](https://digilent.com/reference/_media/reference/pmod/pmod-interface-specification-1_3_1.pdf)
3. [Pmod LS1 Reference Manual](https://digilent.com/reference/pmod/pmodls1/reference-manual)
4. [Pmod MAXSONAR Reference Manual](https://digilent.com/reference/pmod/pmodmaxsonar/reference-manual)
5. [Pmod DHB1 Reference Manual](https://digilent.com/reference/pmod/pmoddhb1/reference-manual)

---

## Week 2

### Introduction

Now that you have assembled your car, the next step in our journey is learning
how to flash programs onto FPGA boards. Doing so will cause the FPGA board to
automatically load your program from flash memory as soon as you connect the
board to a power source to the board. In order to accomplish this, we will have
to make one more modification to the SoC.

### Lab Instructions

__Vivado:__

We're going to add the SPI Flash Memory to our design. The instructions below
will walk you through the process.

1. Open your project in Vivado. In the __Flow Navigator__, navigate to __"Open Block Design"__.
2. Start by adding a port to the __"microblaze_0_axi_mem"__ interconnect.
3. Time to add the __"AXI Quad SPI"__ flash memory controller:
    1. Navigate to the __"Board"__ window.
    2. Right-click on the __"Quad SPI Flash"__ and select __"Auto Connect"__. This
    should add a new IP block to your design.
    3. Next, connect the __"AXI_LITE"__ port to the __"microblaze_0_axi_mem"__
    interconnect.
    4. Use the __"Connection Automation"__ feature to make the following connections:
        - __ext_spi_clk:__ `/mig_7series_0/ui_clk (81 MHz)`
        - __s_axi_aclk:__ `/mig_7series_0/ui_clk (81 MHz)`
        - __M01_ACLK:__ `/mig_7series_0/ui_clk (81 MHz)`
    5. Finally, assign an address to the memory controller.
4. Double-click on the __"AXI Quad Flash"__ controller to open the __"Re-customize IP"__
window:
    - Under __"IP Configuration"__, change the __"Slave Device"__ option to `Spansion`.
    - Select __"OK"__ to save the changes and close the window.
5. Validate your design, and generate a bitstream.
6. Export your bitstream. Remember, to select to __"include bitstream"__ option.

{: .info }
> Spansion is the manufacturer of the SPI flash memory chip on the FPGA board.
>
> -- [follow this link for more details](https://digilent.com/reference/programmable-logic/arty-a7/reference-manual#quad-spi_flash).

__SDK:__

1. Download [this GitHub repo](https://github.com/tkamucheka/embedded-systems-lab/tree/main), and look at the example applications in the
__"embedded_systems_lab.sdk"__ directory. The examples demonstrate how to
use the sensors and control the motors on your car.
2. Download the [Board Flashing Guide](./assets/slides/Board-Flashing-Instructions.pdf). The guide will show you how to flash how to flash any application onto your
FPGA board. You can use any application from the "Fayetteville Bike Crossing"
series to test the process.
3. Design an application capable of following the line and driving your car
around the track.
4. Top 5 fastest cars get 5 points towards final grade.

{: .hint-title}
> Hints:
>
> 1. The light sensors will pick up reflections from shiny objects. Use them to
>    detect the reflective tape on track.
> 2. The sonar sensors API function will return a distance value in inches. When
>    you test them, point them towards a large open area. Spaces with many objects
>    nearby will give you false readings.
> 3. Test the functionality of your sensor on your desk first before moving to the
>    track. This will save you a lot of time during development.
> 4. If you get stuck, ask for help.
> 5. While I recommend using FreeRTOS, you are free to design your application
>    however you see fit. Remember, there are bonus points on the line. Take them
>    by any means necessary!
>
> __Good luck!__

### Project Assignment

{: .note}
> The PDF instructions have more details and illustration images.

- Turn your car into a line following robot. The line will be a (silver) duct tape track and your car should drive around the track with wheels on either side of the track. There will be an obstacle at the end of the track, use this to stop your car. To do this:
  - Use the __GPIO PMOD LS1__ to detect when your car drives over the line and steer that car back onto the track
  - Use the __PMOD MAXSONAR__ to detect obstacles and stop the car before the obstacle.
  - Program the Arty's non-volatile 16 MB Quad-SPI flash memory with your SoC and application so that your application runs when you power on the Arty board. __NB:__ the file names in this section may be different from what you have in your project. Just remember what you named your SoC in Vivado and project name in the SDK and the rest should look familiar. If you are unsure ask your TA for help.
To program your Arty:
  - Right-click your project folder, the project you want to flash onto the board, and select __Generate Linker Script__. On the right side, change all 3 dropdowns from `microblaze_0_local`... to `mig_7series_0_memaddr`. If you are on Linux this might not work. You may need to open the __lscript.ld__ file in your project's `src` directory and manually change all the entries to `mig_7series_0_memaddr`.
  - Click the __Program Flash__ button in the toolbar. Next to Image File, click Browse and navigate to your project folder, then into Debug. Select your_project_name.elf and click open. Next to Offset, enter `0x00C00000`. This is the address offset where your program will be stored in the flash memory. Next to 'Flash Type,' select *S25FL128sxxxxxx0-spi-x1_x2_x4*. __IMPORTANT__, make sure 'Convert ELF to bootloadable SREC format and program' is checked. Click __Program__.
  - To create a bootloader, in the toolbar select File > New > Application Project. Name the project something like 'bootloader' as shown in the image above. Click __Next__.
  - Select __SREC SPI Bootloader__, and select __Finish__.
  - We need to make a few changes to our bootloader before loading it onto the Arty board. First, we need to set the 'address offset' so the bootloader can find our application. In the 'Bootloader' project, open the __blconfig.h__ file and change the __FLASH_IMAGE_BASEADDR__ to `0x00C00000`. Note, this same address you used when you flashed your application onto the flash memory earlier.
  - > __Optional__, if you wish to improve your ArtyBot's startup time. Edit the file bootloader.c and comment out the line `#define VERBOSE`. This will turn off console logging. __NB:__ Console logs may be useful when you're initially setting things up.
  - Click the __Program FPGA__ button in the toolbar. In the right half of the Software Configuration section, there is a cell that says `bootloop`. Click the right side of this cell to reveal a drop-down menu. Select __Bootloop.elf__ from the list, then click Program. This will generate a bit file with the MicroBlaze's memory initialized with the program that you have written. The Arty is also programmed and if everything went well you should see your application running in your favorite console application (e.g. SDK Terminal, Putty or TeraTerm)
  - Almost there, click the __Program Flash__ button in the toolbar. Next to Image File, click Browse and navigate to the SDK workspace, then into `base_soc_wrapper_hw_platform_0`. Select `download.bit` and click __Open__. Next of Offset, enter `0x0`. Next to Flash Type, select `S25FL128sxxxxxx0-spi-x1_x2_x4`. Click __Program__.
  - Congratulations, you have flashed the bootloader and your application to the non-volatile SPI Qflash memory. Next time you power on the board, the bootloader will be copied from the flash memory and when it runs, it will execute your application.

### Prelab Assignment

None this week. Lucky you!

### References

1. FreeRTOS Manual [https://www.freertos.org/FreeRTOS-quick-start-guide.html](https://www.freertos.org/FreeRTOS-quick-start-guide.html) (expand API Reference on the side for full documentation on function calls)
2. [Tang, A. ArtyBot - A two-wheel robotics platform built on the Arty development board. Hackster.io](https://www.hackster.io/atang08/artybot-551478)
