Window get_client_leader(Window window)
{
      Window client_leader = 0;
      Atom atom;
      Atom actual_type;
      int actual_format;
      unsigned long nitems;
      unsigned long bytes_after;
      unsigned char *prop = NULL;

      atom=XInternAtom(wglobal.dpy, "WM_CLIENT_LEADER", False);

      if(XGetWindowProperty(wglobal.dpy, window, atom,
                        0L, 1L, False, AnyPropertyType, &actual_type,
                        &actual_format, &nitems, &bytes_after,
                        &prop) == Success)
      {
              if(actual_type == XA_WINDOW && actual_format == 32
                                      && nitems == 1  && bytes_after == 0)
                      client_leader=*((Window *)prop);
              XFree(prop);
      }
      return client_leader;
}

char *get_window_cmd(Window window)
{
      char **argv, *command=NULL;
      int id, i, len=0, argc=0;

      if(XGetCommand(wglobal.dpy, window, &argv, &argc) && (argc > 0))
              ;
      else if((id=get_client_leader(window)))
              XGetCommand(wglobal.dpy, id, &argv, &argc);

      if(argc > 0){
              for(i=0; i < argc; i++)
                      len+=strlen(argv[i])+1;
              command=ALLOC_N(char, len+1);
              strcpy(command, argv[0]);
              for(i=1; i < argc; i++){
                      strcat(command, " ");
                      strcat(command, argv[i]);
              }
      }

      return command;
}