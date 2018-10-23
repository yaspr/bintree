#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

//
typedef struct node_s { unsigned val; struct node_s *left; struct node_s *right; } node_t;

//
unsigned loadfile(FILE *fd, unsigned *buffer)
{
  unsigned nbelem = 0;
  
  while (fscanf(fd, "%u ", &buffer[nbelem]) != EOF && nbelem < BUFF_SIZE)
    nbelem++;

  return nbelem;
}

//
node_t *new_node(unsigned val)
{
  node_t *tmp = malloc(sizeof(node_t));
  
  tmp->val = val;
  tmp->left = tmp->right = NULL;
  
  return tmp;
}

//Avoid recursion, loops are faster for lookups
node_t *lookup_node(node_t *root, node_t *n)
{
  node_t *curr = root, *prev = NULL;
  
  if (curr)
    {
      while (curr)
	{
	  prev = curr;

	  if (n->val > curr->val)
	    curr = curr->right;
	  else
	    if (n->val < curr->val)
	      curr = curr->left;
	    else
	      prev = curr = NULL;
	}

      return prev;
    }
  else
    return NULL;
}

//
node_t *build_tree(FILE *fd)
{
  node_t *root = NULL;
  unsigned buffer[BUFF_SIZE];
  unsigned nbelem = loadfile(fd, buffer);
  
  if (nbelem)
    {
      root = new_node(buffer[0]);
    
      for (unsigned i = 1; i < nbelem; i++)
	{  
	  node_t *tmp = new_node(buffer[i]);
	  node_t *link = lookup_node(root, tmp);

	  if (link)
	    {
	      if (buffer[i] > link->val)
		link->right = tmp;
	      else
		if (buffer[i] < link->val)
		  link->left = tmp;
	    }
	  else //If value already exists 
	    free(tmp);
	}

      return root;
    }
  else
    return 0;
}

//
void print_tree(node_t *root, char c, unsigned d)
{
  if (root)
    {
      for (unsigned i = 0; i < d; i++)
	printf(" ");
      
      printf("%c(%u)\n", c, root->val);
      
      print_tree(root->left, 'L', d + 1);
      
      print_tree(root->right, 'R', d + 1);
    }
}

//
void _print_tree_dot_(FILE *fd, node_t *root)
{
  if (root && root->right)
    {
      if (root->left)
	{
	  fprintf(fd, "%u -- ", root->val);
	  
	  _print_tree_dot_(fd, root->left);
      	}
      
      fprintf(fd, "%u -- ", root->val);
            
      _print_tree_dot_(fd, root->right);
    }
  else
    fprintf(fd, "%u;\n", root->val);
}

//
void print_tree_dot(FILE *fd, node_t *root)
{
  fprintf(fd, "graph G1 {\n");
  _print_tree_dot_(fd, root);
  fprintf(fd, "}\n");
}

//
int main(int argc, char **argv)
{
  if (argc < 3)
    return printf("OUPS! %s [input file] [output dot file]\n", argv[0]), -1;
  
  FILE *fd = fopen(argv[1], "rb");
  FILE *fo = fopen(argv[2], "wb");
  
  node_t *root = build_tree(fd);

  print_tree(root, '0', 0);
  print_tree_dot(fo, root);

  fclose(fd);
  fclose(fo);
  
  return 0;
}
