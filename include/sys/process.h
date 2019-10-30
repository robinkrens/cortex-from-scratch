typedef struct process process_t;
struct process {
	uint32_t id;
	uint32_t * stackptr;
	uint32_t reserved;
};
