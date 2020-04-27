# Medor
Medor is a time tracking tool.

It serves two purposes:
  1. Provide me with a time tracking tool that does exactly what I want.
  2. Teach me about some useful technologies.

As a result, it's probably a bit bloated. I'm not sure anyone really needs a time tracking tool that uses dbus.
Who knows, it's a jungle out there.

It's a work in progress.

## TODO
* Inactivity detection: pause on sleep / suspend, possibly based on input inactivity.
* Git / Hg hooks and CLI for hinting
* Show commits in reports
* Verify concurrency in SQLite
* Warnings when stopped on a weekday during work hours?
* Add a optional comment field to activities?  
    - Idea 1     
              
           medor annotate "did this did that, fixed problems"
    - Idea 2 
           
           medor start AIA "did this, did that"
           
    - Could be both if e.g. you want to specify later what you have just done? 
* Interface to edit / delete activities / add past activities
