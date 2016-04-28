/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include "util.h"


struct gds_elem {
	uint32_t kind;
	uint8_t layer;
	uint8_t type;
	uint8_t num_xy;
	uint16_t num_col, num_row;
	// ELFLAGS
	// PLEX
	char *sname; /* allocated with struct */
	uint16_t strans;
	double mag, angle;
	gds_xy_t *xy; /* allocated with struct */
	/// @todo Add properties.
};

// struct gds_elem_path {
// 	gds_elem_t elem;
// };


// static int path_write(struct gds_elem_path*, gds_writer_t*);
// static int path_read(struct gds_elem_path*, gds_writer_t*);
// static int path_dispose(struct gds_elem_path*);


// static const struct gds_elem_vtbl {
// 	int (*write)(gds_elem_t*, gds_writer_t*);
// 	int (*read)(gds_elem_t*, gds_reader_t*);
// 	void (*dispose)(gds_elem_t*);
// } vtbl[] = {
// 	[GDS_ELEM_PATH] = {
// 		.write   = (void*)path_write,
// 		.read    = (void*)path_read,
// 		.dispose = (void*)path_dispose,
// 	},
// };


static void
skip_unused(gds_parser_t *parser) {
	if (gds_parser_accept(parser, GDS_RT_ELFLAGS))
		gds_parser_next(parser);
	if (gds_parser_accept(parser, GDS_RT_PLEX))
		gds_parser_next(parser);
}


static void
parse_layer_and_type(gds_parser_t *parser, uint16_t *layer, uint16_t rt, uint16_t *type) {
	assert(parser && layer && type);
	gds_parser_next(parser); // element type record
	if (gds_parser_require(parser, GDS_RT_LAYER)) {
		*layer = gds_parser_get_record(parser)->data.layer;
		gds_parser_next(parser);
	}
	if (gds_parser_require(parser, rt)) {
		*type = gds_parser_get_record(parser)->data.layer;
		gds_parser_next(parser);
	}
}


bool
gds_elem_read(gds_elem_t **out, gds_parser_t *parser) {
	gds_record_t *rec;
	gds_elem_t *elem = NULL;
	uint16_t layer, type;
	assert(out && parser);

	// BOUNDARY
	if (gds_parser_accept(parser, GDS_RT_BOUNDARY)) {
		skip_unused(parser);
		parse_layer_and_type(parser, &layer, GDS_RT_DATATYPE, &type);

		// XY
		if (!gds_parser_require(parser, GDS_RT_XY))
			return false;
		rec = gds_parser_get_record(parser);
		elem = gds_elem_create_boundary(layer, type, (rec->size-4)/8, rec->data.xy);
		gds_parser_next(parser);
	}

	// PATH
	else if (gds_parser_accept(parser, GDS_RT_PATH)) {
		skip_unused(parser);
		parse_layer_and_type(parser, &layer, GDS_RT_DATATYPE, &type);

		// PATHTYPE
		if (gds_parser_accept(parser, GDS_RT_PATHTYPE)) {
			gds_parser_next(parser);
			/// @todo Store PATHTYPE.
		}

		// WIDTH
		if (gds_parser_accept(parser, GDS_RT_WIDTH)) {
			gds_parser_next(parser);
			/// @todo Store WIDTH.
		}

		// XY
		if (!gds_parser_skip_until(parser, GDS_RT_XY))
			return false;
		rec = gds_parser_get_record(parser);
		elem = gds_elem_create_path(layer, type, (rec->size-4)/8, rec->data.xy);
		gds_parser_next(parser);

	}

	// SREF
	// AREF

	// TEXT
	else if (gds_parser_accept(parser, GDS_RT_TEXT)) {
		skip_unused(parser);
		gds_parser_skip_until(parser, GDS_RT_ENDEL);
	}

	// TEXT
	// NODE
	// BOX

	else {
		return false;
	}

	if (gds_parser_require(parser, GDS_RT_ENDEL)) {
		gds_parser_next(parser);
		*out = elem;
		return true;
	} else {
		if (elem) gds_elem_destroy(elem);
		return false;
	}
}


