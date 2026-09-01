#include <X11/Xlib.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <string.h>

#define LINE() printf("%d\n", __LINE__);
#define caml_None Val_int(0)

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

CAMLprim value caml_XGetWindowAttributes(value v_display, value v_win) {
  CAMLparam2(v_display, v_win);
  CAMLlocal1(v_attr);
  Display *display = (Display *)Field(v_display, 0);
  Window win = (Window)Field(v_win, 0);

  XWindowAttributes attr;
  XGetWindowAttributes(display, win, &attr);

  /* Return a tuple: (x, y, width, height) */
  v_attr = caml_alloc(4, 0);
  Store_field(v_attr, 0, Val_int(attr.x));
  Store_field(v_attr, 1, Val_int(attr.y));
  Store_field(v_attr, 2, Val_int(attr.width));
  Store_field(v_attr, 3, Val_int(attr.height));

  CAMLreturn(v_attr);
}

CAMLprim value caml_DefaultRootWindow(value v_display) {
  CAMLparam1(v_display);
  Display *display = (Display *)Field(v_display, 0);
  Window res = DefaultRootWindow(display);
  value v_res = caml_alloc(1, Abstract_tag);
  Field(v_res, 0) = (value)res;
  CAMLreturn(v_res);
}

CAMLprim value caml_XGrabKey(value v_display, value v_keycode,
                             value v_modifiers, value v_grab_window,
                             value v_owner_events, value v_pointer_mode,
                             value v_keyboard_mode) {
  CAMLparam5(v_display, v_keycode, v_modifiers, v_grab_window, v_owner_events);
  CAMLxparam2(v_pointer_mode, v_keyboard_mode);

  Display *display = (Display *)Field(v_display, 0);
  int keycode = Int_val(v_keycode);
  unsigned int modifiers = Int_val(v_modifiers);
  Window grab_window = (Window)Field(v_grab_window, 0);
  Bool owner_events = Bool_val(v_owner_events);
  int pointer_mode = Int_val(v_pointer_mode);
  int keyboard_mode = Int_val(v_keyboard_mode);

  int status = XGrabKey(display, keycode, modifiers, grab_window, owner_events,
                        pointer_mode, keyboard_mode);
  if (status == BadAccess || status == BadValue || status == BadWindow) {
    caml_failwith("XGrabKey failed");
  }

  CAMLreturn(Val_int(status));
}

CAMLprim value caml_XGrabButton(value v_display, value v_button,
                                value v_modifiers, value v_grab_window,
                                value v_owner_events, value v_event_mask,
                                value v_pointer_mode, value v_keyboard_mode,
                                value v_confine_to, value v_cursor) {
  CAMLparam5(v_display, v_button, v_modifiers, v_grab_window, v_owner_events);
  CAMLxparam5(v_event_mask, v_pointer_mode, v_keyboard_mode, v_confine_to,
              v_cursor);

  Display *display = (Display *)Field(v_display, 0);
  unsigned int button = Int_val(v_button);
  unsigned int modifiers = Int_val(v_modifiers);
  Window grab_window = (Window)Field(v_grab_window, 0);
  Bool owner_events = Bool_val(v_owner_events);
  unsigned int event_mask = Int_val(v_event_mask);
  int pointer_mode = Int_val(v_pointer_mode);
  int keyboard_mode = Int_val(v_keyboard_mode);
  Window confine_to = (v_confine_to == caml_None)
                          ? None
                          : (Window)Field(Field(v_confine_to, 0), 0);
  Cursor cursor =
      v_cursor == caml_None ? None : (Cursor)Field(Field(v_cursor, 0), 0);

  int status =
      XGrabButton(display, button, modifiers, grab_window, owner_events,
                  event_mask, pointer_mode, keyboard_mode, confine_to, cursor);

  CAMLlocal1(v_res);
  if (status == BadAccess || status == BadValue || status == BadWindow) {
    v_res = caml_alloc(1, 1);               // size 1, tag 1
    Store_field(v_res, 0, Val_int(status)); //  Error(status)
  } else {
    v_res = caml_alloc(1, 0);               // size 1, tag 0
    Store_field(v_res, 0, Val_int(status)); // Ok(status)
  }

  CAMLreturn(v_res);
}

