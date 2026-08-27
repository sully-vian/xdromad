type display
type window

external open_display : string option -> display = "caml_XOpenDisplay"
external create_simple_window : display -> window = "caml_XCreateSimpleWindow"
external map_window : display -> window -> unit = "caml_XMapWindow"
external event_loop : display -> unit = "caml_XEventLoop"
