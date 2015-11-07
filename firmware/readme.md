##Directory Structure

allweaponbox : Firmware for a scoring box which supports all three weapons. 
epeebox : Firmware for a standalone epee scoring box
foilbox : Firmware for a standalone foil scoring box
sabrebox : Firmware for a standalone sabre scoring box
test_box :  Firmware for a test box, which will plug into a scoring box and test timings and touches.
old : Some older firmware revisions of boxes



##Notes:
The idea is to run the scoring box off an arduino nano with minimal circuitry
the ADCs have been shown to be fast enough and can sample at over 12ksps with
4 channels with an ADC prescaler tweak.