int
gds_elem_write(gds_elem_t *elem, gds_writer_t *wr) {
	int err;
	assert(elem && wr);

	// BOUNDARY | PATH | SREF | AREF | TEXT | NODE | BOX
	static const uint16_t rectypes[] = {
		[GDS_ELEM_BOUNDARY] = GDS_RT_BOUNDARY,
		[GDS_ELEM_PATH]     = GDS_RT_PATH,
		[GDS_ELEM_SREF]     = GDS_RT_SREF,
		[GDS_ELEM_AREF]     = GDS_RT_AREF,
		[GDS_ELEM_TEXT]     = GDS_RT_TEXT,
		[GDS_ELEM_NODE]     = GDS_RT_NODE,
		[GDS_ELEM_BOX]      = GDS_RT_BOX,
	};
	err = gds_write_void(wr, rectypes[elem->kind]);
	if (err != GDS_OK)
		return err;

	// LAYER
	if (elem->kind == GDS_ELEM_BOUNDARY ||
		elem->kind == GDS_ELEM_PATH) {
		char buffer[6];
		struct gds_record *rec = (void*)buffer;
		rec->size = 6;
		rec->type = GDS_RT_LAYER;
		rec->data.layer = elem->layer;
		err = gds_write(wr, rec);
		if (err != GDS_OK)
			return err;
	}

	// DATATYPE
	if (elem->kind == GDS_ELEM_BOUNDARY ||
		elem->kind == GDS_ELEM_PATH) {
		char buffer[6];
		struct gds_record *rec = (void*)buffer;
		rec->size = 6;
		rec->type = GDS_RT_DATATYPE;
		rec->data.datatype = elem->type;
		err = gds_write(wr, rec);
		if (err != GDS_OK)
			return err;
	}

	// SNAME
	if (elem->kind == GDS_ELEM_SREF ||
		elem->kind == GDS_ELEM_AREF) {
		err = gds_write_string(wr, GDS_RT_SNAME, elem->sname);
		if (err != GDS_OK)
			return err;
	}

	// <strans>
	if (elem->kind == GDS_ELEM_SREF ||
		elem->kind == GDS_ELEM_AREF ||
		elem->kind == GDS_ELEM_TEXT) {
		char buffer[12];
		struct gds_record *rec = (void*)buffer;

		// STRANS
		if (elem->strans != 0) {
			rec->size = 6;
			rec->type = GDS_RT_STRANS;
			rec->data.strans = elem->strans;
			err = gds_write(wr, rec);
			if (err != GDS_OK)
				return err;
		}

		// MAG
		if (elem->mag != 1) {
			rec->size = 12;
			rec->type = GDS_RT_MAG;
			rec->data.mag = gds_double_to_real64(elem->mag);
			err = gds_write(wr, rec);
			if (err != GDS_OK)
				return err;
		}

		// ANGLE
		if (elem->angle != 0) {
			rec->size = 12;
			rec->type = GDS_RT_ANGLE;
			rec->data.mag = gds_double_to_real64(elem->angle);
			err = gds_write(wr, rec);
			if (err != GDS_OK)
				return err;
		}
	}

	// COLROW
	if (elem->kind == GDS_ELEM_AREF) {
		char buffer[8];
		struct gds_record *rec = (void*)buffer;
		rec->size = 8;
		rec->type = GDS_RT_COLROW;
		rec->data.colrow.c = elem->num_col;
		rec->data.colrow.r = elem->num_row;
		err = gds_write(wr, rec);
		if (err != GDS_OK)
			return err;
	}

	// XY
	err = gds_write_record_hdr(wr, 4 + elem->num_xy * 8, GDS_RT_XY);
	if (err != GDS_OK)
		return err;
	err = gds_write_record_data(wr, GDS_RT_XY, elem->num_xy * 8, elem->xy);
	if (err != GDS_OK)
		return err;

	// ENDEL
	err = gds_write_void(wr, GDS_RT_ENDEL);
	if (err != GDS_OK)
		return err;

	return GDS_OK;
}


