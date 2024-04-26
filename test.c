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
void	check_permissions(char *argv[])
{
	if (argv[2][0] == '\0' || argv[3][0] == '\0')
	{
		write(2, "zsh: permission denied:\n", 24);
	}
	else if (argv[2][0] == '\0' && argv[3][0] == '\0')
	{
		write(2, "zsh: permission denied:\n", 24);
		exit(1);
	}
}
void	fun_first_child(char **argv, t_pipex *data, int i,int fd[2], char **envp, int argc)
{
	char	*final;

	//if (i == 1)
	//	dup2(data->fd_out, STDOUT_FILENO);
	if (i == argc - 4)
		{
			//close(fd[1]);
			//fd[1] = data->fd_out;
			dup2(data->fd_out, STDOUT_FILENO);
		}
	dup2(fd[1], STDOUT_FILENO);
	close(fd[0]);
	close(fd[1]);
	//dup2(data->fd_in, STDIN_FILENO);
	
	data->com_fir_child = ft_split(argv[i + 2], ' ');
	//if (open_fd_in(data, argv, argc) == 1)
	//	ft_putstr_fd(": No such file or directory\n", 2);
	if (data->com_fir_child == 0)
	{
		close(fd[0]);
		close(fd[1]);
		free_fun(data);
	}
	
	final = path_for_commands(data, data->com_fir_child, data->commands_path);
	
	if (!final)
	{
		free(data->com_sec_child);
		free(final);
		exit(1);
	}

	if (execve(final, data->com_fir_child, envp) == -1)
		free_fun(data);
	
	
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

char	*mine_path(char **argv, char **envp)
{
	if (envp == NULL || *envp == NULL)
		return (0);
	while (ft_strncmp("PATH=", *envp, 5) != 0)
	{
		envp++;
		if (*envp == NULL)
		{
			ft_putstr_fd(argv[2], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
			ft_putstr_fd(argv[3], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
			exit(0);
		}
	}
	return (*envp + 5);
}

int	open_fd_in(t_pipex *data, char **argv, int argc)
{
	if (access(argv[1], F_OK | R_OK) == -1)
	{
		perror("No access for input");
		return (1);
	}
	data->fd_in = open(argv[1], O_RDONLY);
	if (data->fd_in == -1)
	{
		perror("Error in infile");
		return (1);
	}
	data->fd_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (data->fd_out == -1)
	{
		perror("Error in outfile");
		close(data->fd_in);
		exit(1);
	}
	return (0);
}

//void	open_fd_out(t_pipex *data, char **argv, int argc)
//{
	
//}
#include <stdio.h> // to remove
int	main(int argc, char **argv, char **envp)
{
	t_pipex	data;
	char	*path;
	int fd[2];
	int *pid = NULL;
	if (argc < 5)
	{
		perror("Less the 5 argc");
		exit(1);
	}
	if (open_fd_in(&data, argv, argc) == 1)
		ft_putstr_fd(": No such file or directory\n", 2);
	path = mine_path(argv, envp);
	dup2(data.fd_in, STDIN_FILENO);
	close(data.fd_in);
	int i = -1;
	while (++i < argc - 3)
	{
		if(pipe(fd) == -1)
		{
			printf("eroor");
			exit(1);
		}
	}
	data.commands_path = ft_split(path, ':');
	if (data.commands_path == 0)
	{
		close(fd[0]);
		close(fd[1]);
		free_fun(&data);
	}
	pid = malloc(sizeof(int) * (argc - 3));
	if(!pid)
		return(0);
	i = -1;
	while (++i < argc - 3)
	{
		pid[i] = fork();
			ft_putstr_fd("1111\n", 2);
		if(pid[i] == 0)
		{
			fun_first_child(argv, &data,i, fd, envp, argc);
		}
		else if(pid[i] > 0)
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
		}
	}
	close(STDIN_FILENO);
	i = -1;
	while (++i < argc - 3)
		waitpid(pid[i], NULL, 0);
	return (0);
}
