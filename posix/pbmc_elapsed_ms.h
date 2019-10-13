/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined(INC_PBMC_ELAPSED_MS)
#define INC_PBMC_ELAPSED_MS

struct timespec;

int pbmc_elapsed_ms(struct timespec prev_timspec, struct timespec timspec);

#endif /* !defined(INC_PBMC_ELAPSED_MS) */
