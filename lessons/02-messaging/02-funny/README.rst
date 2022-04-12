================================================================================
02-funny - basic Bluccino messaging with Bluccino objects (stage 1)
================================================================================

App Description
- An LED module supports a public API with two functions led_toggle() and
  led_set()
- main() starts to execute function clock(), which invokes 10 calls of
  led_toggle() (from the public LED module's API)
- after finishing clock() => main() executes function sos(), which, guided by
  an SOS pattern string invokes led_set(1,true) or led_set(1,false) calls

Lessons to Learn
- ...

Bluccino Primitives Used
- bl_hello(): setup verbose (logging) level and print a hello message

================================================================================
Exercices
================================================================================

Exercise 1
- ...
