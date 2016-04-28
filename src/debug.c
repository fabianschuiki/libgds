/* Copyright (c) 2016 Fabian Schuiki */
#include <gds.h>
// #include <cairo.h>
// #include <cairo-svg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


// struct gds_structure {
// 	struct gds_record *bgnstr, *strname, *strclass, *endstr;
// 	struct gds_element *elements;
// };

// enum element_kind {
// 	ELEMENT_BOUNDARY = 1,
// 	ELEMENT_PATH,
// 	ELEMENT_SREF,
// 	ELEMENT_AREF,
// 	ELEMENT_TEXT,
// 	ELEMENT_NODE,
// 	ELEMENT_BOX,
// };

// struct structure {
// 	char *name;
// 	struct element **elements;
// 	unsigned num_elements;
// 	unsigned refs;
// };

// struct element {
// 	enum element_kind kind;
// 	int16_t elflags;
// 	int32_t plex;
// 	int16_t layer;
// 	union {
// 		struct {
// 			int16_t datatype;
// 		} boundary;
// 		struct {
// 			int16_t datatype;
// 			int16_t pathtype;
// 			int16_t width;
// 		} path;
// 		struct {
// 			char *sname;
// 			struct structure *ptr;
// 			int16_t strans;
// 			gds_real64_t mag;
// 			gds_real64_t angle;
// 		} sref;
// 		struct {
// 			char *sname;
// 			struct structure *ptr;
// 			int16_t strans;
// 			int16_t cols;
// 			int16_t rows;
// 		} aref;
// 		struct {
// 			int16_t texttype;
// 			int16_t presentation;
// 			int16_t pathtype;
// 			int16_t width;
// 			int16_t strans;
// 			char *string;
// 		} text;
// 		struct {
// 			int16_t boxtype;
// 		} box;
// 	};
// 	struct gds_xy *xy;
// 	unsigned num_xy;
// };

// static struct element *process_element(struct gds_reader *gds, struct gds_record *rec) {
// 	struct element *el;
// 	uint16_t type = rec->type;

// 	el = malloc(sizeof(*el));
// 	assert(el);
// 	memset(el, 0, sizeof(*el));

// 	rec = gds_read(gds);

// 	if (rec->type == GDS_RT_ELFLAGS) {
// 		el->elflags = rec->elflags;
// 		rec = gds_read(gds);
// 	}

// 	if (rec->type == GDS_RT_PLEX) {
// 		el->plex = rec->plex;
// 		rec = gds_read(gds);
// 	}

// 	if (rec->type == GDS_RT_LAYER) {
// 		el->layer = rec->layer;
// 		rec = gds_read(gds);
// 	}

// 	if (type == GDS_RT_BOUNDARY) {
// 		el->kind = ELEMENT_BOUNDARY;

// 		if (rec->type == GDS_RT_DATATYPE) {
// 			el->boundary.datatype = rec->datatype;
// 			rec = gds_read(gds);
// 		}

// 	} else if (type == GDS_RT_PATH) {
// 		el->kind = ELEMENT_PATH;

// 		if (rec->type == GDS_RT_DATATYPE) {
// 			el->path.datatype = rec->datatype;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_PATHTYPE) {
// 			el->path.pathtype = rec->pathtype;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_WIDTH) {
// 			el->path.width = rec->width;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_BGNEXTN) {
// 			while ((rec = gds_read(gds)) && rec->type != GDS_RT_ENDEXTN);
// 			rec = gds_read(gds);
// 		}

// 	} else if (type == GDS_RT_SREF || type == GDS_RT_AREF) {
// 		el->kind = (type == GDS_RT_SREF ? ELEMENT_SREF : ELEMENT_AREF);

// 		assert(rec->type == GDS_RT_SNAME);
// 		el->sref.sname = strndup(rec->sname, rec->size-4);
// 		rec = gds_read(gds);

// 		if (rec->type == GDS_RT_STRANS) {
// 			el->sref.strans = rec->strans;
// 			rec = gds_read(gds);
// 		}

// 	} else if (type == GDS_RT_TEXT) {
// 		el->kind = ELEMENT_TEXT;

// 		assert(rec->type == GDS_RT_TEXTTYPE);
// 		rec = gds_read(gds);

// 		if (rec->type == GDS_RT_PRESENTATION) {
// 			el->text.presentation = rec->presentation;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_PATHTYPE) {
// 			el->text.pathtype = rec->pathtype;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_WIDTH) {
// 			el->text.width = rec->width;
// 			rec = gds_read(gds);
// 		}

// 		if (rec->type == GDS_RT_STRANS) {
// 			el->text.strans = rec->strans;
// 			rec = gds_read(gds);

