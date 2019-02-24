# kvkbd

Virtual Keyboard based on Qt

## This project has been forked from [kvkbd](https://gitlab.com/KDE/kvkbd),
and all the KDE specific classes have been replaced with pure Qt classes.

### Known issues:
- Does not compile with Qt5. See [issue 1](https://github.com/marcusbritanicus/QVKbd/issues/1)
- The `INSTALL` file is outdated.
- The mod keys(ctrl, alt, alt gr, pg up, pg down, ...) may not work if you're
not using Xserver >= 1.5 with evdev enabled due to changes in the keys mapping.
Hopefully, new releases of Kvkbd will detect which mapping is used and behave
accordingly.
- The Loginhelper mode(see manual) does not work currently with KDM, you can either
apply the patch for KDM available here: <http://smarter.free.fr/kvkbd/kdm.html>,
wait for your distribution or for KDE to include the patch, or use XDM.
