#ifndef TEST_TOOLS_H
#define TEST_TOOLS_H

typedef struct icell {
    int data;
    struct icell* next;
} icell;

static
icell*
icons(int data, icell* next)
{
    icell* cell = xmalloc(sizeof(icell));
    cell->data = data;
    cell->next = next;
    return cell;
}

static
void
free_icell(icell* xs)
{
    if (xs != 0) {
        free_icell(xs->next);
        xfree(xs);
    }
}

typedef struct scell {
    char* data;
    struct scell* next;
} scell;

static
scell*
scons(char* data, scell* next)
{
    scell* cell = xmalloc(sizeof(scell));
    cell->data = data;
    cell->next = next;
    return cell;
}

static
void
free_scell(scell* xs)
{
    if (xs != 0) {
        free_scell(xs->next);
        xfree(xs);
    }
}

static
char*
hstrdup(const char* text)
{
    size_t len = strlen(text);
    char*  ss  = xmalloc(len + 1);
    strcpy(ss, text);
    return ss;
}

static
size_t
smax(size_t aa, size_t bb)
{
    return aa > bb ? aa : bb;
}

#endif
