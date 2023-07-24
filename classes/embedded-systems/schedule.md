---
layout: page
title: Schedule
parent: CSCE 4114 Embedded Systems
grand_parent: Classes
nav_order: 2
---

# Weekly Schedule

{% assign modules = site.modules | where: 'course', '4213' %}
{% for module in modules %}
{{ module }}
{% endfor %}