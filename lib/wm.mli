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

type 'a t = X11.display -> state -> 'a * state

val return : 'a -> 'a t
(** monadic equivalent of x => [x] *)

val bind : 'a t -> ('a -> 'b t) -> 'b t
(** basically a map for monads *)

val ( let* ) : 'a t -> ('a -> 'b t) -> 'b t
val ( >>= ) : 'a t -> ('a -> 'b t) -> 'b t
val get_state : state t
val put_state : state -> unit t
val get_display : X11.display t
