#!/bin/bash


make


echo "sign_1.ppm test begin"
valgrind --log-fd=9 9>>valgrind_output.log ./40image -c /h/khu04/CS40/HW3/locality/images/sign_1.ppm > /h/khu04/CS40/HW3/locality/images/output1.ppm
valgrind --log-fd=9 9>>valgrind_output.log ./40image -d /h/khu04/CS40/HW3/locality/images/output1.ppm > /h/khu04/CS40/HW3/locality/images/output2.ppm

 ./ppmdiff /h/khu04/CS40/HW3/locality/images/output2.ppm /h/khu04/CS40/HW3/locality/images/sign_1.ppm

