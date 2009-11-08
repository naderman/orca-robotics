clear all
format compact

directory = 'tango_send_rec_static_46k_1s_ram_to_hdd';

fileprefix = [directory '/histogram_send_'];
[minMaxSend, minMaxDevSend] = parse_histogram( fileprefix )

fileprefix = [directory '/histogram_rec_'];
[minMaxRec, minMaxDevRec] = parse_histogram( fileprefix )
