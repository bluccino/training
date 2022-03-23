#===============================================================================
# 02-blink - tiny LED blinking program
#===============================================================================

- this sample demonstrates blinking of LED @1 with 1000ms period
- the sample is a 4-liner (the two body statements of the endless for loop
 are comma-separated
- the sample is prepared for nrf52dk_nrf52832, nrf52dk_nrf52840 and
 nrf52840dongle_nrf52840 (dongle)

#===============================================================================
# Exercices
#===============================================================================

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
- What might the meaning of the log output be?
- Press one of the buttons of the board and study the log output. What might be
  the meaning of the log output?
