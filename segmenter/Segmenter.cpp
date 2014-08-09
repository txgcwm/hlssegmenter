/*
 * Segmenter.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: satram
 */

#include "Segmenter.h"

Segmenter::Segmenter(ConfigParams &config)
{
	hls_playlist = new HlsPlaylistGenerator();
	hls_playlist->generate_header(config);
	ts_packet_count = 0;
	byte_offset = 0;
}

Segmenter::~Segmenter()
{
	for(auto it = index.begin(), ite = index.end(); it != ite; it++)
		delete (*it);

	if(hls_playlist)
		delete hls_playlist;
}

void Segmenter::create_index_table()
{
    std::list<VideoPktInfo*> pkt_list = input_ts_stream.get_pkt_info();
    for(auto it = pkt_list.begin(), ite = pkt_list.end();it != ite;it++)
    {
        if((*it)->PUSI_flag)
        {
        	int accum_pktcount = ts_packet_count + (*it)->pkt_count;
        	long long accum_byteoffset = (*it)->byte_offset + byte_offset;
        	long long timestamp = (*it)->dts;
            for(int i = 0;i < (*it)->detected_slice_type_count;i++)
            {
                if((*it)->slice_type[i].first == idr)
                {
                	IFrameIndex * last_iframe = index.back();
                	if(last_iframe != 0)
                	{
                		last_iframe->finalize(accum_pktcount, timestamp , accum_byteoffset);
                	}
                    index.push_back(new IFrameIndex(accum_pktcount, timestamp , accum_byteoffset));
                }
                else if ((*it)->slice_type[i].first == non_idr)
                {
                	IFrameIndex * last_iframe = index.back();
                	last_iframe->update(accum_pktcount, timestamp , accum_byteoffset);
                }
            }
        }
    }
}

void Segmenter::update_playlists()
{
	for(auto it = index.begin(), ite = index.end();it != ite;it++)
	{
		if((*it)->flags.update_media_playlist.first && !(*it)->flags.update_media_playlist.second)
		{
			hls_playlist->update_media(*it);
			(*it)->flags.update_media_playlist.second = true;
		}
		if((*it)->flags.update_iframe_playlist.first && !(*it)->flags.update_iframe_playlist.second)
		{
			hls_playlist->update_iframe(*it);
			(*it)->flags.update_iframe_playlist.second = true;
		}
	}
}



void Segmenter::parse_ts_packets(const char *inp_buffer, int bufsize)
{
    input_ts_stream.open(inp_buffer, bufsize);
    input_ts_stream.parse_bytestream();

    create_index_table();
    update_playlists();

    ts_packet_count += input_ts_stream.get_num_packets();
    byte_offset += bufsize;

    std::cout << "ts_packet_count " << ts_packet_count << std::endl;

    input_ts_stream.close();
}


