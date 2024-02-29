---
layout: page
title: People
description: A listing of all the CSDL members.
---

# Laboratory Personnel

Warning: Scam Alert !!!
Someone is posing as Professors and offering online jobs to people. Apparently, their email looks very authentic but this is NOT ME. This "job offer" has nothing to do with me or the University of Arkansas. Do not respond to their emails, and do not attempt to deposit any fake checks they send you. They are simply trying to trick you out of your money.



## Professors

{% assign Professors = site.staffers | where: 'role', 'Professor' %}
{% for staffer in Professors %}
{{ staffer }}
{% endfor %}

## Students

{% assign teaching_assistants = site.staffers | where: 'role', 'Graduate Assistant' %}
{% assign num_teaching_assistants = teaching_assistants | size %}
{% if num_teaching_assistants != 0 %}

{% for staffer in teaching_assistants %}
{{ staffer }}
{% endfor %}
{% endif %}
