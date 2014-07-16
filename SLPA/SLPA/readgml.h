#include "network.h"
#include "datastruct.h"

int read_network(NETWORK *network, FILE *stream);

int read_network_bn(NETWORK *network, FILE *stream);

int read_communities(RESULT_VERTEX *comp, FILE *stream);

void free_network(NETWORK *network);
