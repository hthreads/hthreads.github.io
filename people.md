---
layout: page
title: People
description: A listing of all the CSDL members.
---

# Staff

## Professors

{% assign Professors = site.staffers | where: 'role', 'Professor' %}
{% for staffer in Professors %}
{{ staffer }}
{% endfor %}

{% assign teaching_assistants = site.staffers | where: 'role', 'Teaching Assistant' %}
{% assign num_teaching_assistants = teaching_assistants | size %}
{% if num_teaching_assistants != 0 %}
## Students

{% for staffer in teaching_assistants %}
{{ staffer }}
{% endfor %}
{% endif %}