static gds_elem_t *
gds_elem_create(enum gds_elem_kind kind, size_t num_xy, gds_xy_t *xy, size_t add) {
	size_t off_add = sizeof(gds_elem_t);
	size_t off_xy = off_add + add;
	size_t off_end = off_xy + num_xy * sizeof(gds_xy_t);

	void *ptr = calloc(1, off_end);
	gds_elem_t *elem = ptr;
	elem->kind = kind;
	elem->num_xy = num_xy;
	elem->mag = 1;
	elem->xy = num_xy > 0 ? ptr + off_xy : NULL;
	if (num_xy) {
		assert(xy);
		memcpy(elem->xy, xy, num_xy * sizeof(gds_xy_t));
	}

	return elem;
}


gds_elem_t *
gds_elem_create_boundary(uint8_t layer, uint8_t type, uint8_t num_xy, gds_xy_t *xy) {
	assert(num_xy >= 4 && "Boundary requires at least 4 XY coordinates");
	assert(memcmp(xy, xy+num_xy-1, sizeof(*xy)) == 0 && "First and last XY coordinates of boundary must coincide");
	gds_elem_t *elem = gds_elem_create(GDS_ELEM_BOUNDARY, num_xy, xy, 0);
	elem->layer = layer;
	elem->type = type;
	return elem;
}


gds_elem_t *
gds_elem_create_path(uint8_t layer, uint8_t type, uint8_t num_xy, gds_xy_t *xy) {
	assert(num_xy >= 2 && "Path requires at least 2 XY coordinates");
	gds_elem_t *elem = gds_elem_create(GDS_ELEM_PATH, num_xy, xy, 0);
	elem->layer = layer;
	elem->type = type;
	return elem;
}


gds_elem_t *
gds_elem_create_sref(const char *sname, gds_xy_t xy) {
	assert(sname);
	size_t len = strlen(sname)+1;
	gds_elem_t *elem = gds_elem_create(GDS_ELEM_SREF, 1, &xy, len);
	elem->sname = (void*)elem + sizeof(gds_elem_t);
	memcpy(elem->sname, sname, len);
	return elem;
}


gds_elem_t *
gds_elem_create_aref(const char *sname, uint16_t n_col, uint16_t n_row, gds_xy_t p_ref, gds_xy_t p_maxcol, gds_xy_t p_maxrow) {
	assert(sname && n_col <= 32767 && n_row <= 32767);
	size_t len = strlen(sname)+1;
	gds_elem_t *elem = gds_elem_create(GDS_ELEM_AREF, 3, (gds_xy_t[]){p_ref, p_maxcol, p_maxrow}, len);
	elem->sname = (void*)elem + sizeof(gds_elem_t);
	elem->num_col = n_col;
	elem->num_row = n_row;
	memcpy(elem->sname, sname, len);
	return elem;
}


void
gds_elem_destroy(gds_elem_t *elem) {
	assert(elem);
	free(elem);
}


int
gds_elem_get_kind(gds_elem_t *elem) {
	assert(elem);
	return elem->kind;
}


uint16_t
gds_elem_get_layer(gds_elem_t *elem) {
	assert(elem);
	return elem->layer;
}


uint16_t
gds_elem_get_type(gds_elem_t *elem) {
	assert(elem);
	return elem->type;
}


void
gds_elem_set_strans(gds_elem_t *elem, gds_strans_t strans) {
	assert(elem);
	elem->strans = strans.flags;
	elem->mag    = strans.mag;
	elem->angle  = strans.angle;
}


gds_strans_t
gds_elem_get_strans(gds_elem_t *elem) {
	assert(elem);
	return (gds_strans_t){
		.flags = elem->strans,
		.mag   = elem->mag,
		.angle = elem->angle,
	};
}


gds_xy_t *
gds_elem_get_xy(gds_elem_t *elem) {
	assert(elem);
	return elem->xy;
}


uint16_t
gds_elem_get_num_xy(gds_elem_t *elem) {
	assert(elem);
	return elem->num_xy;
}
