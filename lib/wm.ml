type state = {
  dragging : bool;
  start_subwindow : X11.window option;
  start_button : int;
  start_x_root : int;
  start_y_root : int;
  attr_x : int;
  attr_y : int;
  attr_width : int;
  attr_height : int;
}

(* The monad carries the display environment (Reader) and the WM state (State) *)
type 'a t = X11.display -> state -> 'a * state

let return x : 'a t = fun _dpy s -> (x, s)

let bind m f : 'a t =
 fun dpy s ->
  let x, s' = m dpy s in
  f x dpy s'

let ( let* ) = bind
let ( >>= ) = bind

(* monadic operations to read/write state and access display *)
let get_state : state t = fun _dpy s -> (s, s)
let put_state s : unit t = fun _dpy _ -> ((), s)
let get_display : X11.display t = fun dpy s -> (dpy, s)

(* Lift an X11 IO into the WM monad *)
let lift_io f : 'a t = fun dpy s -> (f dpy, s)
