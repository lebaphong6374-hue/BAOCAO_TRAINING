#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x377db2aa, "trace_event_buffer_reserve" },
	{ 0x99f973b1, "trace_event_buffer_commit" },
	{ 0x53d690f1, "__trace_trigger_soft_disabled" },
	{ 0x6115548c, "trace_raw_output_prep" },
	{ 0xfa669c72, "trace_event_printf" },
	{ 0x85d120e3, "trace_handle_return" },
	{ 0xad0072f2, "bpf_trace_run1" },
	{ 0x122c3a7e, "_printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xf5f85346, "cdev_init" },
	{ 0x63597d5f, "cdev_add" },
	{ 0x1dee04a0, "class_create" },
	{ 0x714f9a77, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xcd643cea, "cdev_del" },
	{ 0x704fc943, "class_destroy" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xe8e5ec15, "device_destroy" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x2025b545, "pcpu_hot" },
	{ 0x5a5a2271, "__cpu_online_mask" },
	{ 0x688e72e1, "__SCT__preempt_schedule_notrace" },
	{ 0xbb132e6d, "trace_event_reg" },
	{ 0x879f2894, "trace_event_raw_init" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x53569707, "this_cpu_off" },
	{ 0xad5f0017, "perf_trace_buf_alloc" },
	{ 0xbb18a3be, "perf_trace_run_bpf_submit" },
	{ 0x708cd699, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "02F2734D019CA2E7C2FC490");
