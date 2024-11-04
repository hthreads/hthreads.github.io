---
layout: page
title: Labs
parent: Embedded Systems
grand_parent: Classes
has_children: true
nav_order: 3
has_toc: false
nav_exclude: false
---

# Lab Repository

![Arty Bot](./assets/images/bot.gif)

## Lab Schedule

| Date          | Lab                                                 |
| ------------- | --------------------------------------------------- |
| 08/19 - 08/23 | Prelab: Download [Vivado 2024.1](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2024-1.html) on home computer |
|               | [Vivado 2019.1 Installation Guide](./assets/guides/VivadoGuide2019_1.pdf) |
|               | [Vivado 2024.1 Installation Guide (Preferred)](./assets/guides/VivadoGuide2024_1.pdf) |
| 08/26 - 08/30 | [Lab 1: How to build a basic SoC](./lab1)           |
| 09/02 - 09/06 | [Lab 2: GPIOs + Basic MicroBlaze System](./lab2)    |
| 09/09 - 09/13 | [Lab 3: Fayetteville Bike Crossing](./lab3)         |
| 09/16 - 09/20 | [Lab 4: Timers](./lab4)                             |
| 09/23 - 10/04 | [Lab 5: Interrupt Driven Microblaze: System](./lab5) |
| 10/21 - 10/25 | [Lab 6: Bare Metal Task Control Blocks (TCBs)](./lab6) |
| 10/28 - 11/01 | [Lab 7: FreeRTOS](./lab7)                           |
| 11/03 - 11/15 | [Lab 8: Autonomous Vehicle](./lab8)                 |

## Lab Reports

You are required to complete a lab report after each lab. The report along with any code files from that lab will be due on Friday of the following week.

[Lab Report Template](./assets/guides/report_template.docx)

## Grading

A rubric will be attached to each assignment on Blackboard. Make sure you read the rubric before you submit your assignment.

{% comment %}
## Lab Schedule
{% assign modules = site.modules | where: 'course', '4114-labs' %}
{% for module in modules %}
{{ module }}
{% endfor %}
{% endcomment %}
