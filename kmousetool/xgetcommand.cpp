
    // try to read WM_COMMAND
    int argc;
    char **argv;
    QString command;
    ok = XGetCommand(qt_xdisplay(), win, &argv, &argc);
    
    if (0 != ok) {
	if (argc > 0 && argv != NULL) {
	    command = argv[0];
	    int i = 1;
	    while (i < argc) {
		command += " ";
		command += argv[i];
		++i;
	    }
	
	    XFreeStringList(argv);
	}
    }
    XClassHint hint;
    ok = XGetClassHint(qt_xdisplay(), win, &hint);
    
    if (0 != ok) { // we managed to read the class hint
        resName =  hint.res_name;
        resClass = hint.res_class;
    }
