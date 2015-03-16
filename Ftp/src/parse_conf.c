/*
 * @文件功能：
 * 解析配置文件；
 */
#include "parse_conf.h"
#include "common.h"
#include "strutil.h"
#include "configure.h"

void parseconf_load_setting(const char *setting);
/* 结构体数组 */
//bool类型的配置变量
static struct parseconf_bool_setting
{
  const char *p_setting_name;
  int *p_variable;
}
parseconf_bool_array[] =
{
	{ "pasv_enable", &tunable_pasv_enable },
	{ "port_enable", &tunable_port_enable },
	{ NULL, NULL }
};

static struct parseconf_uint_setting
{
	const char *p_setting_name;
	unsigned int *p_variable;
}
parseconf_uint_array[] =
{
	{ "listen_port", &tunable_listen_port },
	{ "max_clients", &tunable_max_clients },
	{ "max_per_ip", &tunable_max_per_ip },
	{ "accept_timeout", &tunable_accept_timeout },
	{ "connect_timeout", &tunable_connect_timeout },
	{ "idle_session_timeout", &tunable_idle_session_timeout },
	{ "data_connection_timeout", &tunable_data_connection_timeout },
	{ "local_umask", &tunable_local_umask },
	{ "upload_max_rate", &tunable_upload_max_rate },
	{ "download_max_rate", &tunable_download_max_rate },
	{ NULL, NULL }
};

static struct parseconf_str_setting
{
	const char *p_setting_name;
	const char **p_variable;
}
parseconf_str_array[] =
{
	{ "listen_address", &tunable_listen_address },
	{ NULL, NULL }
};


void parseconf_load_file(const char *path)
{
    /* 只读方式打开文件 */
	FILE *fp = fopen(path, "r");
	if (fp == NULL)
		ERR_EXIT("fopen");

	char setting_line[1024] = {0};
    /* 读取文件内容，一次读取一行文本； */
	while (fgets(setting_line, sizeof(setting_line), fp) != NULL)
	{
        /* #号表示解释内容不读取，空行不读取； */
		if (strlen(setting_line) == 0
			|| setting_line[0] == '#'
			|| str_all_space(setting_line))
			continue;

        /* 去除行尾的\r\n字符 */
		str_trim_crlf(setting_line);
        /* 解析文本行 */
		parseconf_load_setting(setting_line);
		memset(setting_line, 0, sizeof(setting_line));
	}

	fclose(fp);
}


/* 解析文本行命令 */
void parseconf_load_setting(const char *setting)
{
	// 去除左空格
	while (isspace(*setting))
		setting++;

	char key[128] ={0};
	char value[128] = {0};
    /* 分离=号两边的字符：左边字符保存在key，右边字符保留在value */
	str_split(setting, key, value, '=');
	if (strlen(value) == 0)
	{
		fprintf(stderr, "mising value in config file for: %s\n", key);
		exit(EXIT_FAILURE);
	}


	{
        /* 解析监听IP地址 */
		const struct parseconf_str_setting *p_str_setting = parseconf_str_array;
		while (p_str_setting->p_setting_name != NULL)
		{
			if (strcmp(key, p_str_setting->p_setting_name) == 0)
			{
				const char **p_cur_setting = p_str_setting->p_variable;
				if (*p_cur_setting)
					free((char*)*p_cur_setting);

                /* 将监听IP地址复制到由*p_cur_setting所指向的内存空间中 */
				*p_cur_setting = strdup(value);
				return;
			}

			p_str_setting++;
		}
	}

	{
        /* 解析ftp的工作模式：PORT与PASV模式 */
		const struct parseconf_bool_setting *p_bool_setting = parseconf_bool_array;
		while (p_bool_setting->p_setting_name != NULL)
		{
			if (strcmp(key, p_bool_setting->p_setting_name) == 0)
			{
                /* 将value字符转换为大写字母，即设置参数时不区分大小写 */
				str_upper(value);
                /* 若value值设置为YES、TRUE、1时，表示该工作模式可用 */
				if (strcmp(value, "YES") == 0
					|| strcmp(value, "TRUE") == 0
					|| strcmp(value, "1") == 0)
					*(p_bool_setting->p_variable) = 1;
                /* 设置为：NO、FALSE、0时，表示该工作模式不可用 */
				else if (strcmp(value, "NO") == 0
					|| strcmp(value, "FALSE") == 0
					|| strcmp(value, "0") == 0)
					*(p_bool_setting->p_variable) = 0;
                /* 设置为其他值时出错 */
				else
				{
					fprintf(stderr, "bad bool value in config file for: %s\n", key);
					exit(EXIT_FAILURE);
				}

				return;
			}

			p_bool_setting++;
		}
	}

	{
        /* 解析其他命令参数设置 */
		const struct parseconf_uint_setting *p_uint_setting = parseconf_uint_array;
		while (p_uint_setting->p_setting_name != NULL)
		{
			if (strcmp(key, p_uint_setting->p_setting_name) == 0)
			{
                /* value值参数第一个字符为0时表示八进制 */
				if (value[0] == '0')
                    /* 把八进制数转换为无符号整数 */
					*(p_uint_setting->p_variable) = str_octal_to_uint(value);
				else
                    /* 直接将字符转换为无符号整数 */
					*(p_uint_setting->p_variable) = atoi(value);

				return;
			}

			p_uint_setting++;
		}
	}
}
