from pwn import *

elf = ELF("/home/sciver/Vagrant/ubuntu2004/vmlinux-5.4.0-128-generic")
base_addr = elf.symbols["ftrace_rec_iter_start"]
# mod_find = elf.symbols["mod_find"]
ftrace_list_end = elf.symbols["ftrace_list_end"]
ftrace_ops_list = elf.symbols["ftrace_ops_list"]
print("base_addr: 0x%x" % base_addr)
print("ftrace_list_end: 0x%x" % ftrace_list_end)
print("ftrace_ops_list: 0x%x" % ftrace_ops_list)
# print("mod_find: 0x%x" % mod_find)
print("ftrace_list_end_offset: 0x%x" % (ftrace_list_end - base_addr))
print("ftrace_ops_list_offset: 0x%x" % (ftrace_ops_list - base_addr))
# print("mod_find_offset: 0x%x" % (mod_find - base_addr))