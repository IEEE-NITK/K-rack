#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xeba59c0d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x36f79f75, __VMLINUX_SYMBOL_STR(generic_delete_inode) },
	{ 0x8efb1ee3, __VMLINUX_SYMBOL_STR(__dynamic_pr_debug) },
	{ 0x1167157, __VMLINUX_SYMBOL_STR(mount_bdev) },
	{ 0xe8ef0ff0, __VMLINUX_SYMBOL_STR(current_kernel_time64) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xf4169cf4, __VMLINUX_SYMBOL_STR(simple_dir_operations) },
	{ 0xdaa23c04, __VMLINUX_SYMBOL_STR(kill_block_super) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x7ccc08b4, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0xda1c1fe2, __VMLINUX_SYMBOL_STR(d_make_root) },
	{ 0xeb9cb7fb, __VMLINUX_SYMBOL_STR(simple_statfs) },
	{ 0xec7d8df3, __VMLINUX_SYMBOL_STR(unregister_filesystem) },
	{ 0xf37bb349, __VMLINUX_SYMBOL_STR(new_inode) },
	{ 0xa3c32683, __VMLINUX_SYMBOL_STR(simple_dir_inode_operations) },
	{ 0x7c82d977, __VMLINUX_SYMBOL_STR(inode_init_owner) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

