#define FUSE_USE_VERSION 26
#include <fuse.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

static char* rootdir = NULL;
unsigned int max_sleep = 5000;
unsigned int min_sleep = 2000;

unsigned char debug = 0;

static void delay() {
  if (max_sleep == min_sleep)
    usleep(min_sleep*1000);
  else
    usleep(((rand() % (max_sleep-min_sleep)) + min_sleep) * 1000);
};

static void fullpath(char fpath[PATH_MAX], const char* path) {
  strcpy(fpath, rootdir);
  strncat(fpath, path, PATH_MAX);
};

static int slowpokefs_access(const char *path, int mask) {
  if (debug)
    fprintf(stderr, "access(%s, %d);\n", path, mask);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = access(fpath, mask);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_getattr(const char *path, struct stat *stbuf) {
  if (debug)
    fprintf(stderr, "getattr(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = lstat(fpath, stbuf);
  if (res != 0)
    return -errno;
  return res;
};

static int slowpokefs_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "fgetattr(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = fstat(fi->fh, stbuf);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_opendir(const char *path, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "opendir(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  DIR *dir = opendir(fpath);
  if (!dir)
    return -errno;
  fi->fh = (unsigned long) dir;
  return 0;
};

static int slowpokefs_releasedir(const char *path, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "closedir(%s);\n", path);
  delay();
  DIR *dir = (DIR*) fi->fh;
  closedir(dir);
  return 0;
};

static int slowpokefs_readdir(const char *path, void *buf, fuse_fill_dir_t filler
                             ,off_t offset, struct fuse_file_info *fi) {
  if (debug)
   fprintf(stderr, "readdir(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  DIR *dir = opendir(fpath);
  struct dirent *de;
  if (!dir)
    return -errno;
  while ((de = readdir(dir)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    if (filler(buf, de->d_name, &st, 0))
      break;
  }
  closedir(dir);
  return 0;
};

static int slowpokefs_open(const char *path, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "open(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int fd = open(fpath, fi->flags);
  if (fd < 0)
    return -errno;
  fi->fh = fd;
  return 0;
};

static int slowpokefs_read(const char *path, char *buf, size_t size
                          ,off_t offset, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "read(%s, size = %ld, offset = %ld);\n", path, size, offset);
  delay();
  int res = pread(fi->fh, buf, size, offset);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_write(const char *path, const char *buf, size_t size
                           ,off_t offset, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "write(%s, size = %ld, offset = %ld);\n", path, size, offset);
  delay();
  int res = pwrite(fi->fh, buf, size, offset);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_mkdir(const char *path, mode_t m) {
  if (debug)
    fprintf(stderr, "mkdir(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = mkdir(fpath, m);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_rmdir(const char *path) {
  if (debug)
    fprintf(stderr, "rmdir(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = rmdir(path);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_create(const char *path, mode_t m, struct fuse_file_info *fi) {
  if (debug)
    fprintf(stderr, "creat(%s, %d);\n", path, m);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int fd = creat(fpath, m);
  if (fd < 0)
    return -errno;
  fi->fh = fd;
  return 0;
};

static int slowpokefs_mknod(const char *path, mode_t m, dev_t d) {
  if (debug)
    fprintf(stderr, "mknod(%s, %d, %ld);\n", path, m, d);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  return mknod(fpath, m, d);
};

static int slowpokefs_readlink(const char *path, char *link, size_t size) {
  if (debug)
    fprintf(stderr, "readlink(%s, %s, %zd);\n", path, link, size);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = readlink(fpath, link, size - 1);
  if (res < 0)
    return -errno;
  link[res] = '\0';
  return 0;
};

static int slowpokefs_unlink(const char *path) {
  if (debug)
    fprintf(stderr, "unlink(%s);\n", path);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = unlink(fpath);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_rename(const char *src, const char *dst) {
  if (debug)
    fprintf(stderr, "rename(%s, %s);\n", src, dst);
  delay();
  char srcpath[PATH_MAX];
  char dstpath[PATH_MAX];
  fullpath(srcpath, src);
  fullpath(dstpath, dst);
  int res = rename(srcpath, dstpath);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_truncate(const char *path, off_t o) {
  if (debug)
    fprintf(stderr, "truncate(%s, %ld);\n", path, o);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = truncate(path, o);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_symlink(const char *path, const char *link) {
  if (debug)
    fprintf(stderr, "symlink(%s, %s);\n", path, link);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, link);
  int res = symlink(path, fpath);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_chmod(const char *path, mode_t m) {
  if (debug)
    fprintf(stderr, "chmod(%s, %d);\n", path, m);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = chmod(fpath, m);
  if (res < 0)
    return -errno;
  return res;
};

static int slowpokefs_chown(const char *path, uid_t u, gid_t g) {
  if (debug)
    fprintf(stderr, "chown(%s, %d, %d);\n", path, u, g);
  delay();
  char fpath[PATH_MAX];
  fullpath(fpath, path);
  int res = chown(fpath, u, g);
  if (res < 0)
    return -errno;
  return res;
};

void usage() {
  printf("USAGE: slowpokefs -F [actual folder] [mount point]\n");
  printf("-h, --help\tThis help.\n");
  printf("-m\t\tMinimum sleep time in milliseconds [defaults to 2000]\n");
  printf("-M\t\tMaximum sleep time in milliseconds [defaults to 5000]\n");
  exit(0);
};

//#define ASCII_SLOWPOKE /* Uncomment this define to get rid of the big ascii slowpoke */

void version() {
  printf("Slowpokefs - %s\n", VERSION);
#ifndef ASCII_SLOWPOKE
  printf("                                  _.---\"'\"\"\"\"\"'`--.._\n");
  printf("                             _,.-'                   `-._\n");
  printf("                         _,.\"                            -.\n");
  printf("                     .-\"\"   ___...---------.._             `.\n");
  printf("                     `---'\"\"                  `-.            `.\n");
  printf("                                                 `.            \\\n");
  printf("                                                   `.           \\\n");
  printf("                                                     \\           \\\n");
  printf("                                                      .           \\\n");
  printf("                                                      |            .\n");
  printf("                                                      |            |\n");
  printf("                                _________             |            |\n");
  printf("                          _,.-'\"         `\"'-.._      :            |\n");
  printf("                      _,-'                      `-._.'             |\n");
  printf("                   _.'                              `.             '\n");
  printf("        _.-.    _,+......__                           `.          .\n");
  printf("      .'    `-\"'           `\"-.,-\"\"--._                 \\        /\n");
  printf("     /    ,'                  |    __  \\                 \\      /\n");
  printf("    `   ..                       +\"  )  \\                 \\    /\n");
  printf("     `.'  \\          ,-\"`-..    |       |                  \\  /\n");
  printf("      / \" |        .'       \\   '.    _.'                   .'\n");
  printf("     |,..\"--\"\"\"--..|    \"    |    `\"\"`.                     |\n");
  printf("   ,\"               `-._     |        |                     |\n");
  printf(" .'                     `-._+         |                     |\n");
  printf("/                           `.                        /     |\n");
  printf("|    `     '                  |                      /      |\n");
  printf("`-.....--.__                  |              |      /       |\n");
  printf("   `./ \"| / `-.........--.-   '              |    ,'        '\n");
  printf("     /| ||        `.'  ,'   .'               |_,-+         /\n");
  printf("    / ' '.`.        _,'   ,'     `.          |   '   _,.. /\n");
  printf("   /   `.  `\"'\"'\"\"'\"   _,^--------\"`.        |    `.'_  _/\n");
  printf("  /... _.`:.________,.'              `._,.-..|        \"'\n");
  printf(" `.__.'                                 `._  /\n");
  printf("                                           \"'\n");
#endif
  exit(0);
};

static int slowpokefs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
  char buf[strlen(arg)];
  unsigned int tmp;
  switch (key) {
  case 0:
    usage();
  case 1:
    if (sscanf(arg, "-F%s", buf) == 1) {
      if (buf[strlen(buf)-1] == '/')
        buf[strlen(buf)-1] = '\0';
      if (buf[0] != '/') {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        strncat(cwd, "/", PATH_MAX);
        strncat(cwd, buf, PATH_MAX);
        rootdir = strdup(cwd);
      } else
        rootdir = strdup(buf);
    }
    return 0;
  case 2:
    if (sscanf(arg, "-M%d", &tmp) == 1)
      max_sleep = tmp;
    return 0;
  case 3:
    if (sscanf(arg, "-m%d", &tmp) == 1) {
      if (tmp > max_sleep) {
        fprintf(stderr, "Your specified minimum sleep time is larger than the max sleep time..\n");
        exit(1);
      }
      min_sleep = tmp;
    }
    return 0;
  case 4:
    version();
  case 5:
    debug = 1;
    return 0;
  }
  return 1;
};

static struct fuse_opt slowpokefs_opts[] = {
  FUSE_OPT_KEY("-h", 0),
  FUSE_OPT_KEY("--help", 0),
  FUSE_OPT_KEY("-F ", 1),
  FUSE_OPT_KEY("-M ", 2),
  FUSE_OPT_KEY("-m ", 3),
  FUSE_OPT_KEY("-v", 4),
  FUSE_OPT_KEY("--version", 4),
  FUSE_OPT_KEY("-D", 5),
  FUSE_OPT_END
};

static struct fuse_operations slowpokefs_oper = {
  .access = slowpokefs_access,
  .getattr = slowpokefs_getattr,
  .fgetattr = slowpokefs_fgetattr,
  .opendir = slowpokefs_opendir,
  .readdir = slowpokefs_readdir,
  .releasedir = slowpokefs_releasedir,
  .open = slowpokefs_open,
  .read = slowpokefs_read,
  .write = slowpokefs_write,
  .create = slowpokefs_create,
  .mknod = slowpokefs_mknod,
  .mkdir = slowpokefs_mkdir,
  .rmdir = slowpokefs_rmdir,
  .unlink = slowpokefs_unlink,
  .readlink = slowpokefs_readlink,
  .truncate = slowpokefs_truncate,
  .rename = slowpokefs_rename,
  .symlink = slowpokefs_symlink,
  .chmod = slowpokefs_chmod,
  .chown = slowpokefs_chown
};

int main(int argc, char** argv) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  char* mountpoint;
  int multithreaded;
  int foreground;
  if (fuse_opt_parse(&args, NULL, slowpokefs_opts, slowpokefs_opt_proc) == -1)
    return 1;
  if (!rootdir) {
    fprintf(stderr, "You didn't specify a real folder..\n\n");
    usage();
  }
  if (fuse_parse_cmdline(&args, &mountpoint, &multithreaded, &foreground) == -1)
    return 1;
  struct fuse_chan *ch = fuse_mount(mountpoint, &args);
  if (!ch)
    return 1;
  struct fuse *fuse = fuse_new(ch, &args, &slowpokefs_oper, sizeof(struct fuse_operations), NULL);
  if (!fuse) {
    fuse_unmount(mountpoint, ch);
    return 1;
  }
  if (debug)
    foreground = 1;
  if (fuse_daemonize(foreground) != -1) {
    if (fuse_set_signal_handlers(fuse_get_session(fuse)) == -1) {
      fuse_unmount(mountpoint, ch);
      fuse_destroy(fuse);
      return 1;
    }
  }
  if (multithreaded)
    return fuse_loop_mt(fuse);
  else
    return fuse_loop(fuse);
};
