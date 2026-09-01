type display
type window
type cursor
type key_event = { keycode : int; subwindow : window option }
type motion_event = { subwindow : window option; x_root : int; y_root : int }
type window_attributes = { x : int; y : int; width : int; height : int }

type button_event = {
  subwindow : window option;
  x : int;
  y : int;
  x_root : int;
  y_root : int;
  button : int;
}

type event =
  | KeyPress of key_event
  | KeyRelease of key_event
  | ButtonPress of button_event
  | ButtonRelease of button_event
  | MotionNotify of motion_event
  | Unknown of int

external open_display : string option -> display = "caml_XOpenDisplay"

external internal_get_window_attributes :
  display -> window -> int * int * int * int = "caml_XGetWindowAttributes"

external select_input : display -> window -> int -> int = "caml_XSelectInput"
external raise_window : display -> window -> int = "caml_XRaiseWindow"

let get_window_attributes display win =
  let x, y, width, height = internal_get_window_attributes display win in
  { x; y; width; height }

external move_resize_window :
  display -> window -> int -> int -> int -> int -> int
  = "dummy" "caml_XMoveResizeWindow"

external grab_key : display -> int -> int -> window -> bool -> int -> int -> int
  = "dummy" "caml_XGrabKey"

external grab_button :
  display ->
  int ->
  int ->
  window ->
  bool ->
  int ->
  int ->
  int ->
  window option ->
  cursor option ->
  (int, int) result = "dummy" "caml_XGrabButton"

external string_to_keysym : string -> int = "caml_XStringToKeysym"
external keysym_to_keycode : display -> int -> int = "caml_XKeysymToKeycode"
external default_root_window : display -> window = "caml_DefaultRootWindow"

let key_press_mask = 1 lsl 0
let key_release_mask = 1 lsl 1

type xevent_raw

external raw_next_event : display -> xevent_raw = "caml_XNextEvent"
external raw_event_type : xevent_raw -> int = "caml_XEvent_type"
external raw_key_keycode : xevent_raw -> int = "caml_XKeyEvent_keycode"
external raw_button_x : xevent_raw -> int = "caml_XButtonEvent_x"
external raw_button_y : xevent_raw -> int = "caml_XButtonEvent_y"
external raw_subwindow : xevent_raw -> window option = "caml_XEvent_subwindow"
external raw_x_root : xevent_raw -> int = "caml_XEvent_x_root"
external raw_y_root : xevent_raw -> int = "caml_XEvent_y_root"
external raw_button : xevent_raw -> int = "caml_XEvent_button"

let next_event display =
  let raw = raw_next_event display in
  match raw_event_type raw with
  | 2 ->
      KeyPress { keycode = raw_key_keycode raw; subwindow = raw_subwindow raw }
  | 3 ->
      KeyRelease
        { keycode = raw_key_keycode raw; subwindow = raw_subwindow raw }
  | 4 ->
      ButtonPress
        {
          subwindow = raw_subwindow raw;
          x = raw_button_x raw;
          y = raw_button_y raw;
          x_root = raw_x_root raw;
          y_root = raw_y_root raw;
          button = raw_button raw;
        }
  | 5 ->
      ButtonRelease
        {
          subwindow = raw_subwindow raw;
          x = raw_button_x raw;
          y = raw_button_y raw;
          x_root = raw_x_root raw;
          y_root = raw_y_root raw;
          button = raw_button raw;
        }
  | 6 ->
      MotionNotify
        {
          subwindow = raw_subwindow raw;
          x_root = raw_x_root raw;
          y_root = raw_y_root raw;
        }
  | n -> Unknown n
