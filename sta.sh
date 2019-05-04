

a=$(find . -regex '.*\.c\|.*\.h' | grep -v ^$ | xargs wc -l | tail -n 1)
a=${a% *}
