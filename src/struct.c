/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include "util.h"


struct gds_struct {
	uint32_t refcount;
	char *name;
	ptrset_t elems;
};


bool
gds_struct_read(gds_struct_t **out, gds_parser_t *parser) {
	gds_struct_t *str = NULL;
	assert(out && parser);

	// BGNSTR
	if (!gds_parser_accept(parser, GDS_RT_BGNSTR))
		return 0;
	gds_parser_next(parser);

	// STRNAME
	if (gds_parser_require(parser, GDS_RT_STRNAME)) {
		char *name = gds_parser_copy_string(parser);
		str = gds_struct_create(name);
		free(name);
		gds_parser_next(parser);
	}

	// STRCLASS
	if (gds_parser_accept(parser, GDS_RT_STRCLASS)) {
		gds_parser_next(parser);
		/// @todo Implement STRCLASS
	}

	// {<element>}*
	gds_elem_t *elem;
	while (gds_elem_read(&elem, parser)) {
		if (elem)
			gds_struct_add_elem(str, elem);
	}

	// ENDSTR
	gds_parser_require(parser, GDS_RT_ENDSTR);
	gds_parser_next(parser);

	int err = gds_parser_get_error(parser);
	if (err == GDS_OK) {
		*out = str;
		return true;
	} else {
		if (str)
			gds_struct_unref(str);
		return false;
	}
}


int
gds_struct_write(gds_struct_t *str, gds_writer_t *wr) {
	assert(str && wr);
	int err;
	char buffer[128];
	struct gds_record *R = (void*)buffer;

	// BGNSTR
	memset(R, 0, 28);
	R->size = 28;
	R->type = GDS_RT_BGNSTR;
	err = gds_write(wr, R);
	if (err != GDS_OK)
		return err;

	// STRNAME
	size_t len = strlen(str->name);
	err = gds_write_record_hdr(wr, 4 + len, GDS_RT_STRNAME);
	if (err != GDS_OK)
		return err;
	err = gds_write_record_data(wr, GDS_RT_STRNAME, len, str->name);
	if (err != GDS_OK)
		return err;

	// {<element>}*
	for (size_t z = 0; z < str->elems.size; ++z) {
		err = gds_elem_write(str->elems.items[z], wr);
		if (err != GDS_OK)
			return err;
	}

	// ENDSTR
	err = gds_write_void(wr, GDS_RT_ENDSTR);
	if (err != GDS_OK)
		return err;

	return GDS_OK;
}


gds_struct_t *
gds_struct_create(const char *name) {
	assert(name);
	assert(strlen(name) <= 32);
	gds_struct_t *str = calloc(1, sizeof(*str));
	str->refcount = 1;
	str->name = gds_dupstr(name);
	ptrset_init(&str->elems);
	return str;
}


static void
gds_struct_destroy(gds_struct_t *str) {
	assert(str);
	for (size_t z = 0; z < str->elems.size; ++z)
		gds_elem_destroy(str->elems.items[z]);
	free(str->name);
	ptrset_dispose(&str->elems);
	free(str);
}


void
gds_struct_ref(gds_struct_t *str) {
	assert(str && str->refcount > 0);
	++str->refcount;
}


void
gds_struct_unref(gds_struct_t *str) {
	assert(str && str->refcount > 0);
	if (--str->refcount == 0)
		gds_struct_destroy(str);
}


const char *
gds_struct_get_name(gds_struct_t *str) {
	assert(str);
	return str->name;
}


void
gds_struct_add_elem(gds_struct_t *str, gds_elem_t *elem) {
	assert(str && elem);
	ptrset_add(&str->elems, elem);
}


void
gds_struct_remove_elem(gds_struct_t *str, gds_elem_t *elem) {
	assert(str && elem);
	ptrset_remove(&str->elems, elem);
}


size_t
gds_struct_get_num_elems(gds_struct_t *str) {
	assert(str);
	return str->elems.size;
}


gds_elem_t *
gds_struct_get_elem(gds_struct_t *str, size_t idx) {
	assert(str && idx < str->elems.size);
	return str->elems.items[idx];
}
