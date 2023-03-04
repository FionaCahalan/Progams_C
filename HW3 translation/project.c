#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct node {
    char *name;
    struct node *parent;
    struct node *children;
    int depth;
};

static int count_spaces(const char *s){
    int sum = 0;
    char c;
    while((c = *s++))
        sum += c == ' ';
    return sum;
}

static struct node *find_node(struct node *subtree, char *name) {
    if(!strcmp(subtree->name, name))
        return subtree;
    subtree = subtree->children;
    if(!subtree)
        return NULL;
    while (subtree->name) {
        struct node *subtree2 = find_node(subtree, name);
        if (subtree2)
            return subtree2;
        subtree++;
    }
    return NULL;
}

static int cmpnode(const void *v1, const void *v2){
    struct node *n1 = v1;
    struct node *n2 = v2;
    return strcmp(n1->name, n2->name);
}

static void direct_super_cat(struct node *subtree){
    printf("%s ", subtree->name);
    if(subtree->parent)
        printf("%s ", subtree->parent->name);
}

static void direct_sub_cat(struct node *subtree){
    printf("%s ", subtree->name);
    struct node *child = subtree->children;
    while(child && child->name){
        printf("%s ", child->name);
        child++;
    }
}

static void all_super_cat(struct node *subtree){
    printf("%s ", subtree->name);
    while(subtree->parent && subtree->name){
        subtree = subtree->parent;
        printf("%s ", subtree->name);
    }
}

static void all_sub_cat(struct node *subtree){
    printf("%s ", subtree->name);
    struct node *child = subtree->children;
    while(child && child->name){
        all_sub_cat(child);
        child++;
    }
}

static void num_all_super_cat(struct node *subtree){
    printf("%s %d", subtree->name, subtree->depth);
}

static int count(struct node *subtree){
    int sum = 0;
    struct node *child = subtree->children;
    while(child && child->name){
        sum += 1 + count(child);
        child++;
    }
    return sum;
}

static void num_all_sub_cat(struct node *subtree){
    printf("%s %d", subtree->name, count(subtree));
}

static void is_super_cat(struct node *subtree, char *extra){
    printf("%s %s ", subtree->name, extra);
    while(subtree && subtree->name){
        if (!strcmp(subtree->name, extra)) {
            printf("yes");
            return;
        }
        subtree = subtree->parent;
    }
    printf("no");
}

static void is_sub_cat(struct node *subtree, char *extra){
    printf("%s %s ", subtree->name, extra);
    if (find_node(subtree, extra) && strcmp(subtree->name, extra))
        printf("yes");
    else
        printf("no");
}

static void common_ancestor(struct node *subtree1, struct node *subtree2){
    if(!subtree1 || !subtree2)
        return;
    printf("%s %s ", subtree1->name, subtree2->name);
    subtree1 = subtree1->parent;
    subtree2 = subtree2->parent;
    if(!subtree1 || !subtree2)
        return;
    while(strcmp(subtree1->name, subtree2->name)){
        if(!subtree1 || !subtree2)
            return;
        else if(subtree1->depth < subtree2->depth)
            subtree2 = subtree2->parent;
        else
            subtree1 = subtree1->parent;
    }
    printf("%s", subtree2->name);
}

int main(int argc, char *argv[]){
    freopen(argv[1], "r", stdin);
    struct node *root = NULL;
    
    char buf[1000];
    while(fgets(buf, sizeof buf, stdin)){  // The compiler provides me a free & on buf, it is really &buf
        if(strchr(buf, '\n'))
            strchr(buf, '\n')[0]='\0';
        if(strchr(buf, '\r'))
            strchr(buf, '\r')[0]='\0';
        int count = count_spaces(buf);
        strtok(buf, " ");
        if(!root){
            root = calloc(sizeof *root, 1);
            root->name = strdup(buf);
        }
        struct node *parent = find_node(root, buf);
        parent->children = calloc(count + 1, sizeof *parent->children);
        for (int i = 0; i < count; i++) {
            parent->children[i].name = strdup(strtok(NULL, " "));
            parent->children[i].parent = parent;
            parent->children[i].depth = parent->depth + 1;
        }
        qsort(parent->children, count, sizeof (struct node), cmpnode);
    }
    freopen(argv[2], "r", stdin);
    while(fgets(buf, sizeof buf, stdin)){
        if(strchr(buf, '\n'))
            strchr(buf, '\n')[0]='\0';
        if(strchr(buf, '\r'))
            strchr(buf, '\r')[0]='\0';
        char *query = strtok(buf, " ");
        char *category = strtok(NULL, " ");
        char *extra = strtok(NULL, " ");

        struct node *subtree = find_node(root, category);
        printf("%s ", query);
        if(!strcmp(query, "DirectSupercategory"))
            direct_super_cat(subtree);
        else if(!strcmp(query, "DirectSubcategories"))
            direct_sub_cat(subtree);
        else if(!strcmp(query, "AllSupercategories"))
            all_super_cat(subtree);
        else if(!strcmp(query, "AllSubcategories"))
            all_sub_cat(subtree);
        else if(!strcmp(query, "NumberOfAllSupercategories"))
            num_all_super_cat(subtree);
        else if(!strcmp(query, "NumberOfAllSubcategories"))
            num_all_sub_cat(subtree);
        else if(!strcmp(query, "IsSupercategory"))
            is_super_cat(subtree, extra);
        else if (!strcmp(query, "IsSubcategory"))
            is_sub_cat(subtree, extra);
        else if (!strcmp(query, "ClosestCommonSupercategory"))
            common_ancestor(subtree, find_node(root, extra));
        printf("\n");
    }
}
