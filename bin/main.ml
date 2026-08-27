open Xdromad

let () =
  let display = X11.open_display None in
  let window = X11.create_simple_window display in
  X11.map_window display window;
  X11.event_loop display
