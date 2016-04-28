/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include "util.h"


struct gds_reader {
	int error;
	FILE *file;
	void *buffer;
};


int
gds_reader_open_file(gds_reader_t **out, const char *name, int flags) {
	FILE *file;
	void *buffer;
	gds_reader_t *gds;
	int result = GDS_OK;
	assert(name && out);

	file = fopen(name, "r");
	if (!file) {
		result = -errno;
		goto fail;
	}

	buffer = malloc(1<<16);
	if (!buffer) {
		result = GDS_ERR_NO_MEM;
		goto fail_file;
	}

	gds = malloc(sizeof(*gds));
	if (!gds) {
		result = GDS_ERR_NO_MEM;
		goto fail_buffer;
	}

	memset(gds, 0, sizeof(*gds));
	gds->file = file;
	gds->buffer = buffer;
	*out = gds;
	return result;

fail_buffer:
	free(buffer); buffer = NULL;
fail_file:
	fclose(file); file = NULL;
fail:
	return result;
}


void
gds_reader_close(gds_reader_t *gds) {
	assert(gds);
	if (gds->file) {
		fclose(gds->file);
	}
	if (gds->buffer) {
		free(gds->buffer);
	}
	free(gds);
}


int
gds_read(gds_reader_t *gds, gds_record_t **out) {
	gds_record_t *rec = gds->buffer;
	size_t nread;
	uint16_t dt;

	assert(gds && gds->buffer && out);

	nread = fread(gds->buffer, 4, 1, gds->file);
	if (nread != 1) {
		*out = NULL;
		return feof(gds->file) ? GDS_OK : -errno;
	}
	be16_to_host(gds->buffer, 4);
	dt = rec->type & 0xFF;

	if (rec->size > 4) {
		nread = fread(gds->buffer+4, rec->size-4, 1, gds->file);
		if (nread != 1) {
			*out = NULL;
			return GDS_ERR_UNEXP_EOF;
		}
		if (dt == GDS_DT_SIGNED16 || dt == GDS_DT_BITARRAY)
			be16_to_host(gds->buffer+4, rec->size-4);
		if (dt == GDS_DT_SIGNED32 || dt == GDS_DT_REAL32)
			be32_to_host(gds->buffer+4, rec->size-4);
		if (dt == GDS_DT_REAL64)
			be64_to_host(gds->buffer+4, rec->size-4);
	}

	*out = rec;
	return GDS_OK;
}
