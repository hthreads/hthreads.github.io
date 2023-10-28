---
layout: page
title: Lab 6
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 7: FreeRTOS

## Introduction

When determining which scheduling solution your embedded system should use, there are a variety of options that range from a simplistic bare-metal design, like we used in lab 6, to a full embedded Linux distribution (called PetaLinux). Choosing one may come down to factors like:

- Responsiveness and compatibility: Companies like TI or Xilinx may create their own RTOS (e.g.: TI-RTOS or XilKernel) that work the best on the hardware they tailor it for. Some also follow POSIX standards such that maximum compatibility exists for programming between different devices (e.g.: You can create pthreads on your computer, as well as TI-RTOS and XilKernel as they are POSIX compliant, but not the standard FreeRTOS).
- System resources: Larger embedded systems may have multi-core systems that support embedded Linux or a combination of systems (PetaLinux + FreeRTOS).
- Licensing: Certain software packages like Azure RTOS and others commercially available for cloud IoT uses require licensing.

For this lab, we will be using FreeRTOS to convert our bare-metal task switcher into a full RTOS-driven system that supports preemption and has a programming feel that is closer to that of programming multi-threaded programs for computers. FreeRTOS has been described as "the de facto standard" and "market leading" because of its robustness, the wide range of microprocessorssupported, and lack of a license needed to use it. Xilinx has deprecated the use of their "XilKernel" system and have since removed it in versions of SDK > 2016. FreeRTOS provides a small, robust, and modular kernel. It is highly integrated with the Xilinx SDK and is a free software library option that can be selected when creating a new project. It allows a very high degree of customization, letting users tailor the kernel to an optimal level both in terms of size and functionality with most modifications readily available in the "FreeRTOS.h" and "FreeRTOSConfig.h" header files. It supports the core features required in a lightweight embedded kernel, however, the version included with the Xilinx 2016.4 SDK is not POSIX compliant and does not support pthreads. FreeRTOS can be used to implement higher level services (such as networking, video, and audio) and subsequently run applications using these services.
