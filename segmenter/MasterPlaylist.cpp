/*
 * MasterPlaylist.cpp
 *
 *  Created on: Aug 6, 2014
 *      Author: satram
 */

#include "MasterPlaylist.h"

MasterPlaylist::MasterPlaylist() {
	// TODO Auto-generated constructor stub

}

MasterPlaylist::~MasterPlaylist() {
	// TODO Auto-generated destructor stub
}

void MasterPlaylist::remove_node()
{
}

void MasterPlaylist::set_url(std::string &path, std::string &filename)
{
	playlist_path = path;
	playlist_name = filename;
}

void MasterPlaylist::add_header(ConfigParams & config)
{
	Section header("header");
	header.add_tag("M3U");
	header.add_tag("VERSION", 3);
	playlist.add_section(header);

	for(auto it = config.variant_streams.begin(), ite= config.variant_streams.end(); it != ite; it++)
	{
		std::ostringstream oss;
		oss << "\"";
		if(it->vid.codec == h264video)
			oss << "avc1";
		if(it->vid.profile == "baseline" && it->vid.level == 3.1)
			oss << ".42001f";
		if(it->aud.codec == mpeg4audio_latm)
			oss << ",mp4a";
		if(it->aud.subtype == "aac-lc")
			oss << ".40.2";
		oss << "\"";
		{
			Section node("n1");
			Tag t("STREAM-INF");
			t.add_property("BANDWIDTH", it->bandwidth);
			t.add_property("RESOLUTION",it->vid.resolution);
			t.add_property("CODECS",oss.str());
			node.add_tag(t);
			node.set_path((config.web_server_url + it->id).c_str());
			node.set_locator("media.m3u8");
			playlist.add_section(node);
		}

		{
			Section node("n1");
			Tag t("I-FRAME-STREAM-INF");
			t.add_property("BANDWIDTH", 512000);
			t.add_property("RESOLUTION","1280x720");
			t.add_property("CODECS",oss.str());
			t.add_property("URI",config.web_server_url + it->id + "/iframe.m3u8");
			node.add_tag(t);
			playlist.add_section(node);
		}
	}
}



void MasterPlaylist::publish_playlist()
{
//	std::cout << "-----------------Master playlist-----------------" << std::endl;
//	std::cout << playlist.marshall();
	std::ofstream file;
	file.open(playlist_path + playlist_name, std::ofstream::out);
	if(file.is_open())
	{
		file << playlist.marshall();
		file.close();
	}
}



void MasterPlaylist::add_node(IFrameIndex *index, VariantPlaylist *variant_playlist)
{
}



void MasterPlaylist::add_header(variant_stream_info & stream_info)
{
}

void MasterPlaylist::add_footer()
{
}