CAMLprim value caml_XKeysymToKeycode(value v_display, value v_keysym) {
  CAMLparam2(v_display, v_keysym);
  Display *display = (Display *)Field(v_display, 0);
  KeySym keysym = Int_val(v_keysym);
  int res = XKeysymToKeycode(display, keysym);
  CAMLreturn(Val_int(res));
}
CAMLprim value caml_XStringToKeysym(value v_string) {
  CAMLparam1(v_string);
  const char *string = String_val(v_string);
  KeySym res = XStringToKeysym(string);
  CAMLreturn(Val_int(res));
}

CAMLprim value caml_XSelectInput(value v_display, value v_win, value v_mask) {
  CAMLparam3(v_display, v_win, v_mask);
  Display *display = (Display *)Field(v_display, 0);
  Window win = (Window)Field(v_win, 0);
  long mask = Long_val(v_mask);
  int res = XSelectInput(display, win, mask);
  CAMLreturn(Val_int(res));
}

static struct custom_operations xevent_ops = {
    "xdromad.xevent",           custom_finalize_default,
    custom_compare_default,     custom_hash_default,
    custom_serialize_default,   custom_deserialize_default,
    custom_compare_ext_default, custom_fixed_length_default};

CAMLprim value caml_XNextEvent(value v_display) {
  CAMLparam1(v_display);
  Display *display = (Display *)Field(v_display, 0);

  // XNextEvent is blocking. If trying to write directly to OCaml heap, might
  // fall on GC-ed address. So, do it on the C stack and then copy to OCaml
  // heap
  XEvent event;
  XNextEvent(display, &event); // on C stack

  value v_event = caml_alloc_custom(&xevent_ops, sizeof(XEvent), 0, 1);
  memcpy(Data_custom_val(v_event), &event, sizeof(XEvent));
  CAMLreturn(v_event);
}

/* --- XEvent Getters --- */
CAMLprim value caml_XEvent_type(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->type);
}

CAMLprim value caml_XKeyEvent_keycode(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xkey.keycode);
}

CAMLprim value caml_XButtonEvent_x(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xbutton.x);
}

CAMLprim value caml_XButtonEvent_y(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xbutton.y);
}

CAMLprim value caml_X11_None(value unit) {
  value v_ = caml_alloc(1, Abstract_tag);
  Field(v_, 0) = (value)None;
  return v_;
}

CAMLprim value caml_XEvent_x_root(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xbutton.x_root);
}

CAMLprim value caml_XEvent_y_root(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xbutton.y_root);
}

CAMLprim value caml_XEvent_button(value v_event) {
  return Val_int(((XEvent *)Data_custom_val(v_event))->xbutton.button);
}

CAMLprim value caml_XRaiseWindow(value v_display, value v_win) {
  CAMLparam2(v_display, v_win);
  Display *display = (Display *)Field(v_display, 0);
  Window win = (Window)Field(v_win, 0);
  int status = XRaiseWindow(display, win);
  CAMLreturn(Val_int(status));
}

CAMLprim value caml_XMoveResizeWindow(value v_display, value v_win, value v_x,
                                      value v_y, value v_w, value v_h) {
  CAMLparam5(v_display, v_win, v_x, v_y, v_w);
  CAMLxparam1(v_h);

  Display *display = (Display *)Field(v_display, 0);
  Window win = (Window)Field(v_win, 0);
  int x = Int_val(v_x), y = Int_val(v_y);
  int w = Int_val(v_w), h = Int_val(v_h);

  int status = XMoveResizeWindow(display, win, x, y, w, h);
  CAMLreturn(Val_int(status));
}

CAMLprim value caml_XEvent_subwindow(value v_event) {
  CAMLparam1(v_event);
  CAMLlocal2(v_win, v_opt);

  XEvent *ev = (XEvent *)Data_custom_val(v_event);
  Window subwindow;

  if (ev->type == KeyPress || ev->type == KeyRelease) {
    subwindow = ev->xkey.subwindow;
  } else {
    subwindow = ev->xbutton.subwindow;
  }

  if (subwindow == None) {
    CAMLreturn(Val_int(0)); // OCaml None
  } else {
    v_win = caml_alloc(1, Abstract_tag);
    Field(v_win, 0) = (value)subwindow;

    // wrap window block in Some() block
    v_opt = caml_alloc(1, 0);
    Store_field(v_opt, 0, v_win);
    return v_opt;
  }
}
