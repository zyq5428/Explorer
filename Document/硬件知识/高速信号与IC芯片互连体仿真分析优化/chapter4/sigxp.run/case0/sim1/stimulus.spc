
.subckt DESIGN.IN1.1_stim 5 4 3
v_enable_off 5 3 0
v_input_inactive 4 3 1
.ends DESIGN.IN1.1_stim

*** Printing Custom Stimuli ***

.subckt DESIGN.OUT1.1_stim 5 4 3
v_5 5 3 1
v_4 4 3 PULSE (0 1 0 3.6e-10 3.6e-10 4.964e-08 1e-07)

.ends DESIGN.OUT1.1_stim
