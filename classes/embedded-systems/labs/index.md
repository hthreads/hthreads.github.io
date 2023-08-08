---
layout: page
title: Labs
parent: CSCE 4114 Embedded Systems
grand_parent: Classes
has_children: true
nav_order: 3
has_toc: false
nav_exclude: false
---

# Lab Repository

## At A Glance

| Date        | Lab                                                 |
| ----------- | --------------------------------------------------- |
| 8/21 - 8/25 | Prelab: download [Vivado 2019.1](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html) on home computer |
| 8/29 - 9/2  | [Lab 1: How to build a basic SoC](./lab1)           |
| 9/5  - 9/9  | [Lab 2: GPIOs + Basic MicroBlaze System](./lab2)    |
| 9/12 - 9/23 | [Lab 3: Fayetteville Bike Crossing](./lab3)         |
| 9/26 - 10/7 | [Lab 4: Timers](./lab4)                             |
| 9/26 - 10/7 | [Lab 5: Interrupt Driven Microblaze System](./lab5) |
| 9/26 - 10/7 | [Lab 6: Autonomous Vehicle](./lab6)                 |

## Lab Schedule

{% assign modules = site.modules | where: 'course', '4114-labs' %}
{% for module in modules %}
{{ module }}
{% endfor %}
