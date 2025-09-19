---
layout: page
title: People
description: A listing of all the CSDL members.
---

# Laboratory Personnel



## Professors

{% assign Professors = site.staffers | where: 'role', 'Professor' %}
{% for staffer in Professors %}
{{ staffer }}
{% endfor %}

## Students

{% assign assistants = site.staffers | where_exp: "assistant", "assistant.role == 'Graduate Assistant' or assistant.role == 'Research Assistant'" %}
{% assign num_assistants = assistants | size %}
{% if num_assistants != 0 %}

{% for staffer in teaching_assistants %}
{{ staffer }}
{% endfor %}

{% endif %}
