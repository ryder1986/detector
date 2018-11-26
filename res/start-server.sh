cd ..
./server  &> /dev/stdout  | grep -v -E "frame =|conv"

