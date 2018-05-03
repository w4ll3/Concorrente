for dest in $(<$1); do
  scp obj/$2 ${dest}:~
done