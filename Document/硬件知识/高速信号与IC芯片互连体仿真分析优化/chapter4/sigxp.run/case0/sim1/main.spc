*Main Circuit File
* Interconnect subcircuit statements for Board DESIGN
xDESIGN_icn_ckt 1 2 DESIGN_icn_ckt
* Component subcircuit statements
xDESIGN.OUT1 1 DESIGN.OUT1
xDESIGN.IN1 2 DESIGN.IN1
.include ./interconn.spc
.include ./comps.spc
.include ./stimulus.spc
.end
