// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char new_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, new_command_line);
	seq_putc(m, '\n');
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/*
static void remove_flag(char *cmd, const char *flag)
{
	char *start_addr, *end_addr;

	/ * Ensure all instances of a flag are removed * /
	while ((start_addr = strstr(cmd, flag))) {
		end_addr = strchr(start_addr, ' ');
		if (end_addr)
			memmove(start_addr, end_addr + 1, strlen(end_addr));
		else
			*(max(cmd, start_addr - 1)) = '\0';
	}
}*/

static void patch_flag(char *cmd, const char *flag, const char *val)
{
	size_t flag_len, val_len;
	char *start, *end;

	start = strstr(cmd, flag);
	if (!start)
		return;

	flag_len = strlen(flag);
	val_len = strlen(val);
	end = start + flag_len + strcspn(start + flag_len, " ");
	memmove(start + flag_len + val_len, end, strlen(end) + 1);
	memcpy(start + flag_len, val, val_len);
}

static int __init proc_cmdline_init(void)
{
    strcpy(new_command_line, saved_command_line);
	patch_flag(new_command_line, "androidboot.verifiedbootstate=", "green androidboot.vbmeta.digest=3fc93a877b0bc0e92e89cde9bb1f89444ff9cfb70a3cce3d2cd8a77e0f2ecab7 androidboot.vbmeta.hash_alg=sha256 androidboot.vbmeta.size=4096 androidboot.vbmeta.avb_version=1.1");
	patch_flag(new_command_line, "buildvariant=", "user");
	//patch_flag(new_command_line, "androidboot.vbmeta.digest=","8c36e88b4b331f29ed15d3f12e39f776b8e011b2db1228d313ee9ac15549d7c4");

	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
fs_initcall(proc_cmdline_init);
