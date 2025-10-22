---
layout: page
title: People
description: A listing of all the CSDL members.
---

# Laboratory Personnel

## Professors

{% assign professors = site.staffers | where: "role", "professor" %}
{% for professor in professors %}
  {{ professor }}
{% endfor %}

## Students

{% assign GAs = site.staffers | where: "role", "Graduate Assistant" %}
{% assign RAs = site.staffers | where: "role", "Research Assistant" %}
{% assign students = GAs | concat: RAs %}
{% for student in students %}
  {{ student }}
{% endfor %}
