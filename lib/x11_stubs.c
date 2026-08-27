#include <X11/Xlib.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>

CAMLprim value caml_XOpenDisplay(value v_display_name) {
  CAMLparam1(v_display_name);
  const char *display_name = NULL;

  if (v_display_name != Val_int(0)) { // None is 0
    // Some(x) is a block where Field(foo,0) is x
    display_name = String_val(Field(v_display_name, 0));
  }

  Display *res = XOpenDisplay(display_name);

  if (res == NULL) {
    caml_failwith("XOpenDisplay failed");
  }

  /* Wrap pointer safely for OCaml */
  value v_res = caml_alloc(1, Abstract_tag);
  Field(v_res, 0) = (value)res;

  CAMLreturn(v_res);
}

CAMLprim value caml_XCreateSimpleWindow(value v_display) {
  CAMLparam1(v_display);
  Display *display = (Display *)Field(v_display, 0);
  int screen = DefaultScreen(display);

  Window win = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10,
                                   800, 600, 1, BlackPixel(display, screen),
                                   WhitePixel(display, screen));

  /* Wrap the Window (an integer/XID) for OCaml */
  value v_win = caml_alloc(1, Abstract_tag);
  Field(v_win, 0) = (value)win;

  CAMLreturn(v_win);
}

CAMLprim value caml_XMapWindow(value v_display, value v_win) {
  CAMLparam2(v_display, v_win);
  Display *display = (Display *)Field(v_display, 0);
  Window win = (Window)Field(v_win, 0);

  XMapWindow(display, win);
  XFlush(display); // Ensure the server processes the map request

  CAMLreturn(Val_unit);
}

CAMLprim value caml_XEventLoop(value v_display) {
  CAMLparam1(v_display);
  Display *display = (Display *)Field(v_display, 0);
  XEvent event;

  /* Block and wait for events indefinitely */
  while (1) {
    XNextEvent(display, &event);
  }

  CAMLreturn(Val_unit);
}
