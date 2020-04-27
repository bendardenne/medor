# Medor
Medor is a time tracking tool. I've never found a time tracking tool that did exactly what I needed, so I wrote my own.

I also use it as a pretext to teach myself about some things I don't know yet (dbus, boost libraries, ...). So I'm not 
sure this can be useful for anyone else, but feel free to try it out.

## Features

* Live tracking with periodical system notifications (freedesktop.org notifications)
* Integration with git and hg: warnings whan activity happens in a non-active project
* Generation of weekly reports with daily per-project summary
* Possibility to integrate with dmenu/rofi for easy and cumberless project switching 

## Building

    git clone https://github.com/bendardenne/medor && cd medor
    mkdir build
    cd build && cmake .. 
    make
    
`medord` is a service which exposes dbus objects with various tracking methods.
`medor` is a CLI to control medord.

## TODO

### Features
* Inactivity detection: pause on sleep / suspend, possibly based on input inactivity.
* Git / Hg hooks
* Show commits in reports
* Warnings when stopped on a weekday during work hours?
* Add a optional comment field to activities?  
    - Idea 1     
              
           medor annotate "did this did that, fixed problems"
    - Idea 2 
           
           medor start Project "did this, did that"
           
    - Could be both if e.g. you want to specify later what you have just done? 
* Interface to edit / delete activities / add past activities  (GUI? let's go nuts)
* Add a way to get help about individual commands (a "help" command?).
### "Production-readiness"  (i.e. will probably never do any this)
* Verify concurrency in SQLite
* Add a manpage
* Provide .service file (and dbus policy conf?) 