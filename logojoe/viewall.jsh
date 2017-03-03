
curDir := !newInstance "java.io.File",".".
list := curDir list.
!foreach list,{:entry.
   !if (entry endsWith ".joe"), {
      !runJoe entry.
   }.
}.
