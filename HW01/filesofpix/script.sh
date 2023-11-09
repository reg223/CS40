#!/bin/bash
make 
valgrind --leak-check=full -s --track-origins=yes ./restoration ../Corrupted_PGM/yeaa.pgm
