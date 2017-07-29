#include <pcap.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "test.c"

#define WSMP_MODE WSMP_STRICT

pcap_t *wsmp_if_init(const char *ifname) {
     char pcap_errbuf[PCAP_ERRBUF_SIZE];
     pcap_errbuf[0] = '\0';

     pcap_t *pcap = pcap_open_live(ifname, 96, 0, 0, pcap_errbuf);

     if (pcap_errbuf[0] != '\0') {
	  fprintf(stderr, "ERROR:\twsmp_if_init | pcap_errbuf[0] == %s\n", pcap_errbuf);
     }

     if (!pcap) {
	  return NULL;
     }

     return pcap;
}

int wsmp_if_send(const struct wsmp_wsm *msg, const char *ifname, pcap_t *pcap) {
     struct ether_header hdr;
     hdr.ether_type = htons(0x88DC);
     memset(hdr.ether_dhost, 0xFF, sizeof(hdr.ether_dhost));

     struct ifreq ifr;
     size_t ifname_len = strlen(ifname);

     if (ifname_len < sizeof(ifr.ifr_name)) {
	  memcpy(ifr.ifr_name, ifname, ifname_len);
	  ifr.ifr_name[ifname_len] = 0;
     } else {
	  fprintf(stderr, "ERROR:\twsmp_if_send | !(ifname_len < sizeof(ifr.ifr_name))\n");
	  return EXIT_FAILURE;
     }

     int fd = socket(AF_INET, SOCK_DGRAM, 0);

     if (fd == -1) {
	  perror(0);
	  return EXIT_FAILURE;
     }

     if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
	  perror(0);
	  close(fd);
	  return EXIT_FAILURE;
     }

     // ARPHRD_ETHER := 1
     if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
	  fprintf(stderr, "ERROR:\twsmp_if_send | ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER\n");
	  close(fd);
	  return EXIT_FAILURE;
     }

     const unsigned char *src_mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
     memcpy(hdr.ether_shost, src_mac, sizeof(hdr.ether_shost));
     close(fd);


     size_t wsm_cnt = 0;
     int wsm_err = 0;
     uint8_t *wsm = wsmp_wsm_encode(msg, &wsm_cnt, &wsm_err, WSMP_MODE);

     if (wsm_err) {
	  fprintf(stderr, "ERROR:\twsmp_if_send | wsm_err\n");
	  return EXIT_FAILURE;
     }

     unsigned char frame[sizeof(struct ether_header) + wsm_cnt];
     memcpy(frame, &hdr, sizeof(struct ether_header));
     memcpy(frame + sizeof(struct ether_header), wsm, wsm_cnt);

     if (pcap_inject(pcap, frame, sizeof(frame)) == -1) { // BREAK HERE
	  pcap_perror(pcap, 0);
	  return EXIT_FAILURE;
     }

     return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
     if (argc != 2) {
	  fprintf(stderr, "Usage: wireless <ifname>\n");
	  return EXIT_FAILURE;
     }

     pcap_t *pcap = wsmp_if_init(argv[1]);

     if (pcap == NULL) {
	  fprintf(stderr, "Failed to open pcap interface.\n");
	  return EXIT_FAILURE;
     }

     int i;
     for (i = 0; i < 100; i++) {
	  struct wsmp_wsm *msg = gen_wsm(WSMP_STRICT);
	  printf("%d\n", wsmp_if_send(msg, argv[1], pcap));
	  free_wsm(msg);
     }

     pcap_close(pcap);

     return EXIT_SUCCESS;
}
