open Xdromad
open Wm

let mod1mask = 1 lsl 3
let grab_mode_async = 1
let button_press_mask = 1 lsl 2
let button_release_mask = 1 lsl 3
let pointer_motion_mask = 1 lsl 6

let handle_button_press (b : X11.button_event) =
  let* dpy = get_display in
  match b.subwindow with
  | Some win ->
      let attr = X11.get_window_attributes dpy win in
      put_state
        {
          dragging = true;
          start_subwindow = Some win;
          start_button = b.button;
          start_x_root = b.x_root;
          start_y_root = b.y_root;
          attr_x = attr.x;
          attr_y = attr.y;
          attr_width = attr.width;
          attr_height = attr.height;
        }
  | None -> return ()

let handle_motion (m : X11.motion_event) =
  let* state = get_state in
  let* dpy = get_display in
  match (state.start_subwindow, m.subwindow) with
  | Some win, Some _ when state.dragging ->
      let xdiff = m.x_root - state.start_x_root in
      let ydiff = m.y_root - state.start_y_root in

      let new_x = state.attr_x + if state.start_button = 1 then xdiff else 0 in
      let new_y = state.attr_y + if state.start_button = 1 then ydiff else 0 in
      let new_w =
        max 1 (state.attr_width + if state.start_button = 3 then xdiff else 0)
      in
      let new_h =
        max 1 (state.attr_height + if state.start_button = 3 then ydiff else 0)
      in
      (* Lift the FFI side-effect into the monad *)
      let _ = X11.move_resize_window dpy win new_x new_y new_w new_h in
      return ()
  | _ -> return ()

let rec loop () =
  let* dpy = get_display in
  let ev = X11.next_event dpy in
  let* () =
    match ev with
    | X11.KeyPress k ->
        let _ = Option.map (X11.raise_window dpy) k.subwindow in
        return ()
    | X11.ButtonPress b -> handle_button_press b
    | X11.MotionNotify m -> handle_motion m
    | X11.ButtonRelease _ ->
        let* state = get_state in
        put_state { state with dragging = false; start_subwindow = None }
    | _ -> return ()
  in
  loop ()

let () =
  let dpy = X11.open_display None in
  let f1_keysym = X11.string_to_keysym "F1" in
  let f1_keycode = X11.keysym_to_keycode dpy f1_keysym in
  let default_root_window = X11.default_root_window dpy in

  let _ =
    X11.grab_key dpy f1_keycode mod1mask default_root_window true
      grab_mode_async grab_mode_async
  in

  let mask =
    button_press_mask lor button_release_mask lor pointer_motion_mask
  in

  (* Grab Button 1 for Moving *)
  let _ =
    X11.grab_button dpy 1 mod1mask default_root_window true mask grab_mode_async
      grab_mode_async None None
  in

  (* Grab Button 3 for Resizing *)
  let _ =
    X11.grab_button dpy 3 mod1mask default_root_window true mask grab_mode_async
      grab_mode_async None None
  in

  let initial_state =
    {
      dragging = false;
      start_subwindow = None;
      start_button = 0;
      start_x_root = 0;
      start_y_root = 0;
      attr_x = 0;
      attr_y = 0;
      attr_width = 0;
      attr_height = 0;
    }
  in

  let _final_result, _final_state = loop () dpy initial_state in
  ()