// 			if (rec->type == GDS_RT_MAG)
// 				rec = gds_read(gds);

// 			if (rec->type == GDS_RT_ANGLE)
// 				rec = gds_read(gds);
// 		}

// 	} else {
// 		printf("element type '%s' not supported\n", gds_record_type_str(type));
// 	}

// 	if (rec->type == GDS_RT_XY) {
// 		el->num_xy = (rec->size-4)/8;
// 		el->xy = malloc(rec->size-4);
// 		assert(el->xy);
// 		memcpy(el->xy, rec->xy, rec->size-4);
// 		rec = gds_read(gds);
// 	}

// 	if (type == GDS_RT_TEXT) {
// 		assert(rec->type == GDS_RT_STRING);
// 		el->text.string = strndup(rec->string, rec->size-4);
// 		rec = gds_read(gds);
// 	}

// 	while (rec && rec->type != GDS_RT_ENDEL)
// 		rec = gds_read(gds);
// 	return el;
// }

// static struct structure *process_structure(struct gds_reader *gds, struct gds_record *rec) {
// 	struct structure *str;
// 	struct element **els;
// 	size_t num_els, cap_els;
// 	unsigned num_commands = 0;

// 	str = malloc(sizeof(*str));
// 	memset(str, 0, sizeof(*str));

// 	rec = gds_read(gds);

// 	assert(rec && rec->type == GDS_RT_STRNAME);
// 	str->name = strndup(rec->strname, rec->size-4);
// 	printf("structure '%s'\n", str->name);
// 	rec = gds_read(gds);

// 	if (rec->type == GDS_RT_STRCLASS)
// 		rec = gds_read(gds);

// 	cap_els = 4;
// 	num_els = 0;
// 	els = malloc(cap_els * sizeof(*els));
// 	assert(els);

// 	while (rec && rec->type != GDS_RT_ENDSTR) {
// 		if (num_els == cap_els) {
// 			cap_els *= 2;
// 			els = realloc(els, cap_els * sizeof(*els));
// 		}
// 		els[num_els] = process_element(gds, rec);
// 		++num_els;
// 		rec = gds_read(gds);
// 	}

// 	str->elements = realloc(els, num_els * sizeof(*els));
// 	str->num_elements = num_els;

// 	return str;
// }

// static void plot_element(struct element *el, cairo_t *cr) {
// 	unsigned i;
// 	assert(el && cr);


// 	if (el->kind == ELEMENT_BOUNDARY) {
// 		double scale = 5e-6;
// 		assert(el->num_xy >= 4);
// 		cairo_move_to(cr, el->xy[0].x*scale, -el->xy[0].y*scale);
// 		for (i = 1; i < el->num_xy; ++i)
// 			cairo_line_to(cr, el->xy[i].x*scale, -el->xy[i].y*scale);

// 		cairo_set_source_rgba(cr,
// 			((el->layer / 1) % 4) * 0.25,
// 			((el->layer / 4) % 4) * 0.25,
// 			((el->layer / 8) % 4) * 0.25,
// 			0.25
// 		);
// 		cairo_fill_preserve(cr);

// 		cairo_set_source_rgba(cr,
// 			((el->layer / 1) % 4) * 0.25,
// 			((el->layer / 4) % 4) * 0.25,
// 			((el->layer / 8) % 4) * 0.25,
// 			1.0
// 		);
// 		cairo_stroke(cr);
// 	}

// 	if (el->kind == ELEMENT_TEXT) {
// 		double scale = 5e-6;
// 		cairo_move_to(cr, el->xy[0].x*scale, -el->xy[0].y*scale);

// 		cairo_set_source_rgba(cr,
// 			((el->layer / 1) % 4) * 0.25,
// 			((el->layer / 4) % 4) * 0.25,
// 			((el->layer / 8) % 4) * 0.25,
// 			1.0
// 		);
// 		cairo_show_text(cr, el->text.string);
// 	}
// }

// static void plot_structure(struct structure *str, cairo_t *cr) {
// 	unsigned i;

// 	assert(str && cr);
// 	for (i = 0; i < str->num_elements; ++i) {
// 		plot_element(str->elements[i], cr);
// 	}
// }

