check() {
    if [ "$1" != "$2" ]; then
        echo "Test failed: expected \"$2\", got \"$1\""
        exit 1
    fi
}

check "$(echo "Hello World" | ./egg "-ua\n")"      "HELLO WORLD"
check "$(echo "Hello World" | ./egg "-la\n")"      "hello world"
check "$(echo "hello world" | ./egg "-Ca\n")"      "Hello World"
check "$(echo "hello world" | ./egg "-ra\n")"      "dlrow olleh"
check "$(echo "hello world" | ./egg "-da\n")"      "hello worldhello world"
check "$(echo "a b c" | ./egg "-sa\n")"            "abc"
check "$(echo "  test  " | ./egg "-ta\n")"         "test"
check "$(echo "hello world" | ./egg "-ja\n")"      "hello_world"
check "$(echo "hello world" | ./egg "ea\n")"       "hello world\n"
check "$(echo "hello world" | ./egg "-ea\n")"      "hello world"
check "$(echo "hello " | ./egg "-ea'world\n'")"    "hello world"
check "$(echo "hello" | ./egg "-Eda\n")"           "hheelllloo"
check "$(echo "Hello" | ./egg "-E(dd-)a\n")"       "HHHeeellllllooo"
check "$(echo "hello, world!" | egg "c")"          "b631dfc0"
check "$(echo "hello, world!" | egg "xl")"         "heo, word!"
check "$(echo "LeetCode" | egg leetify.eggscript)" "1337C0d3"
