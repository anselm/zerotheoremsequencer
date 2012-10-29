MacOSX libcinder based movie / video sequencing tool without pauses or black frames for live sequencing of effects driven by actors on stage

Plays small scripts of the form

scene100
  play /zerotheoremshared/STAGE-2_02_v07_mouse_24fps_empty_h264.mov
  hide /zerotheoremshared/math_window_ZT_A_flow_med.mov
  move 200 300
  rotate 0
  play /zerotheoremshared/STAGE-2_02_v07_mouse_24fps_empty_h264.mov
  move 0 0
  rotate 0
  looping 1
  onmouse 0 0 2000 2000 scene200
  onkey 32 scene200
  return

scene200
  stop /zerotheoremshared/STAGE-2_02_v07_mouse_24fps_empty_h264.mov
  play /zerotheoremshared/math_window_ZT_A_flow_med.mov
  range 290 9999
  looping 1
  onmouse 0 0 2000 2000 scene300
  onkey 32 scene300
  return

scene300
  stop /zerotheoremshared/math_window_ZT_A_flow_med.mov
  onmouse 0 0 2000 2000 scene100
  onkey 32 scene100
  return

