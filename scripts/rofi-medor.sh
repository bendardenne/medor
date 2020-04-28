#!/usr/bin/env bash

theme="~/Repos/rofi-scripts/launchers-ribbon/mortecouille_watson.rasi"
prompt=""

rofi_() {
   rofi -theme $theme -dmenu -p $prompt -i "$@"
}

active_project=$(medor status | sed "s/No project started//g")
active_project_icon="$active_project\0icon\x1femblem-ok"

#Remove active project from list, if present. We'll put it first in the list .
list=$(medor projects | sed "/^$active_project\$/ d")

if [[ $active_project != "" ]]; then
  choice=$(echo -e "$active_project_icon\n$list\nStop" | rofi_ -a 0)
else
  choice=$(echo -e "$list" | rofi_ )
fi

if [[ $choice == "Stop" ]]; then
  medor stop
else
  medor start $choice
fi
