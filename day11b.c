// #define DEBUG
#include "include/mrp.c"
#include "include/mrp_scanner.c"
#include <sys/param.h>

#define STB_DS_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "include/stb_ds.h"
#pragma GCC diagnostic pop

#define BUFFER_SIZE 1024
static char BUFFER[BUFFER_SIZE];
static char BUFFER2[BUFFER_SIZE];

typedef struct {
    char *key;
    int value;
} NodeNameToID;

typedef struct {
    int key;    // from node
    int *value; // to nodes (stb_ds array)
} Adj;

typedef struct {
    NodeNameToID *node_name_to_id;
    char **node_id_to_name;
    Adj *adj_hash;
    int you_node;
    int out_node;
    int nodes_len;
} State;

static State STATE = {
    .you_node = -1,
    .out_node = -1,
};

void debug_log_node(int n)
{
    UNUSED(n);
    log_debug("Node %d (%s)", n, STATE.node_id_to_name[n]);
}

void debug_log_adj(Adj adj)
{
    debug_log_node(adj.key);
    log_debug(" connects to: ");
    for (ptrdiff_t i = 0; i < arrlen(adj.value); i++) {
        int dst = adj.value[i];
        UNUSED(dst);
        log_debug("%d (%s)", dst, STATE.node_id_to_name[dst]);
        if (i < arrlen(adj.value) - 1) {
            log_debug(", ");
        }
    }
    log_debug("\n");
}

int get_node_id(char *name)
{
    NodeNameToID *node_name_to_id = shgetp_null(STATE.node_name_to_id, name);
    if (node_name_to_id) {
        return node_name_to_id->value;
    } else {
        int new_id = (int) shlen(STATE.node_name_to_id);
        shput(STATE.node_name_to_id, name, new_id);
        arrput(STATE.node_id_to_name, shgetp_null(STATE.node_name_to_id, name)->key);
        STATE.nodes_len = (int) arrlen(STATE.node_id_to_name);
        return new_id;
    }
}

void add_connection(int from, int to)
{
    Adj *existing_adj_p = hmgetp_null(STATE.adj_hash, from);
    if (!existing_adj_p) {
        hmputs(STATE.adj_hash, ((Adj) {
            .key = from,
            .value = NULL,
        }));
        existing_adj_p = hmgetp_null(STATE.adj_hash, from);
    }
    arrput(existing_adj_p->value, to);
}

void parse_input(char *path)
{
    FILE *file = fopen_r_or_abort(path);
    log_debug("== FUNCTION: parse_input ==\n");
    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        log_debug("Parsing line: %s\n", BUFFER);
        Scanner scanner = scanner_new(BUFFER);
        size_t read = scan_until_ch(&scanner, BUFFER2, BUFFER_SIZE, ':');
        mrp_assert(read, "Could not read source node name");
        eat_char_or_die(&scanner, ':');
        eat_char_or_die(&scanner, ' ');
        int from = get_node_id(BUFFER2);
        while (scan_until_ch(&scanner, BUFFER2, BUFFER_SIZE, ' ')) {
            int to = get_node_id(BUFFER2);
            add_connection(from, to);
            eat_char(&scanner, ' ');
        }
    }
    STATE.you_node = shget(STATE.node_name_to_id, "you");
    STATE.out_node = shget(STATE.node_name_to_id, "out");
}

void debug_log_int_array(int *arr, int len)
{
    UNUSED(arr);
    log_debug("[ ");
    for (int i = 0; i < len; i++) {
        log_debug("%d ", arr[i]);
    }
    log_debug("]\n");
}

int *toposort(Adj *adj_hash, int node_count)
{
    log_debug("== FUNCTION: toposort ==\n");
    int *output_list = NULL;
    int *no_incoming = NULL;
    int *indegree = NULL; // indegree[i] == number of incoming edges to i

    ptrdiff_t adj_count = hmlen(adj_hash);

    arraddnptr(indegree, (size_t) node_count);
    memset(indegree, 0, (size_t) node_count * sizeof(int));

    // fill no_incoming list, of adjs without incoming edges
    for (ptrdiff_t i = 0; i < adj_count; i++) {
        Adj adj = adj_hash[i];
        for (ptrdiff_t j = 0; j < arrlen(adj.value); j++) {
            int target_node = adj.value[j];
            indegree[target_node] += 1;
        }
    }
    log_debug("indegree (before sort) = ");
    debug_log_int_array(indegree, (int) adj_count);

    for (ptrdiff_t i = 0; i < arrlen(indegree); i++) {
        if (indegree[i] == 0) {
            arrput(no_incoming, (int) i);
        }
    }

    while (arrlen(no_incoming) > 0) {
        int n = arrpop(no_incoming);
        arrput(output_list, n);
        Adj *adj = hmgetp_null(adj_hash, n);
        if (adj) {
            for (ptrdiff_t i = 0; i < arrlen(adj->value); i++) {
                int dest = adj->value[i];
                // remove edge, and if m has no other incoming edges then insert
                // dest into no_incoming
                if (!--indegree[dest]) {
                    arrput(no_incoming, dest);
                }
            }
        }
    }

    log_debug("indegree (after sort) = ");
    debug_log_int_array(indegree, (int) arrlen(indegree));

    log_debug("output_list = ");
    debug_log_int_array(output_list, (int) arrlen(output_list));

    return output_list;
}

