# Medor
Medor is a time tracking tool.

It serves two purposes:
  1. Provide me with a time tracking tool that does exactly what I want.
  2. Teach me about some useful technologies.

As a result, it's probably a bit bloated. I'm not sure anyone really needs a time tracking tool that uses dbus.
Who knows, it's a jungle out there.

It's a work in progress.

## VCS integration

Idea:   VcsTracker  DBUS object

Add a clone hook, possibly prompting to link the cloned repo with a project, maybe linking with the current active project?
Can you get user input during a hook?

Add commit/update hooks that notify the tracker that we are working on a given repo.
The tracker can then emit a notification warning if the current project does not match,
  or even automatically change the active project.

If repos are linked to projects, reports can perhaps include commit messages.
Probably need git and hg third party lib to read the commit history.



## TODO
* Warnings when stopped on a weekday during workhours?
* Figure out logging and better error handling
* Add a comment field to activities?
* Interface to edit / delete activities? Add past activities
