#ifndef __NETWORK_DATA__
#define __NETWORK_DATA__

#define BOXES_STATS			0x06
#define INPUT_ARCS			0x07
#define OUTPUT_ARCS			0x08
#define NUM_TUPLES			0x09
#define LOADSHEDDER_CURVE	0x0a
#define STR_TYPE			0x0b
#define DROPS_ON_ARCS		0x0c

#define QUERY_NAME			0x01
/* server -> client packets */
typedef struct data_packet_s {
  char _type;
  char _data[0];
} data_packet_t;

typedef struct map_packet_s {
  int	_key_size;
  int	_entry_size;
  int	_num_entries;
  char	_data[0];
} map_packet_t;

typedef struct map_element_s {
  int	_element_size;
  char	_data[0];
} map_element_t;

typedef struct vector_packet_s {
  int	_type_size;
  int	_num_entries;
  char	_data[0];
} vector_packet_t;

typedef struct str_packet_s {
  char	_info;
  int	_len;
  char	_data[0];
} str_packet_t;

#define LOADSHEDDER_VALUE	0x05

/* client -> server packets */
typedef struct server_data_s {
  char _type;
  char _data[0];
} server_data_t;

typedef struct load_shedder_s {
  float	_value;
} load_shedder_t;


#endif
