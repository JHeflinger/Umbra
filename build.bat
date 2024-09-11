@echo off
call ./help.bat audit
bazel build windows_binary
