/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"


struct gds_writer {
	FILE *file;
	size_t bytes_written;
};


int
gds_writer_open_file(struct gds_writer **out, const char *name, int flags) {
	FILE *file;
	struct gds_writer *gds;
	int result = GDS_OK;
	assert(name && out);

	file = fopen(name, "w");
	if (!file) {
		result = -errno;
		goto fail;
	}

	gds = malloc(sizeof(*gds));
	if (!gds) {
		result = GDS_ERR_NO_MEM;
		goto fail_file;
	}

	memset(gds, 0, sizeof(*gds));
	gds->file = file;
	*out = gds;
	return result;

fail_file:
	fclose(file); file = NULL;
fail:
	return result;
}


static void
host_to_be16(void *buffer, size_t length) {
	for (; length >= 2; length -= 2, buffer += 2) {
		uint16_t i = *(uint16_t*)buffer;
		uint8_t *o = buffer;
		o[0] = (i >> 8) & 0xFF;
		o[1] = (i >> 0) & 0xFF;
	}
}

static void
host_to_be32(void *buffer, size_t length) {
	for (; length >= 4; length -= 4, buffer += 4) {
		uint32_t i = *(uint32_t*)buffer;
		uint8_t *o = buffer;
		o[0] = (i >> 24) & 0xFF;
		o[1] = (i >> 16) & 0xFF;
		o[2] = (i >> 8)  & 0xFF;
		o[3] = (i >> 0)  & 0xFF;
	}
}

static void
host_to_be64(void *buffer, size_t length) {
	for (; length >= 8; length -= 8, buffer += 8) {
		uint64_t i = *(uint64_t*)buffer;
		uint8_t *o = buffer;
		o[0] = (i >> 56) & 0xFF;
		o[1] = (i >> 48) & 0xFF;
		o[2] = (i >> 40) & 0xFF;
		o[3] = (i >> 32) & 0xFF;
		o[4] = (i >> 24) & 0xFF;
		o[5] = (i >> 16) & 0xFF;
		o[6] = (i >> 8)  & 0xFF;
		o[7] = (i >> 0)  & 0xFF;
	}
}

int
gds_write(struct gds_writer *gds, struct gds_record *rec) {
	size_t sz, nwr;
	uint16_t dt;
	assert(gds && rec);
	void *ptr = rec;

	sz = rec->size;
	dt = rec->type & 0xFF;

	// printf("gds_write: record { size = %04x, type = %04x (%s, %s) }\n",
	// 	rec->size,
	// 	rec->type,
	// 	gds_record_type_str(rec->type),
	// 	gds_data_type_str(rec->type & 0xFF)
	// );

	host_to_be16(ptr, 4);
	if (sz > 4) {
		if (dt == GDS_DT_SIGNED16 || dt == GDS_DT_BITARRAY)
			host_to_be16(ptr+4, sz-4);
		if (dt == GDS_DT_SIGNED32 || dt == GDS_DT_REAL32)
			host_to_be32(ptr+4, sz-4);
		if (dt == GDS_DT_REAL64)
			host_to_be64(ptr+4, sz-4);
	}

	nwr = fwrite(ptr, sz, 1, gds->file);
	if (nwr != 1) {
		return -errno;
	}

	gds->bytes_written += sz;
	return GDS_OK;
}


void
gds_writer_close(struct gds_writer *gds) {
	assert(gds);
	if (gds->file) {
		/// @todo Round up to the next 2048 bytes.
		fclose(gds->file);
	}
	free(gds);
}


int
gds_write_record_hdr(gds_writer_t *gds, uint16_t sz, uint16_t rt) {
	assert(gds);

	// printf("gds_write_record_hdr: { size = %04x, type = %04x (%s, %s) }\n",
	// 	(unsigned)sz,
	// 	(unsigned)rt,
	// 	gds_record_type_str(rt),
	// 	gds_data_type_str(rt & 0xFF)
	// );

	// Create a record header stub on the stack and fill in the type and size of
	// the record.
	uint32_t buffer;
	struct gds_record *rec = (void*)&buffer;
	rec->type = rt;
	rec->size = sz;

	// Swap endianness.
	host_to_be16(rec, 4);

	// Write to disk.
	size_t nwr = fwrite(rec, 4, 1, gds->file);
	if (nwr != 1) {
		return -errno;
	}
	gds->bytes_written += 4;
	return GDS_OK;
}


int
gds_write_record_data(gds_writer_t *gds, uint16_t dt, size_t len, void *data) {
	assert(gds && len < (1 << 16) && data);
	dt &= 0xFF;

	// Convert to big-endian.
	if (dt == GDS_DT_SIGNED16 || dt == GDS_DT_BITARRAY)
		host_to_be16(data,len);
	if (dt == GDS_DT_SIGNED32 || dt == GDS_DT_REAL32)
		host_to_be32(data,len);
	if (dt == GDS_DT_REAL64)
		host_to_be64(data,len);

	// Write to disk.
	size_t nwr = fwrite(data, len, 1, gds->file);
	if (nwr != 1) {
		return -errno;
	}
	gds->bytes_written += len;
	return GDS_OK;
}


int
gds_write_void(gds_writer_t *wr, uint16_t rt) {
	return gds_write_record_hdr(wr, 4, rt);
}


int
gds_write_string(gds_writer_t *wr, uint16_t rt, const char *str) {
	int err;
	size_t len = strlen(str);
	err = gds_write_record_hdr(wr, 4 + len, rt);
	if (err != GDS_OK)
		return err;
	return gds_write_record_data(wr, rt, len, (void*)str);
}
