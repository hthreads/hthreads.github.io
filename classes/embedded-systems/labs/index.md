---
layout: page
title: Labs
parent: EECS 4114 Embedded Systems
grand_parent: Classes
has_children: true
nav_order: 3
has_toc: false
nav_exclude: false
---

# Lab Repository

## Lab Schedule

| Date          | Lab                                                 |
| ------------- | --------------------------------------------------- |
| 08/21 - 08/25 | Prelab: download [Vivado 2019.1](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html) on home computer |
| 09/04 - 09/08 | [Lab 1: How to build a basic SoC](./lab1)           |
| 09/11 - 09/15 | [Lab 2: GPIOs + Basic MicroBlaze System](./lab2)    |
| 09/18 - 09/22 | [Lab 3: Fayetteville Bike Crossing](./lab3)         |
| 09/25 - 09/29 | [Lab 4: Timers](./lab4)                             |
| 10/02 - 10/06 | [Lab 5: Interrupt Driven Microblaze System](./lab5) |
| 10/23 - 10/27 | [Lab 6: Bare Metal Task Control Blocks (TCBs)](./lab6) |
| 10/30 - 11/10 | [Lab 7: Autonomous Vehicle](./lab7)                 |

## Lab Reports

You are required to complete a lab report after each lab. The report along with any code files from that lab will be due on Friday of the following week.

[Lab Report Template](./assets/report_template.docx)

## Grading

A rubric will be attached to each assignment on Blackboard. Make sure you read the rubric before you submit your assignment.

{% comment %}
## Lab Schedule
{% assign modules = site.modules | where: 'course', '4114-labs' %}
{% for module in modules %}
{{ module }}
{% endfor %}
{% endcomment %}
