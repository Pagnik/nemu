
ori=258646
a=$(find . -regex '.*\.c\|.*\.h' | grep -v ^$ | xargs wc -l | tail -n 1)
a=${a% *}
d=$(($a-$ori))
echo 新增代码$d行