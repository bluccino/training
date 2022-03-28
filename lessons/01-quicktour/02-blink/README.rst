================================================================================
02-blink - tiny LED blinking program
================================================================================

About the Sample App
- this sample demonstrates blinking of LED @1 with 1000ms period
- it demonstrates
  1) Bluccino initialising
  2) LED control and
  3) deal with simple timing
- the sample is prepared for nrf52dk_nrf52832, nrf52dk_nrf52840 and
 nrf52840dongle_nrf52840 (dongle)

Lessons to Learn
- how to initialize the Bluccino module (if core level functions like LED
  control, button control and/or Bluetooth mesh access is utilized)
- demonstration of simple LED control
- how to get in low power sleep mode for some intended delay time

Bluccino Primitives Used
- bluccino(): public module interface of bluccino module
- bl_init(): initialize a module
- bl_sleep(): low power sleeping for some given milliseconds
- bl_led(): LED on/off or toggle control for given LED @id

================================================================================
Exercices
================================================================================

Exercise 1
- add an initial line which initializes verbose level with 5, and prints the
  line "02-blink (tiny LED blinking program)" using bl_hello() function

Exercise 2
- Same task as in Exercise 1, except use functions bl_verbose() and bl_log() to
  generate the log output.
- Which log level has to be chosen to get exactly the same log output as in
  exercise 1?
- Use macro BL_R to switch to red text color.
- How does log output change if function bl_prt() is used instead of bl_log()

Exercise 3
- Use the code of exercise 1 for a study!
- Vary verbose level (first argument in bl_hello()) from 0 to 5 and study the
  effect of the log output

Exercise 4
- Provide a function (above the main() funtion)

    int when(BL_ob *o, int val)
    {
      bl_log(1,"when:",o,val);
    }

- In 02-blink/main.c replace the call bl_init(bluccino,NULL) by
  bl_init(bluccino,when) and study the log output
- Press one of the buttons of the board and study the log output. What might be
  the meaning of the log output?
