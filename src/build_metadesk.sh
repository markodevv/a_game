disable_warnings="-Wno-write-strings -Wno-switch -Wno-unused-variable -Wno-comment -Wno-sign-compare -Wno-unused-but-set-variable -Wno-format-extra-args"
gcc -std=c++11 -Wall $disable_warnings -I ../third_party/metadesk/ ./metadesk.cpp -o metadesk -lstdc++
