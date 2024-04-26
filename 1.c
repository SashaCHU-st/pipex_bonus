#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "pipex.h"
void	ft_putstr_fd(char *s, int fd)
{
	size_t	i;

	i = 0;
	if (!s)
		return ;
	while (s[i] != '\0')
	{
		write (fd, &s[i], 1);
		i++;
	}
}
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while ((s1[i] != '\0' || s2[i] != '\0') && i < n)
	{
		if (s1[i] > s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		else if (s1[i] < s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
size_t	ft_strlen(const char *str )
{
	size_t	i;
	size_t	count;

	count = 0;
	i = 0;
	while (str[i] != '\0')
	{
		i++;
		count++;
	}
	return (count);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*new;

	i = 0;
	j = 0;
	if (!s1 || !s2)
		return (NULL);
	new = (char *) malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!new)
		return (NULL);
	while (s1[i] != '\0')
	{
		new[i] = s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		new[i + j] = s2[j];
		j++;
	}
	new[i + j] = '\0';
	return (new);
}
int	ft_words(const char *str, char c)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!str)
		return (0);
	while (str[i])
	{
		if (str[i] == c)
			i++;
		else
		{
			j++;
			while (str[i] != '\0' && str[i] != c)
				i++;
		}
	}
	return (j);
}

char	*ft_wd(const char *str, char c)
{
	size_t	i;
	char	*wd;

	i = 0;
	while (*str && *str == c)
		str++;
	while (str[i] && str[i] != c)
		i++;
	wd = malloc(sizeof(char) * (i + 1));
	if (wd == NULL)
		return (NULL);
	i = 0;
	while (str[i] && str[i] != c)
	{
		wd[i] = str[i];
		i++;
	}
	wd[i] = '\0';
	return (wd);
}

void	free_w(size_t i, char **ptr)
{
	while (i > 0)
	{
		free(ptr[i - 1]);
		i--;
	}
	free(ptr);
}

char	**ft_split(char const *s, char c)
{
	char	**ptr;
	size_t	words;
	size_t	i;

	words = ft_words (s, c);
	ptr = (char **)malloc(sizeof(char *) * (words + 1));
	if (!ptr)
		return (NULL);
	i = 0;
	while (i < words)
	{
		while (*s && *s == c)
			s++;
		ptr[i] = ft_wd(s, c);
		if (ptr[i] == NULL)
		{
			free_w(i, ptr);
			return (NULL);
		}
		while (*s && *s != c)
			s++;
		i++;
	}
	ptr[i] = NULL;
	return (ptr);
}
void	free_fun(t_pipex *data)
{
	if (data->commands_path != NULL)
		free_array(data->commands_path);
	if (data->com_fir_child != NULL)
		free_array(data->com_fir_child);
	if (data->com_sec_child != NULL)
		free_array(data->com_sec_child);
	exit(1);
}

void	free_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		if (array[i] != NULL)
		{
			free(array[i]);
			array[i] = NULL;
			i++;
		}
	}
}

void	ft_error(void)
{
	perror("Error fork()");
	exit(1);
}
char	*path_for_commands(t_pipex *data, char **child_command, char **path)
{
	char	*command;
	char	*command_temp;

	while (*path)
	{
		command_temp = ft_strjoin(*path, "/");
		if (command_temp == 0)
			free_fun(data);
		command = ft_strjoin (command_temp, *child_command);
		free(command_temp);
		if (command == 0)
			free_fun(data);
		if (access(command, F_OK | X_OK) == 0)
			return (command);
		free(command);
		path++;
	}
	if (!*path)
	{
		ft_putstr_fd(*child_command, 2);
		ft_putstr_fd(": command not found\n", 2);
	}
	return (0);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "pipex.h"

int main(int argc, char **argv, char **envp) {
    int fd[2], pid1, pid2;
    t_pipex data;

    if (argc != 5) {
        fprintf(stderr, "Usage: ./pipex file1 cmd1 cmd2 file2\n");
        exit(EXIT_FAILURE);
    }

    data.fd_in = open(argv[1], O_RDONLY);
    if (data.fd_in < 0) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    data.fd_out = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (data.fd_out < 0) {
        perror("Error opening output file");
        close(data.fd_in);
        exit(EXIT_FAILURE);
    }

    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        // First child process
        close(fd[0]);  // Close unused read end of the pipe
        dup2(data.fd_in, STDIN_FILENO);  // Duplicate the file descriptor for stdin
        dup2(fd[1], STDOUT_FILENO);  // Duplicate the pipe write end to stdout
        close(data.fd_in);
        close(fd[1]);

        data.com_fir_child = ft_split(argv[2], ' ');
        if (execve(path_for_commands(&data, data.com_fir_child, data.commands_path), data.com_fir_child, envp) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }
    }

    pid2 = fork();
    if (pid2 == 0) {
        // Second child process
        close(fd[1]);  // Close unused write end of the pipe
        dup2(fd[0], STDIN_FILENO);  // Duplicate the pipe read end to stdin
        dup2(data.fd_out, STDOUT_FILENO);  // Duplicate the file descriptor for stdout
        close(data.fd_out);
        close(fd[0]);

        data.com_sec_child = ft_split(argv[3], ' ');
        if (execve(path_for_commands(&data, data.com_sec_child, data.commands_path), data.com_sec_child, envp) == -1) {
            perror("execve failed");
            exit(EXIT_FAILURE);
        }
    }

    // Close all file descriptors in parent
    close(data.fd_in);
    close(data.fd_out);
    close(fd[0]);
    close(fd[1]);

    // Wait for all children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
