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
	{ 0x2caea77d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x231d1383, __VMLINUX_SYMBOL_STR(simple_statfs) },
	{ 0x177683d5, __VMLINUX_SYMBOL_STR(generic_delete_inode) },
	{ 0x9ffe4e70, __VMLINUX_SYMBOL_STR(kill_litter_super) },
	{ 0x44b262d6, __VMLINUX_SYMBOL_STR(unregister_filesystem) },
	{ 0xc0663838, __VMLINUX_SYMBOL_STR(register_filesystem) },
	{ 0xa0c4260f, __VMLINUX_SYMBOL_STR(iput) },
	{ 0x398b2e1c, __VMLINUX_SYMBOL_STR(d_make_root) },
	{ 0x65f91aef, __VMLINUX_SYMBOL_STR(simple_dir_operations) },
	{ 0x8746d3f4, __VMLINUX_SYMBOL_STR(simple_dir_inode_operations) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xb742fd7, __VMLINUX_SYMBOL_STR(simple_strtol) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xdf353996, __VMLINUX_SYMBOL_STR(dput) },
	{ 0x3a9a206c, __VMLINUX_SYMBOL_STR(d_rehash) },
	{ 0x7a2273f4, __VMLINUX_SYMBOL_STR(d_instantiate) },
	{ 0x919c8174, __VMLINUX_SYMBOL_STR(d_alloc) },
	{ 0x6f20960a, __VMLINUX_SYMBOL_STR(full_name_hash) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xe8ef0ff0, __VMLINUX_SYMBOL_STR(current_kernel_time64) },
	{ 0x2ec91852, __VMLINUX_SYMBOL_STR(new_inode) },
	{ 0x6f8722da, __VMLINUX_SYMBOL_STR(mount_bdev) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

