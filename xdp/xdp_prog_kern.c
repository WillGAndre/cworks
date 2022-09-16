#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include <arpa/inet.h>
#include "common/parsing_helpers.h"
#include "common/rewrite_helpers.h"

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <stdbool.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include <linux/pkt_cls.h>


static inline unsigned short checksum(unsigned short *buf, int bufsz) {
    unsigned long sum = 0;

    while (bufsz > 1) {
        sum += *buf;
        buf++;
        bufsz -= 2;
    }

    if (bufsz == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

SEC("xdp")
int xdp_p1(struct xdp_md *ctx) {
	//const char str[] = "got a egress packet\n";
	//const char res[] = "tcp  port 4444 or 9090\n";
	void *data = (void *)(long)ctx->data;
	void *data_end = (void *)(long)ctx->data_end;
	struct ethhdr *eth = data;

	if ((void*)eth + sizeof(*eth) <= data_end) {
		struct iphdr *ip = data + sizeof(*eth);
		if ((void*)ip + sizeof(*ip) <= data_end) {
			if (ip->protocol == IPPROTO_TCP) {
				struct tcphdr *tcp = (void*)ip + sizeof(*ip);
				if ((void*)tcp + sizeof(*tcp) <= data_end) {

					if (tcp->dest == ntohs(4444)) {
						tcp->dest = ntohs(4445);
						// tcp->source = ntohs(9090);
						tcp->check=0;
						tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr));

						return XDP_TX;
					} else if (tcp->dest == ntohs(9090)) {
						tcp->dest = ntohs(9091);
						tcp->check=0;
						tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr));

						return XDP_TX;
					}
				}
			}
		}
	}

	return XDP_PASS;
}

SEC("action")
int tc_main(struct __sk_buff *skb) {
	const char str[] = "got a egress 4445 src packet\n";
	void *data = (void *)(long)skb->data;
	void *data_end = (void *)(long)skb->data_end;
	struct ethhdr *eth = data;

	if ((void*)eth + sizeof(*eth) <= data_end) {
		struct iphdr *ip = data + sizeof(*eth);

		if ((void*)ip + sizeof(*ip) <= data_end) {
			if (ip->protocol == IPPROTO_TCP) {
				struct tcphdr *tcp = (void*)ip + sizeof(*ip);

				if ((void*)tcp + sizeof(*tcp) <= data_end) {
					if (tcp->source == ntohs(4445)) {
						bpf_trace_printk(str, sizeof(str));
						
						tcp->source = ntohs(4444);
						tcp->check=0;
						tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr));	
					} else if (tcp->source == ntohs(9091)) {
						tcp->source = ntohs(9090);
						tcp->check=0;
						tcp->check = checksum((unsigned short *)tcp, sizeof(struct tcphdr));
					}
				}
			}
		}
	}

	return TC_ACT_OK;
}

char _license[] SEC("license") = "GPL";