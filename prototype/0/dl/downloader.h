#ifndef DL_DOWNLOADER_H_
#define DL_DOWNLOADER_H_

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Multi.hpp>
#include <fstream>
#include <string>
#include "downloadlistener.h"

namespace dl {

/*!
 * Use libcurl to download files from a server.
 *
 * \author Harry Slatyer
 * \author Malcolm Gill
 *
 * \copyright see LICENSE
 */
class Downloader {
	private:
		curlpp::Cleanup clean_;
		curlpp::Easy request_;
		curlpp::Multi multi_;
		curlpp::types::ProgressFunctionFunctor prog_func_;
		std::fstream output_;
		bool downloading_;

		double dlsize_, dlcur_;

		dl::DownloadListener* listener_;

		std::string url_;

		void finish_download();
	public:
		Downloader(dl::DownloadListener* l=0);

		void set_listener(dl::DownloadListener*);

		void set_url(const std::string&);
		void start_download(const std::string&,const std::string&);
		void abort_download();
		bool downloading();

		double dlsize();
		double dlcur();

		bool update();

		double progress_callback(double,double,double,double);
};

}

#endif
