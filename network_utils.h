#ifndef __NETWORK_UTILS_H__
#define __NETWORK_UTILS_H__


char *
network_convert_ip_n_to_p(uint32_t ip_addr,
                        char *output_buffer);

uint32_t
network_covert_ip_p_to_n(const char *ip_addr);

#endif