# Quirks

This is a full list of the quirks and variants currently supported.

- QUIRK_F : quirks_enable_flags_reset  
  8XY1, 8XY2, and 8XY3 reset the flags register to zero

- QUIRK_I : quirks_increment_index_register  
  FX55 and FX65 increment the index register

- QUIRK_V : quirks_vblank_wait  
  Sprite drawing waits for the vblank interupt

- QUIRK_C : quirks_sprite_clipping  
  Sprite coordinates wrap, but sprites are clipped

- QUIRK_Y : quirks_shifting_ignores_y  
  8XY6 and 8XYE ignore the Y register and shift X instead

- QUIRK_J : quirks_jump_high_nubble_as_register  
  Treat the highest nibble in BNNN as the register address


VARIANT | QUIRK_F | QUIRK_I | QUIRK_V | QUIRK_C | QUIRK_Y | QUIRK_J
--------|---------|---------|---------|---------|---------|--------
`cosmac`| true    | true    | true    | true    | false   | false
