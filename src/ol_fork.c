#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>
#include "ol_fork.h"
#include "ol_debug.h"

struct ForkSource
{
  GSource source;
  GPollFD poll_fd;
  OlForkCallback callback;
  void *data;
};

static void ol_fork_watch_fd (int fd,
                              OlForkCallback callback,
                              void *data);
static gboolean ol_fork_prepare (GSource *source,
                             gint *timeout);
static gboolean ol_fork_check (GSource *source);
static gboolean ol_fork_dispatch (GSource *source,
                                  GSourceFunc callback,
                                  gpointer user_data);
static void ol_fork_finalize (GSource *source);

static gboolean
ol_fork_prepare (GSource *source,
                 gint *timeout)
{
  ol_assert_ret (timeout != NULL, FALSE);
  *timeout = -1;
  return FALSE;
}

static gboolean
ol_fork_check (GSource *source)
{
  ol_assert_ret (source != NULL, FALSE);
  struct ForkSource *fsource = (struct ForkSource *) source;
  gint revents = fsource->poll_fd.revents;
  if (revents & (G_IO_NVAL))
  {
    g_source_remove_poll (source, &(fsource->poll_fd));
    g_source_unref (source);
    return FALSE;
  }
  if ((revents & G_IO_IN))
  {
    return TRUE;
  }
  return FALSE;
}

static gboolean ol_fork_dispatch (GSource *source,
                                  GSourceFunc callback,
                                  gpointer user_data)
{
  ol_assert_ret (source != NULL, FALSE);
  struct ForkSource *fsource = NULL;
  fsource = (struct ForkSource*) source;
  gint revents = fsource->poll_fd.revents;
  if (revents & G_IO_IN)
  {
    if (fsource->callback != NULL)
      fsource->callback (fsource->poll_fd.fd, fsource->data);
    g_source_remove_poll (source, &(fsource->poll_fd));
    g_source_unref (source);
    return TRUE;
  }
  return FALSE;
}

static void
ol_fork_finalize (GSource *source)
{ }

static void
ol_fork_watch_fd (int fd, OlForkCallback callback, void *data)
{
  ol_assert (fd > 0);
  ol_assert (callback != NULL);
  static GSourceFuncs fork_funcs = {
    ol_fork_prepare,
    ol_fork_check,
    ol_fork_dispatch,
    ol_fork_finalize,
  };
  struct ForkSource *source = NULL;
  source = (struct ForkSource*) g_source_new (&fork_funcs,
                                              sizeof (struct ForkSource));
  source->callback = callback;
  source->data = data;
  source->poll_fd.fd = fd;
  source->poll_fd.events = G_IO_IN | G_IO_HUP |
    G_IO_ERR | G_IO_NVAL;
  g_source_add_poll ((GSource *) source, &source->poll_fd);
  g_source_attach ((GSource *)source, NULL);
}

pid_t
ol_fork (OlForkCallback callback, void *data)
{
  int pipefd[2] = {0, 0};
  if (pipe (pipefd) != 0)
  {
    ol_errorf ("pipe () failed: %s\n", strerror (errno));
    return -1;
  }
  pid_t pid = 0;
  if ((pid = fork ()) == 0)     /* Child process */
  {
    close (pipefd[0]);
    dup2 (pipefd[1], STDOUT_FILENO);
    return 0;
  }
  else                          /* Parent */
  {
    close (pipefd[1]);
    ol_fork_watch_fd (pipefd[0], callback, data);
    return pid;
  }
}
