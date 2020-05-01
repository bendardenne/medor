# Medor

[![Build Status](https://travis-ci.com/bendardenne/medor.svg?branch=master)](https://travis-ci.com/bendardenne/medor)

Medor is a time tracking tool. I've never found a time tracking tool that did exactly what I needed, so I wrote my own.

I also use it as a pretext to teach myself about some things I don't know yet (dbus, boost libraries, ...). So I'm not 
sure this can be useful for anyone else, but feel free to try it out.

## Features

* Live tracking with periodical system notifications (freedesktop.org notifications)
* Integration with git and hg: warnings when activity happens in a non-active project
* Generation of weekly reports with daily per-project summary
* Possibility to integrate with dmenu/rofi for easy and cumberless project switching 

## Building

    git clone https://github.com/bendardenne/medor
    cd medor && mkdir build && cd build
    conan install .. 
    cmake .. 
    make
    
`medord` is a service which exposes dbus objects to control the time tracker.
`medor` is a CLI to control medord.


## VCS integration

### mercurial
    # In the hg repo to be linked with project Foo
    medor repo $(hg root) Foo

    [hooks]
    commit = medor repo $(hg root)
    update = medor repo $(hg root)
    # on any hook you like

### git
Can be done per project in .git/hooks, and can be set up in the templates for each new project with for example:  

    # In the git repo to be linked with project Foo
    medor repo $(git rev-parse --show-toplevel) Foo

    git config --global init.templatedir '~/.git-templates'
    mkdir -p ~/.git-templates/hooks
    git init  # in every existing project, to add the new hook template

Then in the hooks themselves (e.g. `~/.git-templates/hooks/pre-commit`), use

    medor repo $(git rev-parse --show-toplevel)


## TODO

### Features
* CLI to show linked repo of projects
* Error handling via DBus
* Inactivity detection: pause on sleep / suspend, possibly based on input inactivity.
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