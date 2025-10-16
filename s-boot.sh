 riscv64-unknown-elf-g++ -T link_s.ld startup.S main.cc -o main -march=rv64imac -mabi=lp64 -fPIC -nostdlib -nostartfiles -mcmodel=medany
 qemu-system-riscv64 -nographic -machine virt -kernel main