function _medor(){
  local line
  _arguments -C "1:command:($(medor | grep Commands | sed s/Commands://g))" \
                "*::arg:->args"

  case $line[1] in
    start)
      compadd $(medor projects)
      ;;
    quiet)
      compadd on off
      ;;
  esac
}

compdef _medor medor
