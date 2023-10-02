---
layout: page
title: Lab 6
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 6: Bare Metal Task Control Blocks (TCBs)

_--An Introduction to FreeRTOS-like Programming:_

The goal of this lab is to become familiar with the concept of pseudo-multitasking on microprocessors (which
typically only have a single core) using a stripped down version of a real-time operating system, or RTOS. You will
use the MicroBlaze system from past labs that includes timer interrupts, and a structure to organize the tasks we
want to process, called a task control block. These components form the basis for a non-preemptive round-robin
RTOS. This RTOS will be adapted for our Fayetteville bike trail crossing traffic signal to work with this paradigm.