// returns a plain C int array with length equal to the number of nodes, with
// each value == 1 if that node is reachable from the "you" node, 0 otherwise.
void nodes_reachable_from_node_recur(State state, int starting_node, int *reachable)
{
    reachable[starting_node] = 1;
    Adj *adj = hmgetp_null(state.adj_hash, starting_node);
    if (adj) {
        for (ptrdiff_t i = 0; i < arrlen(adj->value); i++) {
            int dest = adj->value[i];
            if (reachable[dest]) continue;
            nodes_reachable_from_node_recur(state, dest, reachable);
        }
    }
}

// returns a plain C int array with length equal to the number of nodes, with
// each value == 1 if that node is reachable from the "you" node, 0 otherwise.
int *nodes_reachable_from_node(State state, int starting_node)
{
    int *reachable = calloc((size_t) state.nodes_len, sizeof(int));
    nodes_reachable_from_node_recur(state, starting_node, reachable);
    return reachable;
}

int indexof(int *arr, int arr_len, int val)
{
    for (int i = 0; i < arr_len; i++) {
        if (arr[i] == val) {
            return i;
        }
    }
    return -1;
}

long count_paths(State state, int starting_node, int ending_node)
{
    int *sorted_nodes = toposort(state.adj_hash, (int) arrlen(state.node_id_to_name));
    int *reachability = nodes_reachable_from_node(state, starting_node);
    long *paths_memo = calloc((size_t) state.nodes_len, sizeof(long));
    memset(paths_memo, -1, (size_t) state.nodes_len * sizeof(long));

    for (int sorted_nodes_idx = state.nodes_len - 1; sorted_nodes_idx >= 0; sorted_nodes_idx--) {
        int from_node = sorted_nodes[sorted_nodes_idx];
        if (!reachability[from_node]) {
            paths_memo[from_node] = 0;
        } else if (from_node == ending_node) {
            paths_memo[from_node] = 1;
        } else {
            int paths = 0;
            Adj *adj = hmgetp_null(state.adj_hash, from_node);
            if (adj) {
                for (size_t conn_idx = 0; conn_idx < (size_t) arrlen(adj->value); conn_idx++) {
                    int to_node = adj->value[conn_idx];
                    assert(paths_memo[to_node] >= 0);
                    paths += paths_memo[to_node];
                }
            }
            paths_memo[from_node] = paths;
        }
    }
    return paths_memo[starting_node];
}

void print_graphviz(State state)
{
    printf("digraph G {\n");
    for (ptrdiff_t i = 0; i < hmlen(state.adj_hash); i++) {
        Adj adj = state.adj_hash[i];
        for (ptrdiff_t j = 0; j < arrlen(adj.value); j++) {
            int to = adj.value[j];
            printf("    \"%s\" -> \"%s\";\n",
                   state.node_id_to_name[adj.key],
                   state.node_id_to_name[to]);
        }
    }
    printf("}\n");
}

int main(int argc, char **argv)
{
    sh_new_strdup(STATE.node_name_to_id);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    parse_input(input_file);

    if (argc >= 3 && strcmp(argv[2], "--gv") == 0) {
        print_graphviz(STATE);
        return 0;
    }

    log_debug("== ADJACENCY ==\n");
    for (ptrdiff_t i = 0; i < hmlen(STATE.adj_hash); i++) {
        Adj adj = STATE.adj_hash[i];
        debug_log_adj(adj);
    }

    int *sorted_nodes = toposort(STATE.adj_hash, (int) arrlen(STATE.node_id_to_name));
    int svr_idx = indexof(sorted_nodes, (int) arrlen(sorted_nodes), get_node_id("svr"));
    int dac_idx = indexof(sorted_nodes, (int) arrlen(sorted_nodes), get_node_id("dac"));
    int fft_idx = indexof(sorted_nodes, (int) arrlen(sorted_nodes), get_node_id("fft"));
    log_debug("svr_idx=%d, dac_idx=%d, fft_idx=%d\n", svr_idx, dac_idx, fft_idx);

    int first_stop = sorted_nodes[MIN(dac_idx, fft_idx)];
    int second_stop = sorted_nodes[MAX(dac_idx, fft_idx)];

    long first_stop_paths = count_paths(STATE, sorted_nodes[svr_idx], first_stop);
    log_debug("Paths from svr to first stop (%s): %ld\n",
              STATE.node_id_to_name[first_stop], first_stop_paths);

    long second_stop_paths = count_paths(STATE, first_stop, second_stop);
    log_debug("Paths from first stop (%s) to second stop (%s): %ld\n",
              STATE.node_id_to_name[first_stop],
              STATE.node_id_to_name[second_stop],
              second_stop_paths);

    long final_paths = count_paths(STATE, second_stop, STATE.out_node);
    log_debug("Paths from second stop (%s) to out: %ld\n",
              STATE.node_id_to_name[second_stop], final_paths);

    long total_paths = first_stop_paths * second_stop_paths * final_paths;
    printf("%ld\n", total_paths);

    return 0;
}
