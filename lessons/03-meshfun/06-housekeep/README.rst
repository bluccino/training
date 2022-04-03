================================================================================
06-housekeep (using WLSTD wireless core, HWTINY hardware core and NVM memory)
================================================================================

- augmentation of sample 05-nvm by house keeping functions
- mesh application which can count system starts
- utilizes non-volatile memory (NVM)
- every system start another LED is being used
- in addition house keeping is done:
  a) quick status LED blinking if un-provisioned, slow blinking if provisioned
	b) periodic blinking of main LED and status LED off if attentioning
	c) node reset possibility based on reset counter logic