static void process(const char *filename) {
	struct gds_reader *gds;
	struct gds_record *rec;
	struct structure **strs;
	size_t num_strs, cap_strs;
	int err;
	unsigned i,n,t;

	err = gds_reader_open_file(&gds, filename, 0);
	if (err != GDS_OK) {
		fprintf(stderr, "Unable to open GDS file \"%s\": %s (error %d)\n", filename, gds_errstr(err), err);
		return;
	}

	cap_strs = 4;
	num_strs = 0;
	strs = malloc(cap_strs * sizeof(*strs));
	assert(strs);

	while ((err = gds_read(gds, &rec)) == GDS_OK && rec->type != GDS_RT_ENDLIB) {
		printf("record { size = %04x, type = %04x (%s, %s) }\n",
			rec->size,
			rec->type,
			gds_record_type_str(rec->type),
			gds_data_type_str(rec->type & 0xFF)
		);

		if (rec->type == GDS_RT_UNITS) {
			double dbu_in_uu = gds_real64_to_double(rec->data.units.dbu_in_uu);
			double dbu_in_m = gds_real64_to_double(rec->data.units.dbu_in_m);
			printf("units = {\n\tdbu_in_uu = 0x%016lx (%g),\n\tdbu_in_m  = 0x%016lx (%g)\n}\n", rec->data.units.dbu_in_uu, dbu_in_uu, rec->data.units.dbu_in_m, dbu_in_m);
		}

		// if (rec->type == GDS_RT_BGNSTR) {
		// 	if (num_strs == cap_strs) {
		// 		cap_strs *= 2;
		// 		strs = realloc(strs, cap_strs * sizeof(*strs));
		// 	}
		// 	strs[num_strs] = process_structure(gds, rec);
		// 	++num_strs;
		// }
	}

	if (err != GDS_OK) {
		fprintf(stderr, "Error while reading GDS file \"%s\": %s (error %d)\n", filename, gds_errstr(err), err);
		goto cleanup_strs;
	}

	// strs = realloc(strs, num_strs * sizeof(strs));

	// for (i = 0; i < num_strs; ++i) {
	// 	for (n = 0; n < strs[i]->num_elements; ++n) {
	// 		struct element *el = strs[i]->elements[n];
	// 		if (el->kind != ELEMENT_SREF && el->kind != ELEMENT_AREF)
	// 			continue;

	// 		for (t = 0; t < num_strs; ++t) {
	// 			if (strcmp(strs[t]->name, el->sref.sname) == 0)
	// 				break;
	// 		}

	// 		if (t < num_strs) {
	// 			++strs[t]->refs;
	// 			el->sref.ptr = strs[t];
	// 		}
	// 	}
	// }

	// printf("parsed %u structures:\n", num_strs);
	// for (i = 0; i < num_strs; ++i) {
	// 	printf("- %s (%u refs)\n", strs[i]->name, strs[i]->refs);
	// }

	// for (i = 0; i < num_strs; ++i) {
	// 	struct structure *str = strs[i];
	// 	// if (str->refs == 0) {
	// 		cairo_surface_t *surf;
	// 		cairo_t *cr;
	// 		char svg_filename[256], png_filename[256];

	// 		printf("plotting structure '%s'\n", str->name);

	// 		strcpy(svg_filename, str->name);
	// 		strcpy(png_filename, str->name);
	// 		strcat(svg_filename, ".svg");
	// 		strcat(png_filename, ".png");

	// 		surf = cairo_svg_surface_create(svg_filename, 1000, 1000);
	// 		// surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 1000, 1000);
	// 		cr = cairo_create(surf);

	// 		cairo_set_source_rgb(cr,1,1,1);
	// 		cairo_paint(cr);

	// 		cairo_set_line_width(cr, 1.0);
	// 		cairo_translate(cr,300.5,800.5);

	// 		cairo_save(cr);
	// 		cairo_set_dash(cr, (double[]){8.0,4.0}, 2, 0.0);
	// 		cairo_set_source_rgb(cr,0.5,0.5,0.5);
	// 		cairo_move_to(cr, -100000, 0);
	// 		cairo_line_to(cr,  100000, 0);
	// 		cairo_stroke(cr);
	// 		cairo_move_to(cr, 0, -100000);
	// 		cairo_line_to(cr, 0,  100000);
	// 		cairo_stroke(cr);
	// 		cairo_restore(cr);

	// 		plot_structure(str, cr);

	// 		cairo_surface_write_to_png(surf, png_filename);
	// 		cairo_destroy(cr);
	// 		cairo_surface_destroy(surf);
	// 	// }
	// }

cleanup_strs:
	free(strs); strs = NULL;
cleanup:
	gds_reader_close(gds);
}

int main(int argc, char **argv) {
	int i;

	if (argc < 2) {
		fprintf(stderr, "usage: %s FILE ...\n", *argv);
		return 1;
	}

	for (i = 1; i < argc; ++i) {
		process(argv[i]);
	}

	return 0;
}
