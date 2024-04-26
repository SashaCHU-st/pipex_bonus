/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aheinane <aheinane@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 10:28:39 by aheinane          #+#    #+#             */
/*   Updated: 2024/04/26 09:47:26 by aheinane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
	pid = malloc(sizeof(int) * (argc - 3));
	if(!pid)
		return(0);
	i = -1;
	while (++i < argc - 3)
	{
		pid[i] = fork();
		if (i == argc - 4)
		{
			//close(fd[1]);
			//fd[1] = data.fd_out;
			dup2(data.fd_out, STDOUT_FILENO);
		}
		if(pid[i] == 0)
		{
			fun_first_child(argv, &data,i, fd, envp);
		}
		else if(pid[i] > 0)
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
		}
	}
	if (data.commands_path == 0)
	{
		close(fd[0]);
		close(fd[1]);
		free_fun(&data);
	}
	//open_fd_out(&data, argv, argc);
	i = -1;
	while (++i < argc - 3)
		waitpid(pid[i], NULL, 0);
	return (0);
}
