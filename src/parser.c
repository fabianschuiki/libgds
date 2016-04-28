/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"

struct gds_parser {
	gds_reader_t *rd;
	gds_record_t *rec;
	int err;
};


gds_parser_t *
gds_parser_create(gds_reader_t *rd) {
	assert(rd);
	gds_parser_t *parser = calloc(1, sizeof(*parser));
	parser->rd = rd;
	gds_parser_next(parser);
	return parser;
}


void
gds_parser_destroy(gds_parser_t *parser) {
	assert(parser);
	free(parser);
}


int
gds_parser_next(gds_parser_t *parser) {
	assert(parser);

	if (parser->err != GDS_OK)
		return parser->err;

	int err = gds_read(parser->rd, &parser->rec);
	if (err != GDS_OK)
		parser->err = err;

	return err;
}


int
gds_parser_get_error(gds_parser_t *parser) {
	assert(parser);
	return parser->err;
}


gds_record_t *
gds_parser_get_record(gds_parser_t *parser) {
	assert(parser);
	return parser->rec;
}


char *
gds_parser_copy_string(gds_parser_t *parser) {
	assert(parser && parser->rec && (parser->rec->type & 0xFF) == GDS_DT_STRING && parser->rec->size > 4);
	size_t len = parser->rec->size-4;
	char *str = malloc(len+1);
	memcpy(str, &parser->rec->data, len);
	str[len] = 0;
	return str;
}


int
gds_parser_require(gds_parser_t *parser, uint16_t rt) {
	assert(parser);

	if (parser->err != GDS_OK)
		return 0;

	if (!parser->rec) {
		fprintf(stderr, "Expected %s record, but file ended\n", gds_record_type_str(rt));
		parser->err = GDS_ERR_SYNTAX;
		return 0;
	}
	if (parser->rec->type != rt) {
		fprintf(stderr, "Expected %s record, but got %s\n", gds_record_type_str(rt), gds_record_type_str(parser->rec->type));
		parser->err = GDS_ERR_SYNTAX;
		return 0;
	}

	return 1;
}


int
gds_parser_accept(gds_parser_t *parser, uint16_t rt) {
	assert(parser);
	if (parser->err != GDS_OK)
		return 0;
	return parser->rec->type == rt;
}


int
gds_parser_skip_until(gds_parser_t *parser, uint16_t rt) {
	assert(parser);

	if (parser->err != GDS_OK)
		return 0;

	do {
		if (!parser->rec) {
			fprintf(stderr, "Unexpected end of file while searching for %s record\n", gds_record_type_str(rt));
			parser->err = GDS_ERR_SYNTAX;
			return 0;
		}
		if (parser->rec->type == rt)
			return 1;
		gds_parser_next(parser);
	} while (parser->err == GDS_OK);

	return 0;
}
