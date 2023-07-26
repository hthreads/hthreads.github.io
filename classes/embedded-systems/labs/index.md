---
layout: page
title: Labs
parent: Embedded Systems
grand_parent: Classes
has_children: true
has_toc: false
---

## Lab Schedule

| Date        | Lab                                                |
| ----------- | -------------------------------------------------- |
| 8/29 - 9/2  | [Lab1: How to build a basic SoC](./lab1)           |
| 9/5  - 9/9  | [Lab2: GPIOs + Basic MicroBlaze System](./lab2)    |
| 9/12 - 9/23 | [Lab3: Fayetteville Bike Crossing](./lab3)         |
| 9/26 - 10/7 | [Lab4: Timers](./lab4)                             |
| 9/26 - 10/7 | [Lab5: Interrupt Driven Microblaze System](./lab5) |
| 9/26 - 10/7 | [Lab6: Autonomous Vehicle](./lab6)                 |


{% assign modules = site.modules | where: 'course', '4114-labs' %}
{% for module in modules %}
{{ module }}
{% endfor %}
