#!/bin/bash
g++ src/main.cpp src/DesktopAssistant.cpp src/Utils.cpp src/Crypto.cpp -Iinclude/ -lcrypto -std=c++20 -o desktop
