blucSourceFile=src/test_basic.bluc #/src/test_basic.bluc/ # src/test_basic.bluc - on dumb windows you have to go up a level first
#
rm -f bin/bluc
# -fsanitize=address
gcc src/*.c src/jms_oop_utils/*.c src/jms_oop_utils/jms_reflection/*.c src/jms_parse/*.c src/jms_parse/jms_expressions/*.c src/jms_parse/jms_statements/*.c src/jms_tests/*.c src/jms_utils/*.c -g -O0 -o bin/bluc #-fsanitize=address
./bin/bluc $blucSourceFile